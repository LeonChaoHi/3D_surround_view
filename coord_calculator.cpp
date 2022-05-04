//
// Created by e712 on 2022/4/29.
//

#include "coord_calculator.h"
void coord_calculator::params_registry(img_type type, cv::Mat K, cv::Mat D, cv::Mat T){
    int save_idx = 0;
    switch(type){
        case img_type::FRONT:
            save_idx = 0;
            break;
        case img_type::LEFT:
            save_idx = 1;
            break;
        case img_type::REAR:
            save_idx = 2;
            break;
        case img_type::RIGHT:
            save_idx = 3;
            break;
    }
    params_vec[save_idx].K = K;
    params_vec[save_idx].D = D;
    params_vec[save_idx].T = T;
    params_vec[save_idx].updated = true;
}

void coord_calculator::calc_coord(img_type type, GLfloat m[], GLfloat s[], int n_points){
    int save_idx = 0;
    switch(type){
        case img_type::FRONT:
            save_idx = 0;
            break;
        case img_type::LEFT:
            save_idx = 1;
            break;
        case img_type::REAR:
            save_idx = 2;
            break;
        case img_type::RIGHT:
            save_idx = 3;
            break;
    }
    if (! params_vec[save_idx].updated){
        throw "error: parameter not updated.";
    }
    cv::Size sz(1280, 2560);
    calc_coord(params_vec[save_idx].K, params_vec[save_idx].D, params_vec[save_idx].T, sz, 1.0, m, s, n_points);
}

void coord_calculator::calc_coord(cv::InputArray K, cv::InputArray D, cv::Mat T, cv::Size outsize, double resize_ratio, GLfloat m[], GLfloat s[], int n_points)
{
//    int m[300], s[200];
//    int n_points = 100;
    CV_Assert(K.depth() == CV_64F && D.depth() == CV_64F && T.depth() == CV_64F);
    Mat out_map = Mat::zeros(int(outsize.height*resize_ratio), int(outsize.width*resize_ratio), CV_32FC2);

    // Step 1: initialize undistortion parameters
    cv::Vec2d f, c;
    Matx33d camMat = K.getMat();
    f = Vec2d(camMat(0, 0), camMat(1, 1));
    c = Vec2d(camMat(0, 2), camMat(1, 2));
    //从畸变系数矩阵D中取出畸变系数k1,k2,k3,k4
    Vec4d k = Vec4d::all(0);
    k = *D.getMat().ptr<Vec4d>();
    //旋转矩阵RR转换数据类型为CV_64F，如果不需要旋转，则RR为单位阵
    cv::Matx33d RR = cv::Matx33d::eye();
    cv::Matx33d PP = cv::Matx33d::eye();
    //关键一步：新的内参矩阵*旋转矩阵，然后利用SVD分解求出逆矩阵iR，后面用到
    cv::Matx33d iR = (PP * RR).inv(cv::DECOMP_SVD);

    // Step 2: transform transmatrix to a vector M
    double M[9];
    int idx = 0;
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            M[idx++] = T.at<double>(i, j);
        }
    }

    // Step 3: calculate each out_map(i,j)
    double x0, y0, w0, u, v, x, y;
    for(int i=0; i<n_points; ++i){

        // 0. scaling i, j, k by resize ratio
        double i0 = m[i*3] / resize_ratio;   // TODO: resize ratio, i0, j0需要平移
        double j0 = m[i*3+1] / resize_ratio;
        double k0 = m[i*3+2] / resize_ratio;

        // 1. Perspective transformation map corelation
        x0 = M[0] * j0 + M[1] * i0 + M[2] + (M[3]*M[7]-M[4]*M[6]) * k0;
        y0 = M[3] * j0 + M[4] * i0 + M[5] + (M[1]*M[6]-M[0]*M[7]) * k0;
        w0 = M[6] * j0 + M[7] * i0 + M[8] + (M[0]*M[4]-M[1]*M[3]) * k0;
        u = x0/w0;
        v = y0/w0;

        // 2. Distortion rectification
        u = (u - c[0])/f[0];
        v = (v - c[1])/f[1];
        double _x = u*iR(0, 0) + v*iR(0, 1) + iR(0, 2),
                _y = u*iR(1, 0) + v*iR(1, 1) + iR(1, 2),
                _w = u*iR(2, 0) + v*iR(2, 1) + iR(2, 2);

        //归一化摄像机坐标系，相当于假定在Z=1平面上
        double x = _x/_w, y = _y/_w;
        //求鱼眼半球体截面半径r
        double r = sqrt(x*x + y*y);
        //求鱼眼半球面上一点与光心的连线和光轴的夹角Theta
        double theta = atan(r);
        //畸变模型求出theta_d，相当于有畸变的角度值
        double theta2 = theta*theta, theta4 = theta2*theta2, theta6 = theta4*theta2, theta8 = theta4*theta4;
        double r_theta = theta * (1 + k[0]*theta2 + k[1]*theta4 + k[2]*theta6 + k[3]*theta8);
        //利用有畸变的Theta值，将摄像机坐标系下的归一化三维坐标，重投影到二维图像平面，得到(j,i)对应畸变图像中的(u,v)
        double scale = (r == 0) ? 1.0 : r_theta / r;
        double X = f[0]*x*scale + c[0];
        double Y = f[1]*y*scale + c[1];

        // 3. Update s[i]
        s[i*2] = float(X);//x
        s[i*2+1] = float(Y);//y

        // 3. 经 长宽等宽，归一化 和 边界截取，转换为纹理坐标 s
        Y = Y * 640 / 480; // 原尺寸为 640x480， 将 dim2 统一到 640. 下面是归一化
        s[i*2] = Y > 0 ? (Y <= 640 ? (Y / 640) : 1) : 0;  // TODO: X 和 Y 的先后？
        s[i*2+1] = X > 0 ? (X <= 640 ? (X / 640) : 1) : 0;
    }
}