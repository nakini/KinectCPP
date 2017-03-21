#ifndef __TRANSFORMATION__
#define __TRANSFORMATION__

#include <opencv2/opencv.hpp>
cv::Mat eulerAngles2RotationMat(cv::Vec3f &theta);
cv::Mat tranlationMat(cv::Vec3f &trans);

#endif
