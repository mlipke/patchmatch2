//
// Created by matt on 19.05.15.
//

#include "ssd.h"
#include "util.h"

double ssd(Mat *left, Mat *right, Point patch, Point test, int window_size) {
    double sum = 0, count = 0;

    if (out_of_bounds(patch, left) || out_of_bounds(test, left))
        return numeric_limits<double>::infinity();

    for (int i = -window_size; i <= window_size; i++) {
        int it_y_patch = i + patch.y;
        int it_y_test = i + test.y;

        if (out_of_bounds(it_y_patch, 0, left->rows) || out_of_bounds(it_y_test, 0, left->rows))
            continue;

        Vec3d *patch_ptr = left->ptr<Vec3d>(it_y_patch);
        Vec3d *test_ptr = right->ptr<Vec3d>(it_y_test);

        for (int j = -window_size; j <= window_size; j++) {
            int it_x_patch = j + patch.x;
            int it_x_test = j + test.x;

            if (out_of_bounds(it_x_patch, 0, left->cols) || out_of_bounds(it_x_test, 0, left->cols))
                continue;

            double b_diff = patch_ptr[it_x_patch][0] - test_ptr[it_x_test][0];
            double g_diff = patch_ptr[it_x_patch][1] - test_ptr[it_x_test][1];
            double r_diff = patch_ptr[it_x_patch][2] - test_ptr[it_x_test][2];

            sum += (b_diff * b_diff) + (g_diff * g_diff) + (r_diff * r_diff);
            count++;
        }
    }

    return sum / count;
}
