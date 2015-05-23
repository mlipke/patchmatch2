//
// Created by matt on 20.05.15.
//

#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>

#include "pyramid.h"

vector<Mat> construct_pyramid(Mat *image, int levels) {
    vector<Mat> pyramid;

    for (int i = 0; i < levels; i++) {
        int scale = pow(2, i);
        Size level_size(image->cols / scale, image->rows / scale);

        Mat level;
        resize(*image, level, level_size);

        pyramid.push_back(level);
    }

    reverse(pyramid.begin(), pyramid.end());

    return pyramid;
}

void pyramid(Mat *left, Mat *right, int window_size, int levels, int iterations) {
    vector<Mat> left_pyramid = construct_pyramid(left, levels);
    vector<Mat> right_pyramid = construct_pyramid(right, levels);

    Mat scaling_flow;
    Mat random_flow;

    for (int i = 0; i < levels; i++) {
        Mat level_left = left_pyramid.at((unsigned long)i);
        Mat level_right = right_pyramid.at((unsigned long)i);

        if (i == 0) {
            initialize_random_flow(&level_left, &scaling_flow);
        } else {
            Mat temp;
            scaling_flow.copyTo(temp);
            scaling_flow = upscale_flow(&temp);
        }

        initialize_random_flow(&level_left, &scaling_flow);

        pyramid_patchmatch(&level_left, &level_right, &random_flow, &scaling_flow, window_size, iterations);
    }
}

Mat upscale_flow(Mat *flow) {
    Mat scaled_flow = Mat_<Point>(flow->rows * 2, flow->cols * 2);

    for (int i = 0; i < flow->rows; i++) {
        Point *flow_ptr = flow->ptr<Point>(i);
        Point *some_scaled_flow_ptr = scaled_flow.ptr<Point>(i * 2);
        Point *another_scaled_flow_ptr = scaled_flow.ptr<Point>(i * 2 + 1);
        for (int j = 0; j < flow->cols; j++) {
            Point current_flow = flow_ptr[j];

            current_flow.x = current_flow.x * 2;
            current_flow.y = current_flow.y * 2;

            some_scaled_flow_ptr[j * 2] = current_flow;
            some_scaled_flow_ptr[j * 2 + 1] = current_flow;
            another_scaled_flow_ptr[j * 2] = current_flow;
            another_scaled_flow_ptr[j * 2 + 1] = current_flow;
        }
    }

    return scaled_flow;
}

void pyramid_patchmatch(Mat *left, Mat *right, Mat *random_flow, Mat *scaled_flow, int window_size, int iterations) {
    for (int i = 0; i < iterations; i++) {
        pyramid_propagate(left, right, random_flow, scaled_flow, i, window_size);
    }
}

void pyramid_propagate(Mat *left, Mat *right, Mat *random_flow, Mat *scaled_flow, int direction, int window_size) {
    if (direction % 2 == 0) {
        pyramid_propagate_even(left, right, random_flow, scaled_flow, window_size);
    } else {
        pyramid_propagate_odd(left, right, random_flow, scaled_flow, window_size);
    }
}

void pyramid_propagate_even(Mat *left, Mat *right, Mat *random_flow, Mat *scaled_flow, int window_size) {
    int pixels_changed = 0;

    for (int i = 1; i < left->rows; i++) {
        for (int j = 1; j < left->cols; j++) {
            Point patch(i, j);
            Point some_patch(i - 1, j); //left
            Point another_patch(i, j - 1); //above

            Point test = random_flow->at<Point>(patch);
            Point some_test = random_flow->at<Point>(some_patch);
            Point another_test = random_flow->at<Point>(another_patch);

            Point some_location(some_test.x + 1, some_test.y);
            Point another_location(another_test.x, another_test.y + 1);

            double patch_ssd = ssd(left, right, patch, test, window_size);
            double some_ssd = ssd(left, right, patch, some_location, window_size);
            double another_ssd = ssd(left, right, patch, another_location, window_size);

            Point alt_test = scaled_flow->at<Point>(patch);
            Point alt_some_test = scaled_flow->at<Point>(some_patch);
            Point alt_another_test = scaled_flow->at<Point>(another_patch);

            Point alt_some_location(alt_some_test.x + 1, alt_some_test.y);
            Point alt_another_location(alt_another_test.x, alt_another_test.y + 1);

            double alt_patch_ssd = ssd(left, right, patch, alt_test, window_size);
            double alt_some_ssd = ssd(left, right, patch, alt_some_location, window_size);
            double alt_another_ssd = ssd(left, right, patch, alt_another_location, window_size);

            double min_ssd = alt_min(patch_ssd, some_ssd, another_ssd,
                                     alt_patch_ssd, alt_some_ssd, alt_another_ssd);

            if (min_ssd == patch_ssd) random_flow->at<Point>(patch) = test;
            if (min_ssd == some_ssd)random_flow->at<Point>(patch) = some_location;
            if (min_ssd == another_ssd) random_flow->at<Point>(patch) = another_location;
            if (min_ssd == alt_patch_ssd) random_flow->at<Point>(patch) = alt_test;
            if (min_ssd == alt_some_ssd) random_flow->at<Point>(patch) = alt_some_location;
            if (min_ssd == alt_another_ssd) random_flow->at<Point>(patch) = alt_another_location;

            if (min_ssd != patch_ssd) pixels_changed++;

            pyramid_random_search(left, right, random_flow, patch, window_size, 128, min_ssd);
        }
    }

    cout << "Pixels changed: " << pixels_changed << endl;
}

void pyramid_propagate_odd(Mat *left, Mat *right, Mat *random_flow, Mat *scaled_flow, int window_size) {
    int pixels_changed = 0;

    for (int i = left->rows - 2; i > 0; i--) {
        for (int j = left->cols - 2; j > 0; j--) {
            Point patch(i, j);
            Point some_patch(i + 1, j); //right
            Point another_patch(i, j + 1); //below

            Point test = random_flow->at<Point>(patch);
            Point some_test = random_flow->at<Point>(some_patch);
            Point another_test = random_flow->at<Point>(another_patch);

            Point some_location(some_test.x - 1, some_test.y);
            Point another_location(another_test.x, another_test.y - 1);

            double patch_ssd = ssd(left, right, patch, test, window_size);
            double some_ssd = ssd(left, right, patch, some_location, window_size);
            double another_ssd = ssd(left, right, patch, another_location, window_size);

            Point alt_test = scaled_flow->at<Point>(patch);
            Point alt_some_test = scaled_flow->at<Point>(some_patch);
            Point alt_another_test = scaled_flow->at<Point>(another_patch);

            Point alt_some_location(alt_some_test.x - 1, alt_some_test.y);
            Point alt_another_location(alt_another_test.x, alt_another_test.y - 1);

            double alt_patch_ssd = ssd(left, right, patch, alt_test, window_size);
            double alt_some_ssd = ssd(left, right, patch, alt_some_location, window_size);
            double alt_another_ssd = ssd(left, right, patch, alt_another_location, window_size);

            double min_ssd = alt_min(patch_ssd, some_ssd, another_ssd,
                alt_patch_ssd, alt_some_ssd, alt_another_ssd);

            if (min_ssd == patch_ssd) random_flow->at<Point>(patch) = test;
            if (min_ssd == some_ssd)random_flow->at<Point>(patch) = some_location;
            if (min_ssd == another_ssd) random_flow->at<Point>(patch) = another_location;
            if (min_ssd == alt_patch_ssd) random_flow->at<Point>(patch) = alt_test;
            if (min_ssd == alt_some_ssd) random_flow->at<Point>(patch) = alt_some_location;
            if (min_ssd == alt_another_ssd) random_flow->at<Point>(patch) = alt_another_location;

            if (min_ssd != patch_ssd) pixels_changed++;

            pyramid_random_search(left, right, random_flow, patch, window_size, 128, min_ssd);
        }
    }

    cout << "Pixels changed: " << pixels_changed << endl;
}

void pyramid_random_search(Mat *left, Mat *right, Mat *flow, Point p, int window_size, int radius, double current_ssd) {
    Point r_point = random_point(p, radius);

    double r_ssd = ssd(left, right, p, r_point, window_size);

    if (r_ssd < current_ssd) {
        flow->at<Point>(p) = flow->at<Point>(r_point);
        current_ssd = r_ssd;
    }

    if (radius > window_size) {
        pyramid_random_search(left, right, flow, p, window_size, radius / 2, current_ssd);
    }
}

double alt_min(double one, double two, double three, double four, double five, double six) {
    double min = one;
    if (two < min) min = two;
    if (three < min) min = three;
    if (four < min) min = four;
    if (five < min) min = five;
    if (six < min) min = six;

    return min;
}
