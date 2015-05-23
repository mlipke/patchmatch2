#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "main.h"
#include "pyramid.h"
#include "patchmatch.h"

using namespace std;
using namespace cv;

int main(int argc, char **argv) {
    if (argc != 5) return 0;

    int window_size = atoi(argv[1]);
    int use_pyramid = atoi(argv[4]);

    Mat image_one = imread(argv[2], CV_LOAD_IMAGE_COLOR);
    Mat image_two = imread(argv[3], CV_LOAD_IMAGE_COLOR);

    Mat left = Mat_<Vec3d>(image_one.rows, image_two.cols);
    Mat right = Mat_<Vec3d>(image_one.rows, image_two.cols);

    image_one.convertTo(left, CV_64FC3);
    image_two.convertTo(right, CV_64FC3);

    srand((unsigned int)(time(NULL)));

    if (use_pyramid) {
        pyramid(&left, &right, window_size, N_PYRAMID_LEVELS, N_ITERATIONS);
    } else {
        Mat flow = Mat_<Point>(left.cols, left.rows);
        initialize_random_flow(&left, &flow);
        patchmatch(&left, &right, &flow, window_size, N_ITERATIONS);

        Mat offset = Mat_<Point>(flow.rows, flow.cols);
        compute_offset(&flow, &offset);
        write_flow_field(&offset);
        write_flo_file(&offset);
    }

    return 0;
}