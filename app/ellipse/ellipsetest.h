#ifndef ELLIPSETEST_H
#define ELLIPSETEST_H

#include "ellipse.h"
#include <QObject>
#include <QThread>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = nullptr);

    void run();

    void randomNum(double a[], int n, int L, int R);
private:

signals:
    void sigDrawXY(double dx, double dy, double dz, Ellipse::Axis axis);
    void sigDrawXZ(double dx, double dy, double dz, Ellipse::Axis axis);
    void sigDrawYZ(double dx, double dy, double dz, Ellipse::Axis axis);
};


class EllipseTest
{
public:
    EllipseTest();
    void test();

    ~EllipseTest();

private:
    MyThread m_thr;

    Ellipse *m_xy;
    Ellipse *m_yz;
    Ellipse *m_xz;
};

#endif // ELLIPSETEST_H
