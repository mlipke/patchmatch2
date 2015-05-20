//
// Created by matt on 20.05.15.
//

#ifndef PATCHMATCH2_PYRAMID_H
#define PATCHMATCH2_PYRAMID_H

#include <math.h>

#include "util.h"
#include "ssd.h"

void pyramid(Mat *left, Mat *right, int window_size, int levels, int iterations);

vector<Mat> construct_pyramid(Mat *image, int levels);
Mat upscale_flow(Mat *flow);

void pyramid_patchmatch(Mat *left, Mat *right, Mat *random_flow, Mat *scaled_flow, int window_size, int iterations);

void pyramid_propagate(Mat *left, Mat *right, Mat *random_flow, Mat *scaled_flow, int direction, int window_size);
void pyramid_propagate_even(Mat *left, Mat *right, Mat *random_flow, Mat *scaled_flow, int window_size);
void pyramid_propagate_odd(Mat *left, Mat *right, Mat *random_flow, Mat *scaled_flow, int window_size);

void pyramid_random_search(Mat *left, Mat *right, Mat *flow, Point p, int window_size, int radius, double current_ssd);

double alt_min(double one, double two, double three, double four, double five, double six);

#endif //PATCHMATCH2_PYRAMID_H
