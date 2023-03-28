#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#define CALIBRATION_OK 000000
#define CALIBRATION_FAILED 000001


/*
 * @param image 输入图像
  * @param result_contour 存储两个矩形四个定点坐标的vector
  * @return CALIBRATION_OK 推理成功
  * @return CALIBRATION_FAILED 推理失败
 */
int CalibrationProcess(cv::Mat & image, std::vector<std::vector<cv::Point>>& result_contour);
