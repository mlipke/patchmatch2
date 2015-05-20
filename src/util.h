//
// Created by matt on 19.05.15.
//

#ifndef PATCHMATCH2_UTIL_H
#define PATCHMATCH2_UTIL_H

#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;

Mat initialize_random_flow(Mat *image);

bool out_of_bounds(Point point, Mat *image);
bool out_of_bounds(int i, int min, int max);

double min(double one, double two, double three);

Point random_point(Point p, int radius);

Mat warp_image(Mat *image, Mat *flow);
Mat compute_offset(Mat *flow);

void write_flow_field(Mat *flow);
void write_flo_file(Mat *flow);

#endif //PATCHMATCH2_UTIL_H
