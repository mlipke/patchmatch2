//
// Created by matt on 19.05.15.
//

#ifndef PATCHMATCH2_SSD_H
#define PATCHMATCH2_SSD_H

#include <opencv2/core/core.hpp>

using namespace cv;

double ssd(Mat *left, Mat *right, Point patch, Point test, int window_size);

#endif //PATCHMATCH2_SSD_H
