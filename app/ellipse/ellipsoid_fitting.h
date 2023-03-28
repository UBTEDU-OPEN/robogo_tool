#ifndef ELLIPSOID_FITTING
#define ELLIPSOID_FITTING

#include "stdio.h"
#include "string.h"
#include "math.h"
#include <QList>

class EllipsePoints;

#define u8 unsigned char

// 取绝对值
double Abs(double a);

// 把矩阵系数全部清除为0
void ResetMatrix(void);

// 把输入的数据先生成矩阵的元素的总和
void CalcData_Input(const QList<EllipsePoints> &points);


// 化简系数矩阵，把除以N带上
void CalcData_Input_average();

// 显示出来系数矩阵和增广矩阵[A|b]
void DispMatrix(void);

// 交换两行元素位置
void Row2_swop_Row1(int row1, int row2);

// 用把row行的元素乘以一个系数k
void k_muiltply_Row(double k, int row);

// 用一个数乘以row1行加到row2行上去
void Row2_add_kRow1(double k, int row1, int row2);


// 列主元，第k次消元之前，把k行到MATRIX_SIZE的所有行进行冒泡排出最大，排序的依据是k列的元素的大小
void MoveBiggestElement_to_Top(int k);

// 高斯消元法，求行阶梯型矩阵
u8 Matrix_GaussElimination(void);

// 求行最简型矩阵，即把对角线的元素全部化成1
void Matrix_RowSimplify(void);

// 求非齐次线性方程组的解
void Matrix_Solve(double* solve);

// 整个椭球校准的过程
void Ellipsoid_fitting_Process(const QList<EllipsePoints> &points, double result[]);

void printDouble(double x[], int size);

#endif // ELLIPSOID_FITTING
