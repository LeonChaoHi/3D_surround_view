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

void coord_calculator::calc_coord(cv::InputArray K, cv::InputArray D, cv::InputArray T, cv::Size outsize, double resize_ratio, GLfloat m[], GLfloat s[], int n_points)
{
    CV_Assert(K.depth() == CV_64F && D.depth() == CV_64F && T.depth() == CV_64F);

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
    cv::Matx33d H = T.getMat();
    cv::Matx33d RT = camMat.inv(cv::DECOMP_SVD) * H;
    cv::Vec3d r1, r2, r3;
    r1 = {RT(0,0), RT(1,0), RT(2,0)};
    r2 = {RT(0,1), RT(1,1), RT(2,1)};
    r3 = r1.cross(r2);
    cv::Matx34d RT_3d = {r1[0], r2[0], r3[0], RT(0,2), r1[1], r2[1], r3[1], RT(1,2), r1[2], r2[2], r3[2], RT(2,2)};
    cv::Matx34d H_3d = camMat * RT_3d;

    double M[12];
    int idx = 0;
    for(int i=0;i<3;i++){
        for(int j=0;j<4;j++){
            M[idx++] = H_3d(i, j);
        }
    }

    resize_ratio = 0.07;
    // Step 3: calculate each out_map(i,j)
    double x0, y0, w0, u, v, x, y;
    for(int i=0; i<n_points; ++i){
        float tmp1 = m[i*3+1];    // y
        float tmp0 = m[i*3];  // x
        float tmp2 = m[i*3+2];    // z


        // 0. scaling i, j, k by resize ratio
//        double i0 = -m[i*3+1] / resize_ratio + 600; // 世界坐标系 y 平移
//        double j0 = m[i*3] / resize_ratio + 640;   // 世界坐标系 x 平移
        double i0 = -m[i*3+1] / resize_ratio + 300; // 世界坐标系 y 平移
        double j0 = m[i*3] / resize_ratio + 450;   // 世界坐标系 x 平移
        double k0 = m[i*3+2] / resize_ratio; // 世界坐标系 z

//        s[i*2] = tmp0 / 30 + 0.5;
//        s[i*2+1] = tmp1 / 30 + 0.5;
//        continue;

        // 1. Perspective transformation map corelation
        double lambda = r1[2] * j0 + r2[2] * i0 + r3[2] * k0 + RT(2,2);
        k0 *= -(200.0  * lambda);
        x0 = M[0] * j0 + M[1] * i0 + M[2] * k0 + M[3];
        y0 = M[4] * j0 + M[5] * i0 + M[6] * k0 + M[7];
        w0 = M[8] * j0 + M[9] * i0 + M[10] * k0 + M[11];
        u = x0/w0;
        v = y0/w0;

//        s[i*2] = u / 1024;
//        s[i*2+1] = 1.0 - v / 1024;
//        continue;

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
        // 3. 经 长宽等宽，归一化 和 边界截取，转换为纹理坐标 s
//        Y = Y * 640 / 480; // 原尺寸为 640x480， 将 dim2 统一到 640. 下面是归一化
//        s[i*2] = X / 640;
//        s[i*2+1] = 1.0 - Y / 640;
        s[i*2] = X / 1024;
        s[i*2+1] = 1.0 - Y / 1024;
//        s[i*2] = Y > 0 ? (Y <= 640 ? (Y / 640) : 1) : 0;  // TODO: X 和 Y 的先后？
//        s[i*2+1] = X > 0 ? (X <= 640 ? (X / 640) : 1) : 0;
    }
}

/*
 *     vector<Point2f> pts_dst, pts_src;
    pts_dst.reserve(4), pts_dst.reserve(4);
    // lu = left up , ru = right up , etc...
    Point2f pts_lu, pts_ru, pts_rb, pts_lb;
    int w = 0, h = 0, displace_l = 0, displace_r = 0;
    // Set points in region of interest
    pts_lu = Point2f(0, 620); // 0 620
    pts_ru = Point2f(1023, 620); // 1023 620
    pts_rb = Point2f(1023, 870); // 1023 870
    pts_lb = Point2f(0, 870); // 0 870
    w = 900; h = 300;
    displace_l = 250; displace_r = 250;
    Size size(w, h);
    Mat im_dst(size, CV_8UC4, Scalar(0, 0, 0, 0));
    Mat homogr;
    pts_src.push_back(pts_lu);
    pts_src.push_back(pts_ru);
    pts_src.push_back(pts_rb);
    pts_src.push_back(pts_lb);
    pts_dst.emplace_back(0.0f, 0.0f);
    pts_dst.emplace_back(size.width - 1.0f, 0.0f);
    pts_dst.emplace_back(float(size.width - displace_r), float(size.height - 1));
    pts_dst.emplace_back(float(displace_l), float(size.height - 1));
    CvMat *object_points = cvCreateMat(4, 3, CV_64FC1);
    CvMat *image_points = cvCreateMat(4, 2, CV_64FC1);
    for(int i=0; i<4; ++i){
        CV_MAT_ELEM(*object_points, double, i, 0) = pts_dst[0].x;
        CV_MAT_ELEM(*object_points, double, i, 1) = pts_dst[0].y;
        CV_MAT_ELEM(*object_points, double, i, 2) = 0.0f;
        CV_MAT_ELEM(*image_points, double, i, 0) = pts_src[0].x;
        CV_MAT_ELEM(*image_points, double, i, 1) = pts_src[0].y;
    }
    CvMat *intrinsic, *distortion;
    CvMat MM = K.getMat(), DD = D.getMat();
    intrinsic = &MM;
    distortion = &DD;

    CvMat *rotation_vector = cvCreateMat(3, 1, CV_32FC1);
    CvMat *rotation_mat = cvCreateMat(3, 3, CV_32FC1);
    CvMat *translation_vector = cvCreateMat(3, 1, CV_32FC1);
    cvFindExtrinsicCameraParams2(object_points, image_points, intrinsic, distortion, rotation_vector, translation_vector);
    cvRodrigues2(rotation_vector, rotation_mat);
    cv::Matx34d rr;
    for(int i=0; i<3; ++i){
        for(int j=0; j<3; ++j){
            rr(i, j) = CV_MAT_ELEM(*rotation_mat, double, i, j);
        }
        rr(i, 3) = CV_MAT_ELEM(*translation_vector, double, i, 0);
    }
 */