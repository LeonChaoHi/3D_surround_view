//
// Created by e712 on 2022/4/29.
//

#ifndef TEXTURE_COORD_CALCULATOR_H
#define TEXTURE_COORD_CALCULATOR_H

#include "main.h"

using namespace std;
using namespace cv;

class coord_calculator {
public:
    coord_calculator(img_type type) : _type(type){
        params_vec.reserve(4);
    }
    typedef struct params{
        params(cv::Mat _K, cv::Mat _D, cv::Mat _T) : K(_K), D(_D), T(_T){};
        cv::Mat K;
        cv::Mat D;
        cv::Mat T;
        bool updated = false;
    } params;
    void params_registry(img_type type, cv::Mat K, cv::Mat D, cv::Mat T);
    void calc_coord(img_type type, GLfloat m[], GLfloat s[], int n_points);
    static void calc_coord(cv::InputArray K, cv::InputArray D, cv::Mat T, cv::Size outsize, double resize_ratio, GLfloat m[], GLfloat s[], int n_points);

private:
    img_type _type;
    vector<params> params_vec;  // 0 for front, 1 for left, 2 for rear, 3 for right.
};


#endif //TEXTURE_COORD_CALCULATOR_H
