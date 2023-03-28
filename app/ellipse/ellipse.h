#ifndef ELLIPSE_H
#define ELLIPSE_H

#include <QObject>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_canvas.h>
#include <QThread>
#include <memory>

struct Ellipsoid
{
    Ellipsoid() {}
    double X0;
    double Y0;
    double Z0;
    double AX;
    double BY;
    double CZ;
};

struct EllipsePoints
{
    EllipsePoints() {}
    double X;
    double Y;
    double Z;
};

class EllipseComputingThread;

class Ellipse : public QwtPlot
{
    Q_OBJECT
public:
    enum Axis
    {
        XAxis,
        YAxis,
        ZAxis
    };

    explicit Ellipse(QWidget *parent = nullptr);
    ~Ellipse();

    void setUpdateThreshold(int num);
    void drawScatter(double dx[], double dy[], double dz[], int size, Ellipse::Axis axis);
    void setTitle(const QString &title, const Ellipse::Axis axis);

signals:
    void addPoint(EllipsePoints point);

public slots:
    void updatePoints(double dx, double dy, double dz, Ellipse::Axis axis);

    void clear();

private slots:
    void onEllipseUpdate(QPolygonF points, QPolygonF center);

private:
    QwtPlotCurve *m_pEllipseCurve;
    QwtPlotCurve *m_pCircleCurve;
//    QwtPlotCurve *m_pScatterCurve;
    QwtPlotCanvas *m_pCanvas;

    volatile bool m_computing_enable;
    Ellipse::Axis m_axis;
    EllipseComputingThread* m_thread;
};


class EllipseComputingThread : public QThread
{
    Q_OBJECT
public:
    explicit EllipseComputingThread(Ellipse::Axis axis);
    ~EllipseComputingThread();

signals:
    void updateEllipseFactor(QPolygonF points, QPolygonF center);

public slots:
    void onAddPoint(EllipsePoints point);

protected:
    virtual void run() override;

private:
    void computing();

    Ellipse::Axis m_axis;
    uint32_t m_threshold;
    // 原始数据集合
    QList<EllipsePoints> m_points;
};

#endif // ELLIPSE_H
