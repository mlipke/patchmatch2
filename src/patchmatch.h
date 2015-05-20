//
// Created by matt on 19.05.15.
//

#ifndef PATCHMATCH2_PATCHMATCH_H
#define PATCHMATCH2_PATCHMATCH_H

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

void patchmatch(Mat *left, Mat *right, Mat *flow, int window_size, int iterations);

void propagate(Mat *left, Mat *right, Mat *random_flow, int direction, int window_size);
void propagate_even(Mat *left, Mat *right, Mat *random_flow, int window_size);
void propagate_odd(Mat *left, Mat *right, Mat *random_flow, int window_size);

void random_search(Mat *left, Mat *right, Mat *flow, Point p, int window_size, int radius, double current_ssd);

#endif //PATCHMATCH2_PATCHMATCH_H
