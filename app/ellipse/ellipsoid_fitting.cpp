#include "ellipsoid_fitting.h"
#include "ellipse.h"

#define MATRIX_SIZE 6

double m_matrix[MATRIX_SIZE][MATRIX_SIZE + 1]; // 系数矩阵
double solve[MATRIX_SIZE] = { 0 }; // 方程组的解对应最小二乘椭球拟合中的，a，b，c，d，e，f，

double m_result[MATRIX_SIZE];
int N = 0; // 计算累计的采样点次数的

// 取绝对值
double Abs(double a)
{
    return a < 0 ? -a : a;
}

// 把矩阵系数全部清除为0
void ResetMatrix(void)
{
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    {
        for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
            m_matrix[row][column] = 0.0f;
    }
}

// 把输入的数据先生成矩阵的元素的总和
void CalcData_Input(const QList<EllipsePoints> &points)
{
    auto size = points.size();
    for (int i = 0; i < size; i++)
    {
        double V[MATRIX_SIZE + 1];
        N++;
        V[0] = points[i].Y * points[i].Y;
        V[1] = points[i].Z * points[i].Z;
        V[2] = points[i].X;
        V[3] = points[i].Y;
        V[4] = points[i].Z;
        V[5] = 1.0;
        V[6] = -(points[i].X)*(points[i].X);
        // 构建系数矩阵，并进行累加
        for (u8 row = 0; row < MATRIX_SIZE; row++)
        {
            for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
            {
                m_matrix[row][column] += V[row] * V[column];
            }
        }
        // b向量是m_matrix[row][6]
    }
}

// 化简系数矩阵，把除以N带上
void CalcData_Input_average()
{
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        m_matrix[row][column] /= N;
    // b向量是m_matrix[row][6]
}

// 显示出来系数矩阵和增广矩阵[A|b]
void DispMatrix(void)
{
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    {
        for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        {
            printf("%23f ", m_matrix[row][column]);
            if (column == MATRIX_SIZE - 1)
                printf("|");
        }
        printf("\r\n");
    }
    printf("\r\n\r\n");
}

// 交换两行元素位置
void Row2_swop_Row1(int row1, int row2)
{
    double tmp = 0;
    for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
    {
        tmp = m_matrix[row1][column];
        m_matrix[row1][column] = m_matrix[row2][column];
        m_matrix[row2][column] = tmp;
    }
}

// 用把row行的元素乘以一个系数k
void k_muiltply_Row(double k, int row)
{
    for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        m_matrix[row][column] *= k;
}

// 用一个数乘以row1行加到row2行上去
void Row2_add_kRow1(double k, int row1, int row2)
{
    for (u8 column = 0; column < MATRIX_SIZE + 1; column++)
        m_matrix[row2][column] += k*m_matrix[row1][column];
}


// 列主元，第k次消元之前，把k行到MATRIX_SIZE的所有行进行冒泡排出最大，排序的依据是k列的元素的大小
void MoveBiggestElement_to_Top(int k)
{
    int row = 0, column = 0;

    for (row = k + 1; row < MATRIX_SIZE; row++)
    {
        if (Abs(m_matrix[k][k]) < Abs(m_matrix[row][k]))
        {
            Row2_swop_Row1(k, row);
        }
    }
}

// 高斯消元法，求行阶梯型矩阵
u8 Matrix_GaussElimination(void)
{
    double k = 0;
    for (u8 cnt = 0; cnt < MATRIX_SIZE; cnt++)//进行第k次的运算，主要是针对k行以下的行数把k列的元素都变成0
    {
        // 把k行依据k列的元素大小，进行排序
        MoveBiggestElement_to_Top(cnt);
        if (m_matrix[cnt][cnt] == 0)
            return(1); // 返回值表示错误
        // 把k行下面的行元素全部消成0，整行变化
        for (u8 row = cnt + 1; row < MATRIX_SIZE; row++)
        {
            k = -m_matrix[row][cnt] / m_matrix[cnt][cnt];
            Row2_add_kRow1(k, cnt, row);
        }
//        DispMatrix();
    }
    return 0;
}

// 求行最简型矩阵，即把对角线的元素全部化成1
void Matrix_RowSimplify(void)
{
    double k = 0;
    for (u8 row = 0; row < MATRIX_SIZE; row++)
    {
        k = 1 / m_matrix[row][row];
        k_muiltply_Row(k, row);
    }
//    DispMatrix();
}

// 求非齐次线性方程组的解
void Matrix_Solve(double* solve)
{
    for (short row = MATRIX_SIZE - 1; row >= 0; row--)
    {
        solve[row] = m_matrix[row][MATRIX_SIZE];
        for (u8 column = MATRIX_SIZE - 1; column >= row + 1; column--)
            solve[row] -= m_matrix[row][column] * solve[column];
    }
//    printf("  a = %f| b = %f| c = %f| d = %f| e = %f| f = %f ", solve[0], solve[1], solve[2], solve[3], solve[4], solve[5]);
//    printf("\r\n");
}

// 整个椭球校准的过程
void Ellipsoid_fitting_Process(const QList<EllipsePoints> &points, double result[])
{
//    printDouble(x, size);
//    printDouble(y, size);
//    printDouble(z, size);
    ResetMatrix();

    // 这里输入任意个点加速度参数，尽量在球面上均匀分布
    CalcData_Input(points);

    CalcData_Input_average(); // 对输入的数据到矩阵元素进行归一化
//    DispMatrix(); // 显示原始的增广矩阵
    if (Matrix_GaussElimination())	// 求得行阶梯形矩阵
        printf("the marix could not be solved\r\n");
    else
    {
        Matrix_RowSimplify(); // 化行最简形态
        Matrix_Solve(solve); // 求解a,b,c,d,e,f

        double a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
        a = solve[0];
        b = solve[1];
        c = solve[2];
        d = solve[3];
        e = solve[4];
        f = solve[5];

        double X0 = 0, Y0 = 0, Z0 = 0, A = 0, B = 0, C = 0;
        X0 = -c / 2;
        Y0 = -d / (2 * a);
        Z0 = -e / (2 * b);
        double dataA = X0*X0 + a*Y0*Y0 + b*Z0*Z0 - f;
        A = sqrt(dataA);
        B = A / sqrt(a);
        C = A / sqrt(b);
//        printf("  ((x - x0) / A) ^ 2 + ((y - y0) / B) ^ 2 + ((z - z0) / C) ^ 2 = 1 Ellipsoid result as below：\r\n");
//        printf("  X0 = %f| Y0 = %f| Z0 = %f| A = %f| B = %f| C = %f \r\n\r\n", X0, Y0, Z0, A, B, C);

        result[0] = X0;
        result[1] = Y0;
        result[2] = Z0;
        result[3] = A;
        result[4] = B;
        result[5] = C;
    }
}

void printDouble(double x[], int size)
{
    for (int i =0; i < size; i++)
    {
        printf("%f ", x[i]);
    }
    printf("\n\n");
}
