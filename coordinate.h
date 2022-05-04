//
// Created by e712 on 2021/8/31.
//

#ifndef TEXTURE_COORDINATE_H
#define TEXTURE_COORDINATE_H
#include "GL/gl.h"
#include <opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include<opencv2/features2d/features2d.hpp>
#include<opencv2/core/core.hpp>

using namespace cv;

/**
 * @brief            Compute coordinates
 * @param[in]   K Intrinsic matrix of camera
 * @param[in]   D    Distortion parameters
 * @param[in]   T    Transform matrix
 * @param[in]   vertex_coord    Vertex coordinate
 * @param[out]texture_coord     Texture coordinate
 */
void get_coordinate(cv::InputArray &K, cv::InputArray &D, const cv::Mat &T,
                    const GLfloat *vertex_coord, GLfloat *texture_coord,
                    const cv::Size & outsize, double resize_ratio, cv::Mat &map1, cv::Mat &map2)
{
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
    double MM[12];  // M with r3
    idx = 0;
    for(int i=0;i<3;i++){
        for(int j=0;j<2;j++){
            MM[idx++] = T.at<double>(i, j);
        }
        MM[idx++] = 0; // new r3 elements
        MM[idx++] = T.at<double>(i, 2);
    }
    MM[2] = MM[4] * MM[9] - MM[8] * MM[5];
    MM[6] = MM[8] * MM[1] - MM[0] * MM[9];
    MM[10] = MM[0] * MM[5] - MM[4] * MM[1];

    // new Step 3: calculate texture coordinate
    double u0, v0, w0, u, v;
    double xw = vertex_coord[0] / resize_ratio;
    double yw = vertex_coord[1] / resize_ratio;
    double zw = vertex_coord[2] / resize_ratio;

    // 1. Perspective transformation map corelation
    u0 = MM[0] * xw + MM[1] * yw + MM[2] * zw + MM[3];
    v0 = MM[4] * xw + MM[5] * yw  + MM[6] * zw + MM[7];
    w0 = MM[8] * xw + MM[9] * yw + MM[10] * zw  + MM[11];
    u = u0 / w0;
    v = v0 / w0;

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
    // set output texture coordinates
    texture_coord[0] = X;
    texture_coord[1] = Y;


    // Old Step 3: calculate each out_map(i,j)
    /*
    double x0, y0, w0, u, v, x, y;
    for(int i=0;i<outsize.height*resize_ratio;i++)
    {
        float *ptr = out_map.ptr<float>(i);
        for(int j=0;j<outsize.width*resize_ratio;j++)
        {
            // 0. scaling i and j by resize ratio
            double i0 = i / resize_ratio;
            double j0 = j / resize_ratio;

            // 1. Perspective transformation map corelation
            x0 = M[0] * j0 + M[1] * i0 + M[2];
            y0 = M[3] * j0 + M[4] * i0  + M[5];
            w0 = M[6] * j0 + M[7] * i0 + M[8];
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

            // 3. Update map[i,j]
            ptr[j*2] = float(X);//x
            ptr[j*2+1] = float(Y);//y

//            ptr[j*2] = (float)u;//x
//            ptr[j*2+1] = (float)v;//y
        }
    }
    */

    // Old Step 4: split outmap into map1 and map2
    /*
    Mat maps[2];

    cv::split(out_map, maps);//CV_32FC2
//    cv::convertMaps(maps[0], maps[1], map1, map2, CV_16SC2);
    cv::convertMaps(maps[0], maps[1], map1, map2, CV_32FC1);
    */
    return;
}

#endif //TEXTURE_COORDINATE_H
