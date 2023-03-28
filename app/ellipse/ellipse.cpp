#include "ellipse.h"
#include <qwt_plot_canvas.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <QDebug>
#include "ellipsoid_fitting.h"
#include <QMetaType>
#include <QThread>
#include "assert.h"

EllipseComputingThread::EllipseComputingThread(Ellipse::Axis axis)
    : m_axis(axis),
      m_threshold(6)
{

}

EllipseComputingThread::~EllipseComputingThread()
{

}

void EllipseComputingThread::onAddPoint(EllipsePoints point)
{
    m_points << point;

    if (m_points.size() >= m_threshold)
    {
        computing();
    }
}


void EllipseComputingThread::run()
{
    exec();
}

void EllipseComputingThread::computing()
{
    double result[6] = {0};
    Ellipsoid_fitting_Process(m_points, result);

    Ellipsoid ellipsoid;

    ellipsoid.X0 = result[0];
    ellipsoid.Y0 = result[1];
    ellipsoid.Z0 = result[2];
    ellipsoid.AX = result[3];
    ellipsoid.BY = result[4];
    ellipsoid.CZ = result[5];

    qDebug() << "x0=" << ellipsoid.X0 << "," << "y0=" << ellipsoid.Y0 << ","<< "z0=" << ellipsoid.Z0 << ","
             << "a=" << ellipsoid.AX << ","<< "b=" << ellipsoid.BY << ","<< "c=" << ellipsoid.CZ;

    QPolygonF points, center;
    bool isOk = false;

    if (m_axis == Ellipse::XAxis && !isnan(ellipsoid.BY) && !isnan(ellipsoid.CZ))
    {
        qDebug() << "around XAxis\n";

        qreal ry, rz;
        for(float i = 0; i <= 2 * M_PI; i += 0.01)
        {
            ry = ellipsoid.Y0 + ellipsoid.BY * cos(i);
            rz = ellipsoid.Z0 + ellipsoid.CZ * sin(i);
            points << QPointF(ry, rz);
        }
        center << QPointF(ellipsoid.Y0, ellipsoid.Z0);

        isOk = true;
    }

    if (m_axis == Ellipse::YAxis && !isnan(ellipsoid.AX) && !isnan(ellipsoid.CZ))
    {
        qDebug() << "around YAxis\n";

        qreal rx, rz;
        for(float i = 0; i <= 2 * M_PI; i += 0.01)
        {
            rx = ellipsoid.X0 + ellipsoid.AX * cos(i);
            rz = ellipsoid.Z0 + ellipsoid.CZ * sin(i);
            points << QPointF(rx, rz);
        }
        center << QPointF(ellipsoid.X0, ellipsoid.Z0);

        isOk = true;
    }

    if (m_axis == Ellipse::ZAxis && !isnan(ellipsoid.AX) && !isnan(ellipsoid.BY))
    {
        qDebug() << "around ZAxis\n";

        qreal rx, ry;
        for(float i = 0; i <= 2 * M_PI; i += 0.01)
        {
            rx = ellipsoid.X0 + ellipsoid.AX * cos(i);
            ry = ellipsoid.Y0 + ellipsoid.BY * sin(i);
            points << QPointF(rx, ry);
        }
        center << QPointF(ellipsoid.X0, ellipsoid.Y0);

        isOk = true;
    }
    if (isOk)
    {
        emit updateEllipseFactor(points, center);
    }
}


Ellipse::Ellipse(QWidget *parent)
    : QwtPlot(parent),
      m_computing_enable(false),
      m_axis(Ellipse::XAxis),
      m_thread(nullptr)
{
    //---------设置画布---------//
    m_pCanvas = new QwtPlotCanvas(this);
    m_pCanvas->setPalette(Qt::white);
    m_pCanvas->setBorderRadius(10);
    setCanvas(m_pCanvas);
    plotLayout()->setAlignCanvasToScales(true);

    //-----------设置x，y坐标和范围--------------//
    //    setAxisTitle(QwtPlot::yLeft, "yLeftlabel");
    //    setAxisTitle(QwtPlot::xBottom, "xBottomlabel");
    //    setAxisScale(QwtPlot::yLeft, -2000.0, 2000.0);
    //    setAxisScale(QwtPlot::xBottom, -2000.0, 2000.0);

    //----------------设置栅格线-------------------//
    QwtPlotGrid *grid = new QwtPlotGrid;
    grid->enableX(true); //设置网格线
    grid->enableY(true);
    grid->setMajorPen(Qt::black, 0, Qt::SolidLine);
    grid->attach(this);

    m_pEllipseCurve = new QwtPlotCurve("EllipseCurve");
    m_pCircleCurve = new QwtPlotCurve("CircleCurve");
    //    m_pScatterCurve = new QwtPlotCurve("ScatterCurve");
    m_pEllipseCurve->setPen(Qt::green, 2); // 设置曲线颜色 粗细
    m_pEllipseCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);

    m_pCircleCurve->setPen(Qt::red, 2);
    m_pCircleCurve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    m_pCircleCurve->setStyle(QwtPlotCurve::Dots);

    QwtSymbol *symbol = new QwtSymbol(QwtSymbol::Ellipse, QBrush(Qt::red), QPen(Qt::red, 2), QSize(6, 6));
    m_pCircleCurve->setSymbol(symbol);

    qRegisterMetaType<Ellipse::Axis>("Ellipse::Axis");
    qRegisterMetaType<QList<QwtLegendData>>("QList<QwtLegendData>");

    this->resize(600,400);
}

Ellipse::~Ellipse()
{
    if (m_thread)
    {
        m_thread->quit();
        m_thread->deleteLater();
    }
}

void Ellipse::setUpdateThreshold(int num)
{
    //    m_threshold = num;
}

void Ellipse::drawScatter(double dx[], double dy[], double dz[], int size, Ellipse::Axis axis)
{
    //-----------------开始画散点----------------------//

    QwtPlotCurve *pScatterCurve = new QwtPlotCurve("ScatterCurve");
    pScatterCurve->setPen(Qt::blue, 3);//设置曲线颜色 粗细
    pScatterCurve->setStyle(QwtPlotCurve::Dots);

    if (axis == XAxis)
    {
        pScatterCurve->setSamples(dy, dz, size);
        pScatterCurve->attach(this);
    }
    else if (axis == YAxis)
    {
        pScatterCurve->setSamples(dx, dz, size);
        pScatterCurve->attach(this);
    }
    else if(axis == ZAxis)
    {
        pScatterCurve->setSamples(dx, dy, size);
        pScatterCurve->attach(this);
    }

    this->replot();
    //    setAutoReplot(true); // 设置自动重画，相当于更新
    //    repaint();
}

void Ellipse::updatePoints(double dx, double dy, double dz, Ellipse::Axis axis)
{
    EllipsePoints ep;
    ep.X = dx;
    ep.Y = dy;
    ep.Z = dz;

    drawScatter(&dx, &dy, &dz, 1, axis);

    emit addPoint(ep);
}

void Ellipse::clear()
{
    this->detachItems(QwtPlotItem::Rtti_PlotCurve, false);
}

void Ellipse::setTitle(const QString &title, const Axis axis)
{
    QwtPlot::setTitle(title);
    m_axis = axis;
    if (!m_thread)
    {
        m_thread = new EllipseComputingThread(m_axis);
        connect(m_thread, &EllipseComputingThread::updateEllipseFactor, this, &Ellipse::onEllipseUpdate);
        connect(this, &Ellipse::addPoint, m_thread, &EllipseComputingThread::onAddPoint);
        m_thread->start();
    }
}

void Ellipse::onEllipseUpdate(QPolygonF points, QPolygonF center)
{
    m_pEllipseCurve->setSamples(points);
    m_pEllipseCurve->attach( this );
    m_pEllipseCurve->setLegendAttribute(m_pEllipseCurve->LegendShowLine);

    m_pCircleCurve->setSamples(center);
    m_pCircleCurve->attach( this );
    m_pCircleCurve->setLegendAttribute(m_pEllipseCurve->LegendShowLine);

//    repaint();
    this->replot();
}
