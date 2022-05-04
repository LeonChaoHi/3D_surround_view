/*
 * Based on rawcode, style changed to glut.
 * 尝试在原代码上修改，但较难
 */
//基于OpenGL投算法实现
//#include <Windows.h>
#include <GL/glut.h>
//#include <GL/glaux.h>
#include <cv.h>
#include <cxcore.h>
#include <opencv2/imgcodecs/imgcodecs_c.h>
//#include <highgui\highgui_c.h>
//#include <core\types_c.h>
#include <stdio.h>
//#include "API912.h"
//#include "Type912.h"
//#include "calcsurface.h"
//#include "28.h"
#include "model.h"
#pragma comment(lib, "opencv_core231d.lib")
#pragma comment(lib, "opencv_imgproc231d.lib")
#pragma comment(lib, "opencv_highgui231d.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#define MAXSIZE 259200
#define MAX_FILE_PATH 200
void myinit(void);
void makeImage(void);
void DrawMyObjects(void);
//void CALLBACK myReshape(GLsizei w, GLsizei h);
//void CALLBACK display(void);
//void CALLBACK rotate(AUX_EVENTREC *event);
void myReshape(GLsizei w, GLsizei h);
void display(void);
//void rotate(AUX_EVENTREC *event);
GLubyte
    imagedata0[MAXSIZE],
    imagedata1[MAXSIZE], imagedata2[MAXSIZE], imagedata3[MAXSIZE];
GLsizei width = 360, height = 240;
GLint i, ii, p, n, Model;
//TVSInfo stTVSInfo;  // 标定参数
int Option = 100, dx = 700, dy = 700, dz = 700,
    dx1 = 700, dy1 = 600, dz1 = 700,
    dx2 = 700, dy2 = 600, dz2 = 700;
//dx = 700;
//dy = 700;
//dz = 700;
//dx1 = 700;
//dy1 = 600;
//dz1 = 700;
//dx2 = 700;
//dy2 = 600;
//dz2 = 700;
//Option = 100;
IplImage *image[4];
GLuint texture[4];

// 定义纹理数据
void makeImage(void)
{
    image[0] = cvLoadImage("F:\\A56test\\22708\\tail.bmp", -1);
    image[1] = cvLoadImage("F:\\A56test\\22708\\front.bmp", -1);
    image[2] = cvLoadImage("F:\\A56test\\22708\\left.bmp", -1);
    image[3] = cvLoadImage("F:\\A56test\\22708\\right.bmp", -1);
    for (p = 0; p < image[0]->height; p++)
    {
        GLubyte *ptr = (GLubyte *)(image[0]->imageData + p * image[0]->widthStep);
        for (n = 0; n < image[0]->width; n++)
        {
            // write R G B value into imagedata[], 1D array.
            imagedata0[p * image[0]->widthStep + n * 3 + 0] = ptr[3 * n + 2];
            imagedata0[p * image[0]->widthStep + n * 3 + 1] = ptr[3 * n + 1];
            imagedata0[p * image[0]->widthStep + n * 3 + 2] = ptr[3 * n + 0];
        }
    }

    glGenTextures(1, &texture[0]);            // 为第i个位投创建纹理
    glBindTexture(GL_TEXTURE_2D, texture[0]);     // 将生成投纹理投名称绑定投指定投纹理上
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 360, 240, 0, GL_RGB, GL_UNSIGNED_BYTE,
                 imagedata0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glGenTextures(1, &texture[1]); // 为第i个位投创建纹理
}

// Initialize
void myinit()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    //makeImage();    // 载入纹理
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    //Model = GL3DS_initialize_28();  // 生成模型的显示列表，保存显示列表号
    Model = GL3DS_initialize_model();  // 生成模型的显示列表，保存显示列表号
}

//void CALLBACK myReshape(GLsizei w, GLsizei h)
void myReshape(GLsizei w, GLsizei h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, (GLfloat)w / (GLfloat)h, 0.1, 300);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    //OMINI
//    glScalef(3, 3, 1);
    gluLookAt(100, 0, 0, 0, 0, 0, 0, 0, 1);
    //front1
    // glScalef(2.5,2.5,1);
    // glTranslatef(0,0,27);
    // gluLookAt(0,-29,15,0,10,0,0,1,0);
    //front2
    // glScalef(2.5,2.5,1);
    // glTranslatef(0,0,27);
    // gluLookAt(0,-37,15,0,10,0,0,1,0);
    //left
    // glScalef(2.5,2.5,1);
    // glTranslatef(0,0,23);
    // glRotatef(-6,0,1,0);
    // gluLookAt(0,-35,15,0,12,0,0,1,0);
    //right
    // glScalef(3,3,1);
    // glTranslatef(0,0,23);
    // glRotatef(6,0,1,0);
    // gluLookAt(0,-40,15,0,12,0,0,1,0);
    //back
    // glScalef(3,3,1);
    // glTranslatef(0,-10,9);
    // glRotatef(180,0,0,1);
    // gluLookAt(0,40,15,0,10,0,0,1,0);
}

// render textures
//void CALLBACK display(void)
void display(void)
{
    glColor3f(0.5, 0.5, 0.5);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    DrawMyObjects();    // 绘制模型并纹理映射
    glFlush();
    glutSwapBuffers();//交换双缓存
    glutPostRedisplay();//标志下次刷新，没有的话，程序打开后不会刷新界面
}

void DrawMyObjects(void)
{

    //纹理映入
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    //glBindTexture(GL_TEXTURE_2D, texture[1]); //1front //2left //0tail //3right
   // glVertexPointer(3, GL_FLOAT, 0, m);
    //glTexCoordPointer(2, GL_FLOAT, 0, s);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 74);

    //接缝处
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //    //glColor4f(1.0f,1.0f,1.0f,0.7f);
    //glColorPointer(4, GL_FLOAT, 0, colorAC);
    //glBindTexture(GL_TEXTURE_2D, texture[1]);
    //glVertexPointer(3, GL_FLOAT, 0, m + 72);
    //glTexCoordPointer(2, GL_FLOAT, 0, sA);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glCallList(Model);  // 调用显示列表
    glDisable(GL_BLEND);
}
/*
void DrawMyObjects_old(void)
{
    GLfloat m[222], m1[222], m2[222], m3[222], m4[222], m5[222], m6[222];
    GLfloat mm[222], mm1[222], mm2[222], mm3[222], mm4[222];
    GLint i, j, k, n, p;
    GLdouble pWorld[4], pCam[4], pCame[4], pImg[2];
    GLfloat smA[18], smA1[18], smA2[18], smA3[18], smA4[18];
    GLfloat smB[16], smB1[16], smB2[16], smB3[16], smB4[16];
    GLfloat smC[16], smC1[16], smC2[16], smC3[16], smC4[16];
    GLfloat smD[16], smD1[16], smD2[16], smD3[16], smD4[16];
    GLfloat colorAC[24] = {1.0, 1.0, 0.9, 1.0, 1.0, 1.0, 1.0, 0.9,
                           1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0,
                           1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0};
    GLfloat colorBD[24] = {1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0,
                           1.0, 1.0, 1.0, 0.0, 1.0, 1.0, 1.0, 0.0,
                           1.0, 1.0, 1.0, 0.9, 1.0, 1.0, 1.0, 0.9};
    p = 0;
    glRotatef(-10, 1.0f, 0.0f, 0.0f);
    ThreeSurface(mm, mm1, mm2, mm3, mm4, m, m1, m2, m3);    //获取三维曲面顶投数据    // 获取三维顶点坐标

    // 计算映射坐标(已知3D坐标，求解2D纹理坐标)
    for (i = 36; i <= 75; i += 3)   // 13个点
    {
        pWorld[0] = -m[i + 1] * dx1;
        pWorld[1] = m[i] * dy1;
        pWorld[2] = m[i + 2] * dz1;
        pWorld[3] = 1;
        matrixMul(&stTVSInfo.staCameras[1].stCameraParamEXT.daExtParam, pWorld, pCam, 4,
                  4, 1);
        for (j = 0; j <= 2; j++)
        {
            pCame[j] = pCam[j];
        }
        Cam2ImgScara(pImg, pCame, &stTVSInfo.staCameras[0]);
        pImg[1] = pImg[1] * 360 / 320; // 原尺寸为 360x320， 将dim2统一到360 下面是归一化
        s[2 * p + 24 + 0] = pImg[1] > 0 ? (pImg[1] <= 360 ? (pImg[1] / 360) : 1) : 0;
        s[2 * p + 24 + 1] = pImg[0] > 0 ? (pImg[0] <= 240 ? (pImg[0] / 240) : 1) : 0;
        p++;
    }
    p = 0;
    for (i = 72; i <= 147; i += 3)  // 25 points
    {
        pWorld[0] = -m1[i + 1] * dx;
        pWorld[1] = m1[i] * dy;
        pWorld[2] = m1[i + 2] * dz;
        pWorld[3] = 1;
        matrixMul(&stTVSInfo.staCameras[2].stCameraParamEXT.daExtParam, pWorld, pCam, 4,
                  4, 1);
        for (j = 0; j <= 2; j++)
        {
            pCame[j] = pCam[j];
        }
        Cam2ImgScara(pImg, pCame, &stTVSInfo.staCameras[0]);
        pImg[1] = pImg[1] * 360 / 320;
        s1left[2 * p + 48 + 0] = pImg[1] > 0 ? (pImg[1] <= 360 ? (pImg[1] / 360) : 1) : 0;
        s1left[2 * p + 48 + 1] = pImg[0] > 0 ? (pImg[0] <= 240 ? (pImg[0] / 240) : 1) : 0;
        p++;
    }
    p = 0;
    for (i = 144; i <= 183; i += 3)     // 13 points
    {
        pWorld[0] = -m1[i + 1] * dx2;
        pWorld[1] = m1[i] * dy2;
        pWorld[2] = m1[i + 2] * dz2;
        pWorld[3] = 1;
        matrixMul(&stTVSInfo.staCameras[0].stCameraParamEXT.daExtParam, pWorld, pCam, 4,
                  4, 1);
        for (j = 0; j <= 2; j++)
        {
            pCame[j] = pCam[j];
        }
        Cam2ImgScara(pImg, pCame, &stTVSInfo.staCameras[0]);
        pImg[1] = pImg[1] * 360 / 320;
        s1rear[2 * p + 96 + 0] = pImg[1] > 0 ? (pImg[1] <= 360 ? (pImg[1] / 360) : 1) : 0;
        s1rear[2 * p + 96 + 1] = pImg[0] > 0 ? (pImg[0] <= 240 ? (pImg[0] / 240) : 1) : 0;
        p++;
    }
    p = 0;
    for (i = 180; i <= 255; i += 3)     // 25 points
    {
        ii = i;
        if (ii > 219)
        {
            ii = i - 222 + 6;
        }
        pWorld[0] = -m1[ii + 1] * dx;
        pWorld[1] = m1[ii] * dy;
        pWorld[2] = m1[ii + 2] * dz;
        pWorld[3] = 1;
        matrixMul(&stTVSInfo.staCameras[3].stCameraParamEXT.daExtParam, pWorld, pCam, 4,
                  4, 1);
        for (j = 0; j <= 2; j++)
        {
            pCame[j] = pCam[j];
        }
        Cam2ImgScara(pImg, pCame, &stTVSInfo.staCameras[0]);
        pImg[1] = pImg[1] * 360 / 320;
        s1right[2 * p + 120 + 0] = pImg[1] > 0 ? (pImg[1] <= 360 ? (pImg[1] / 360) : 1) : 0;
        s1right[2 * p + 120 + 1] = pImg[0] > 0 ? (pImg[0] <= 240 ? (pImg[0] / 240) : 1) : 0;
        p++;
    }
    //接缝处
    p = 0;
    for (i = 72; i <= 87; i += 3)   // 5 points
    {
        pWorld[0] = -m[i + 1] * dx1;
        pWorld[1] = m[i] * dy1;
        pWorld[2] = m[i + 2] * dz1;
        pWorld[3] = 1;
        matrixMul(&stTVSInfo.staCameras[1].stCameraParamEXT.daExtParam, pWorld, pCam, 4,
                  4, 1);
        for (j = 0; j <= 2; j++)
        {
            pCame[j] = pCam[j];
        }
        Cam2ImgScara(pImg, pCame, &stTVSInfo.staCameras[0]);
        pImg[1] = pImg[1] * 360 / 320;
        sA[2 * p + 0] = pImg[1] > 0 ? (pImg[1] <= 360 ? (pImg[1] / 360) : 1) : 0;
        sA[2 * p + 1] = pImg[0] > 0 ? (pImg[0] <= 240 ? (pImg[0] / 240) : 1) : 0;
        p++;
    }
    p = 0;
    for (i = 72; i <= 87; i += 3)       //  5 points
    {
        pWorld[0] = -m1[i + 1] * dx1;
        pWorld[1] = m1[i] * dy1;
        pWorld[2] = m1[i + 2] * dz1;
        pWorld[3] = 1;
        matrixMul(&stTVSInfo.staCameras[1].stCameraParamEXT.daExtParam, pWorld, pCam, 4,
                  4, 1);
        for (j = 0; j <= 2; j++)
        {
            pCame[j] = pCam[j];
        }
        Cam2ImgScara(pImg, pCame, &stTVSInfo.staCameras[0]);
        pImg[1] = pImg[1] * 360 / 320;
        sA1[2 * p + 0] = pImg[1] > 0 ? (pImg[1] <= 360 ? (pImg[1] / 360) : 1) : 0;
        sA1[2 * p + 1] = pImg[0] > 0 ? (pImg[0] <= 240 ? (pImg[0] / 240) : 1) : 0;
        p++;
    }

    //纹理映入
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, texture[1]); //1front //2left //0tail //3right
    glVertexPointer(3, GL_FLOAT, 0, m);
    glTexCoordPointer(2, GL_FLOAT, 0, s);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 74);

    //接缝处
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glColor4f(1.0f,1.0f,1.0f,0.7f);
    glColorPointer(4, GL_FLOAT, 0, colorAC);
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glVertexPointer(3, GL_FLOAT, 0, m + 72);
    glTexCoordPointer(2, GL_FLOAT, 0, sA);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glCallList(Model);  // 调用显示列表
    glDisable(GL_BLEND);
}*/

// catch keyboard/mouse event
//void CALLBACK rotate(AUX_EVENTREC *event)
/*
void rotate(AUX_EVENTREC *event)
{
    GLint x, y;
    myinit();
    x = event->data[AUX_MOUSEX];
    y = event->data[AUX_MOUSEY];
}
 */

// 定义键盘事件时的动作
void keyboard(unsigned char key, int x, int y)
{}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    myinit();
    //print_version();
    glutDisplayFunc(&display);
//    glutIdleFunc(&display);
    glutReshapeFunc(&myReshape);
    glutKeyboardFunc(&keyboard);
    glutMainLoop();

    return 0;
    //TvsLibSetDefaultParam(&stTVSInfo, Option);  //初始化相机参数的函数
    //auxInitDisplayMode(AUX_SINGLE | AUX_RGBA);
    //auxInitPosition(0, 0, 400, 400);
    //auxInitWindow("Geometric Primitive Types");
    //myinit();
    //auxReshapeFunc(myReshape);
    //auxMouseFunc(AUX_LEFTBUTTON, AUX_MOUSEDOWN, rotate);
    //auxMainLoop(display);
}