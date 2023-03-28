#include "gradienter.h"
#include <algorithm>
#include <cmath>
#include <QPainter>
#include <QVector>
#include <QTimer>
#include <QDebug>

using namespace std;

static const double PI = 3.1415926;

Gradienter::Gradienter(QWidget *parent)
    : QWidget(parent), m_deg_value(0.0), m_roll_value(0.0)
{
//    auto timer = new QTimer(this);
//    connect(timer, SIGNAL(timeout()), this, SLOT(updateValues()));
//    timer->start(100);
}

Gradienter::~Gradienter()
{
}

void Gradienter::setDegValue(qreal value)
{
    m_deg_value = value;
    update();
}

void Gradienter::setRollValue(qreal value)
{
    m_roll_value = value;
    update();
}

void Gradienter::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    auto width = this->width();
    auto height = this->height();
    auto side = min(width, height);

    // 绘制准备工作, 启用反锯齿, 平移坐标轴中心, 等比例缩放
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter.translate(width / 2, height / 2);
    painter.scale(side / 200.0, side / 200.0);

    // 绘制外边框
    drawBorderOut(painter);
    // 绘制内边框
//    drawBorderIn(painter);
    // 绘制背景
    drawBg(painter);
    // 绘制姿势仪背景
//    drawPlane(painter);
    drawGlass(painter);
    drawScale(painter);
    drawLine(painter);
    drawPointer(painter);
//    drawHandle(painter);
}

void Gradienter::drawBorderOut(QPainter &painter)
{
    uint32_t radius = 99;
    painter.save();
    painter.setPen(Qt::NoPen);

    auto borderGradient = QLinearGradient(0, -radius, 0, radius);
    borderGradient.setColorAt(0, Qt::black);
    borderGradient.setColorAt(1,  QColor(0x33, 0x33, 0x33));
//    borderGradient.setColorAt(1,  QColor(23, 149, 166));
    painter.setBrush(borderGradient);
//    painter.setBrush(QColor(0x33, 0x33, 0x33));
    painter.drawEllipse(-radius, -radius, radius * 2, radius * 2);
    painter.restore();
}

void Gradienter::drawBorderIn(QPainter &painter)
{
    uint32_t radius = 87;
    painter.save();
    painter.setPen(Qt::NoPen);
    auto borderGradient = QLinearGradient(0, -radius, 0, radius);
    borderGradient.setColorAt(0, QColor(50, 50, 50));
    borderGradient.setColorAt(1, QColor(220, 220, 220));
    painter.setBrush(borderGradient);
    painter.drawEllipse(-radius, -radius, radius * 2, radius * 2);
    painter.restore();
}

void Gradienter::drawBg(QPainter &painter)
{
    uint32_t radius = 83;
    painter.save();
    painter.setPen(Qt::NoPen);

    auto borderGradient = QLinearGradient(0, -radius, 0, radius);
//    auto bgColor = QColor(41, 127, 184);
//    auto bgColor = QColor("#007AFF");
//    auto bgColor = QColor("#3399FF");
    auto bgColor = QColor("#1E90FF");
//    bgColor.setAlpha(255);
//    borderGradient.setColorAt(0, bgColor);
//    bgColor.setAlpha(150);
//    borderGradient.setColorAt(1, bgColor);
//    painter.setBrush(borderGradient);
    painter.setBrush(bgColor);
    painter.drawEllipse(-radius, -radius, radius * 2, radius * 2);
    painter.restore();
}

void Gradienter::drawPlane(QPainter &painter)
{
    uint32_t radius = 80;
    painter.save();
    painter.rotate(m_deg_value);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#d68070"));
    auto rect = QRect(-radius, -radius, radius * 2, radius * 2);

    qreal offset = -(m_roll_value * radius / 60);
    qreal startAngle = 180 + offset;
    qreal endAngle = offset;
    qreal span = endAngle + startAngle;
    painter.drawChord(rect, -16 * startAngle, 16 * span);
    painter.restore();
}

void Gradienter::drawGlass(QPainter &painter)
{
    uint32_t radius = 75;
    painter.save();
    painter.setPen(Qt::NoPen);

    painter.rotate(m_deg_value);

    // 饼圆1为整个圆形区域
    QRect pieRect1 = QRect(-radius, -radius, radius * 2, radius * 2);
    // 饼圆2位饼圆1区域的中间部分的四分之一
    QRect pieRect2 = QRect(-radius, -radius / 4, radius * 2, radius / 4 * 2);
    //  颜色线性渐变, 颜色透明度产生立体感
    auto linearGradient = QLinearGradient(pieRect1.topLeft(), pieRect1.bottomRight());
    auto glassColor = QColor("#14DE32");
//    glassColor.setAlpha(30);
    linearGradient.setColorAt(0.1, glassColor);
//    glassColor.setAlpha(100);
    linearGradient.setColorAt(0.5, glassColor);
    // 绘制两个饼圆
    painter.setBrush(linearGradient);
    painter.drawPie(pieRect1, 0, 16 * 180);
    painter.drawPie(pieRect2, 0, -16 * 180);
    painter.restore();
}

void Gradienter::drawScale(QPainter &painter)
{
    uint32_t radius = 92;
    painter.save();
    // 设置画笔颜色
    auto pen = QPen();
    auto scaleColor = QColor(255, 255, 255);
    pen.setColor(scaleColor);
    pen.setWidthF(4);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    // 线条长度
    auto len = 8;
    // 左侧平行线
    painter.rotate(90);
    painter.drawLine(0, radius - len, 0, radius);
    // 逐个绘制大刻度
    for(auto i = 1; i <= 6; ++i)
    {
        painter.rotate(30);
        painter.drawLine(0, radius - len, 0, radius);
    }

    // 回到小刻度,重新设置画笔宽度
    pen.setWidthF(2);
    painter.setPen(pen);
    painter.rotate(-120);
    // 逐个绘制小刻度
    for(auto i = 1; i <= 5; ++i)
    {
        painter.rotate(10);
        painter.drawLine(0, radius - len, 0, radius);
    }
    painter.restore();
}

void Gradienter::drawLine(QPainter &painter)
{
    painter.save();
    auto width = this->width();
    auto height = this->height();
//    auto side = min(width, height);

    // 重置坐标系
    painter.resetTransform();
//    painter.translate(width / 2, height / 2 + m_roll_value);
    painter.translate(width / 2,
                      height / 2);
//    painter.translate(0, m_roll_value);
//    painter.scale(side / 200.0, side / 200.0);

    painter.rotate(m_deg_value);

    auto radius = 66;
    QRegion circle_region(QRect(-radius, -radius, radius * 2, radius * 2));
    painter.setClipRegion(circle_region);

    radius = 90;

    // 设置画笔颜色
    auto pen = QPen();
    auto lineColor = QColor(255, 255, 255);
    pen.setColor(lineColor);
    pen.setWidthF(2);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);

    auto pen_for_zero = pen;
    pen_for_zero.setColor(QColor(0, 0, 0));
    pen_for_zero.setWidthF(4);

    // 设置画笔字体
    auto font = QFont();
    font.setPixelSize(10);
    font.setBold(true);
    painter.setFont(font);


    // 依次绘制水平距离线条及值

//    qDebug() << "roll :" << m_roll_value;
    auto step = radius / 25;
    auto i = -90;
    while (i <= 90)
    {
        auto value = qAbs(i);

        auto pos_y = (i + m_roll_value) * step;
        /*
        if ((pos_y > radius) || (pos_y < -radius))
        {
            i += 10;
            continue;
        }
        */

        if (i == 0)
        {
            painter.setPen(pen_for_zero);
        }
        else
        {
            painter.setPen(pen);
        }

        auto pt1 = QPointF(-radius / 2, pos_y);
        auto pt2 = QPointF(-pt1.x(), pt1.y());
        painter.drawLine(pt1, pt2);

        if (i != -90)
        {
            painter.setPen(pen);
            auto pt3 = QPointF(pt1.x()/2, pt1.y() - 5 * step);
            auto pt4 = QPointF(pt2.x()/2, pt2.y() - 5 * step);
            painter.drawLine(pt3, pt4);
        }

        // 根据字体大小计算绘制文字的区域
        auto strValue = QString::number(int(value));
        auto fm = painter.fontMetrics();
        auto textRect = QRectF(QPointF(QPoint(0, 0)), QSizeF(fm.size(Qt::AlignCenter, strValue)));
        textRect.moveCenter(pt1 - QPointF(radius / 10, 0));
        painter.drawText(textRect, Qt::AlignCenter, strValue);

        textRect.moveCenter(pt2 + QPointF(radius / 10, 0));
        painter.drawText(textRect, Qt::AlignCenter, strValue);
        i += 10;
    }

    painter.restore();
}

void Gradienter::drawPointer(QPainter &painter)
{
    auto radius = 70;
    painter.save();
    painter.rotate(m_deg_value + 90);
    painter.setPen(Qt::NoPen);
    // 指针长度
    auto len = 10;
    // 三角形坐标
    auto pts = QPolygon();
    pts.setPoints(3, -radius, 0, -radius + len, -len / 2, -radius + len, len / 2);
    auto pointerColor = QColor("#FEFF05");
    painter.setBrush(pointerColor);
    painter.drawConvexPolygon(pts);
    painter.restore();
}

void Gradienter::drawHandle(QPainter &painter)
{
    painter.save();
    auto handleColor = QColor("gray");
    auto pen = QPen();
    pen.setColor(handleColor);
    pen.setWidthF(4);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::MiterJoin);
    painter.setPen(pen);
    painter.setBrush(handleColor);
    // 绘制弧形
    auto arcRadius = 15;
    auto rect = QRect(-arcRadius, -arcRadius, arcRadius * 2, arcRadius * 2);
    painter.drawArc(rect, 0, -16 * 180);
    // 绘制中心圆
    auto centerRadius = 2;
    rect = QRect(-centerRadius, -centerRadius, centerRadius * 2, centerRadius * 2);
    painter.drawEllipse(rect);

    // 绘制左侧右侧横向水平线条
    auto len = 30;
    auto pt1 = QPoint(-len, 0);
    auto pt2 = QPoint(-arcRadius, 0);
    painter.drawLine(pt1, pt2);

    pt1 = QPoint(len, 0);
    pt2 = QPoint(arcRadius, 0);
    painter.drawLine(pt1, pt2);

    // 绘制纵向垂直线条
    pt1 = QPoint(0, len / 2);
    pt2 = QPoint(0, 80);
    painter.drawLine(pt1, pt2);

    // 绘制底部梯形
    auto p1 = QPoint(-30, 82);
    auto p2 = QPoint(-20, 60);
    auto p3 = QPoint(20, 60);
    auto p4 = QPoint(30, 82);
    auto pts_vector = QVector<QPoint>({p1, p2, p3, p4});
    auto pts = QPolygon(pts_vector);

    painter.drawConvexPolygon(pts);
    // 绘制底部填充圆弧
    auto radius = 32;
    rect = QRect(-radius, 77, radius * 2, 13);
    painter.setPen(Qt::NoPen);
    painter.drawPie(rect, 0, -16 * 180);
    painter.restore();
}

void Gradienter::updateValues()
{
    static uint32_t count = 0;

    auto deg = 30 * sin(2 * PI * count / 36);
    auto roll = 30 * sin(2 * PI * count / 36);

    count += 1;

    setDegValue(deg);
    setRollValue(roll);
}
