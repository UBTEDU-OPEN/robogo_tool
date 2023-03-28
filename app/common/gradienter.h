#ifndef GRAIDENTER_H
#define GRAIDENTER_H

#include <QWidget>

class Gradienter : public QWidget
{
    Q_OBJECT

public:
    Gradienter(QWidget *parent = nullptr);
    ~Gradienter();

    void setDegValue(qreal value);
    void setRollValue(qreal value);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateValues();

private:
    void drawBorderOut(QPainter &painter);
    void drawBorderIn(QPainter &painter);
    void drawBg(QPainter &painter);
    void drawPlane(QPainter &painter);
    void drawGlass(QPainter &painter);
    void drawScale(QPainter &painter);
    void drawLine(QPainter &painter);
    void drawPointer(QPainter &painter);
    void drawHandle(QPainter &painter);

    qreal m_deg_value;
    qreal m_roll_value;
};
#endif // GRAIDENTER_H
