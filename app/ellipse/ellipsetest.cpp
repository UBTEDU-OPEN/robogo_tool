#include "ellipsetest.h"
#include "ellipse.h"
#include "ellipsoid_fitting.h"


MyThread::MyThread(QObject *parent) : QThread(parent)
{

}

void MyThread::run()
{
//    Ellipse *xy = new Ellipse("xy");
//    Ellipse *xz = new Ellipse("xz");
//    Ellipse *yz = new Ellipse("yz");

//    xy->show();
//    xz->show();
//    yz->show();

    double x[] = {-855, -836, -773, -731, -772, -865, -952, -1023, -1063, -1086, -1120, -1168, -1214, -1257, -1293};
    double y[] = {2579, 2546, 2415, 2083, 1612, 1156,  802, 552, 413, 311,199,121,137,216,301};
    double z[] = {2024, 1999 , 2038 , 2333 , 2550 , 2508 , 2302, 2065, 1902, 1786,1671,1569,1481,1370,1253};

    int size = 6;

    for (int i = 0; i < 600; i++)
    {
        randomNum(x, size, -1000, 1000);
        randomNum(y, size, -1000, 1000);
        randomNum(z, size, -1000, 1000);

//        xy->drawEllipse(x, y, z, size, Ellipse::ZAxis);
//        xy->drawScatter(x, y, z, size, Ellipse::ZAxis);

//        xz->drawEllipse(x, y, z, size, Ellipse::YAxis);
//        xz->drawScatter(x, y, z, size, Ellipse::YAxis);

//        yz->drawEllipse(x, y, z, size, Ellipse::XAxis);
//        yz->drawScatter(x, y, z, size, Ellipse::XAxis);

//        xy->setUpdateThreshold(10);
//        xy->updatePoints(x[0], y[0], z[0], Ellipse::ZAxis);
//        xz->updatePoints(x[0], y[0], z[0], Ellipse::YAxis);
//        yz->updatePoints(x[0], y[0], z[0], Ellipse::XAxis);
//        size++;

        sigDrawXY(x[0], y[0], z[0], Ellipse::ZAxis);
        sigDrawXZ(x[0], y[0], z[0], Ellipse::YAxis);
        sigDrawYZ(x[0], y[0], z[0], Ellipse::XAxis);

        fflush(stdout);
        msleep(50);
    }

//    xy->clear();

    exec();
}

void MyThread::randomNum(double a[], int n, int L, int R)
{
    static int seed = 0;
    srand(time(0) + seed);  // 设置时间种子
    for(int i=0; i<n; i++)
    {
        a[i] = rand() % (R-L+1)+L; // 生成区间r~l的随机数
//        printf("%f ", a[i]);
    }
//    printf("\n");
    seed++;
}


EllipseTest::EllipseTest()
{
    m_xy = new Ellipse();
    m_xy->setTitle("xy", Ellipse::ZAxis);
    m_yz = new Ellipse();
    m_yz->setTitle("yz", Ellipse::XAxis);
    m_xz = new Ellipse();
    m_xz->setTitle("xz", Ellipse::YAxis);

}

void EllipseTest::test()
{
    m_thr.start();


    m_xy->show();
    m_xy->move(100, 100);


    m_yz->show();
    m_yz->move(800,100);


    m_xz->show();
    m_xz->move(100, 600);

    QObject::connect(&m_thr, SIGNAL(sigDrawXY(double,double,double,Ellipse::Axis)), m_xy, SLOT(updatePoints(double,double,double, Ellipse::Axis)));
    QObject::connect(&m_thr, SIGNAL(sigDrawYZ(double,double,double,Ellipse::Axis)), m_yz, SLOT(updatePoints(double,double,double, Ellipse::Axis)));
    QObject::connect(&m_thr, SIGNAL(sigDrawXZ(double,double,double,Ellipse::Axis)), m_xz, SLOT(updatePoints(double,double,double, Ellipse::Axis)));
    fflush(stdout);
}

EllipseTest::~EllipseTest()
{
    delete m_xy;
    delete m_yz;
    delete m_xz;
}
