//
// Created by matt on 19.05.15.
//

#include <iostream>
#include <fstream>

#include "util.h"

void initialize_random_flow(Mat *image, Mat *flow) {
    for (int i = 0; i < image->rows; i++) {
        Point *p = flow->ptr<Point>(i);
        for (int j = 0; j < image->cols; j++) {
            int random_x = rand() % (image->cols - 1);
            int random_y = rand() % (image->rows - 1);

            p[j] = Point(random_y, random_x);
        }
    }
}

bool out_of_bounds(Point point, Mat *image) {
    return (point.x > image->cols || point.y > image->rows || point.x <= 0 || point.y <= 0);
}

bool out_of_bounds(int i, int min, int max) {
    return (i < min || i >= max);
}

double min(double one, double two, double three) {
    double m = one;

    if (two < m) m = two;
    if (three < m) m = three;

    return m;
}

Point random_point(Point p, int radius) {
    int rand_x = rand() % (radius - 1) * 2 - radius;
    int rand_y = rand() % (radius - 1) * 2 - radius;

    return Point(p.x + rand_x, p.y + rand_y);
}

Mat warp_image(Mat *image, Mat *flow) {
    Mat warped_image = Mat_<Vec3d>(image->rows, image->cols);

    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            Point location = flow->at<Point>(i, j);
            Vec3d value = image->at<Vec3d>(location.x, location.y);
            warped_image.at<Vec3d>(i, j) = value;
        }
    }

    return warped_image;
}

Mat offset_warp(Mat *image, Mat *flow) {
    Mat warped_image = Mat_<Vec3d>(image->rows, image->cols);
    Mat offset = Mat_<Point>(flow->rows, flow->cols);
    compute_offset(flow, &offset);

    for (int i = 1; i < image->rows; i++) {
        for (int j = 1; j < image->cols; j++) {
            Point pixel_offset = offset.at<Point>(i, j);
            Vec3d value = image->at<Vec3d>(i, j);

            if (i + pixel_offset.x < image->cols && j + pixel_offset.y < image->rows
                    && i + pixel_offset.x > 0 && j + pixel_offset.y > 0) {
                warped_image.at<Vec3d>(i + pixel_offset.x, j + pixel_offset.y) = value;
            }
        }
    }

    return warped_image;
}

void write_flow_field(Mat *flow) {
    ofstream flow_file;
    flow_file.open("flow.txt");
    flow_file << *flow;
    flow_file.close();
}

void compute_offset(Mat *flow, Mat *offset) {
    for (int i = 0; i < flow->rows; i++) {
        for (int j = 0; j < flow->cols; j++) {
            Point f = flow->at<Point>(i, j);
            offset->at<Point>(i, j) = Point(f.x - j, f.y - i);
        }
    }
}

void write_flo_file(Mat *flow) {
    ofstream flo;
    flo.open("flow.flo", ios::out | ios::binary);

    int width = flow->cols;
    int height = flow->rows;

    flo.write("PIEH", sizeof(char) * 4);
    flo.write(reinterpret_cast<const char *>(&width), sizeof(int));
    flo.write(reinterpret_cast<const char *>(&height), sizeof(int));

    for (int i = 0; i < flow->rows; i++) {
        Point *p = flow->ptr<Point>(i);
        for (int j = 0; j < flow->cols; j++) {
            Point value = p[j];

            float x = (float)value.x;
            float y = (float)value.y;

            flo.write(reinterpret_cast<const char *>(&x), sizeof(float));
            flo.write(reinterpret_cast<const char *>(&y), sizeof(float));
        }
    }

    flo.close();
}
