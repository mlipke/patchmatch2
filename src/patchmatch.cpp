//
// Created by matt on 19.05.15.
//

#include <iostream>
#include <sstream>

#include "patchmatch.h"
#include "ssd.h"
#include "util.h"

void patchmatch(Mat *left, Mat *right, Mat *flow, int window_size, int iterations) {
    for (int i = 0; i < iterations; i++) {
        propagate(left, right, flow, i, window_size);

        stringstream string;
        string << "Image" << i + 1 << ".png";

        //imwrite(string.str(), warp_image(left, flow));
        //imwrite(string.str(), offset_warp(right, flow));
        cout << "Iteration " << i + 1 << " complete!" << endl;
    }
}

void propagate(Mat *left, Mat *right, Mat *flow, int direction, int window_size) {
    if (direction % 2 == 0) {
        propagate_even(left, right, flow, window_size);
    } else {
        propagate_odd(left, right, flow, window_size);
    }
}

void propagate_even(Mat *left, Mat *right, Mat *flow, int window_size) {
    int pixels_changed = 0;

    for (int i = 1; i < left->rows; i++) {
        for (int j = 1; j < left->cols; j++) {
            Point patch(i, j);
            Point some_patch(i - 1, j); //left
            Point another_patch(i, j - 1); //above

            Point test = flow->at<Point>(patch);
            Point some_test = flow->at<Point>(some_patch);
            Point another_test = flow->at<Point>(another_patch);

            Point some_location(some_test.x + 1, some_test.y);
            Point another_location(another_test.x, another_test.y + 1);

            double patch_ssd = ssd(left, right, patch, test, window_size);
            double some_ssd = ssd(left, right, patch, some_location, window_size);
            double another_ssd = ssd(left, right, patch, another_location, window_size);

            double min_ssd = min(patch_ssd, some_ssd, another_ssd);

            if (min_ssd == patch_ssd) flow->at<Point>(patch) = test;
            if (min_ssd == some_ssd) flow->at<Point>(patch) = some_location;
            if (min_ssd == another_ssd) flow->at<Point>(patch) = another_location;

            if (min_ssd != patch_ssd) pixels_changed++;

            random_search(left, right, flow, patch, window_size, 128, min_ssd);
        }
    }

    cout << "Pixels changed: " << pixels_changed << endl;
}

void propagate_odd(Mat *left, Mat *right, Mat *flow, int window_size) {
    int pixels_changed = 0;

    for (int i = left->rows - 1; i > 0; i--) {
        for (int j = left->cols - 1; j > 0; j--) {
            Point patch(i, j);
            Point some_patch(i + 1, j); //right
            Point another_patch(i, j + 1); //below

            Point test = flow->at<Point>(patch);
            Point some_test = flow->at<Point>(some_patch);
            Point another_test = flow->at<Point>(another_patch);

            Point some_location(some_test.x - 1, some_test.y);
            Point another_location(another_test.x, another_test.y - 1);

            double patch_ssd = ssd(left, right, patch, test, window_size);
            double some_ssd = ssd(left, right, patch, some_location, window_size);
            double another_ssd = ssd(left, right, patch, another_location, window_size);

            double min_ssd = min(patch_ssd, some_ssd, another_ssd);

            if (min_ssd == patch_ssd) flow->at<Point>(patch) = test;
            if (min_ssd == some_ssd)flow->at<Point>(patch) = some_location;
            if (min_ssd == another_ssd) flow->at<Point>(patch) = another_location;

            if (min_ssd != patch_ssd) pixels_changed++;

            random_search(left, right, flow, patch, window_size, 128, min_ssd);
        }
    }

    cout << "Pixels changed: " << pixels_changed << endl;
}

void random_search(Mat *left, Mat *right, Mat *flow, Point p, int window_size, int radius, double current_ssd) {
    Point r_point = random_point(p, radius);

    double r_ssd = ssd(left, right, p, r_point, window_size);

    if (r_ssd < current_ssd) {
        flow->at<Point>(p) = flow->at<Point>(r_point);
        current_ssd = r_ssd;
    }

    if (radius > window_size) {
        random_search(left, right, flow, p, window_size, radius / 2, current_ssd);
    }
}