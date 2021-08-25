/*
 * Based on main.cpp, content substituted with rawcode
 */
//#include <glad/glad.h>
#include <iostream>
#include <GL/glut.h>
//#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "model.h"
#define PI 3.14159265359

static GLuint texName[4];//存放纹理
static GLint Model = 0;
static double Ox = 0;
static double Oy = 0;
static double direction = 0;//站立位置及前进方向；
static double step = 0.5;
GLUquadricObj*g_text;//曲面，制作一个包围房间的大球体做背景



void loadTexture(const char* filename,GLuint &texture)
{
    static GLint   ImageWidth;
    static GLint   ImageHeight;
    static GLint   PixelLength;
    static GLubyte* PixelData;

    // 打开文件
    FILE* pFile = fopen(filename, "rb");
    if (pFile == nullptr) {
        printf("[Notice] File %s not found.", filename);
        exit(0);
    }

    // 读取24位bmp图象的大小信息
    fseek(pFile, 0x0012, SEEK_SET);
    fread(&ImageWidth, sizeof(ImageWidth), 1, pFile);
    fread(&ImageHeight, sizeof(ImageHeight), 1, pFile);

    // 计算像素数据长度
    PixelLength = ImageWidth * 3;
    while (PixelLength % 4 != 0)//bmp图像保证数据区域的长度是4的倍数
        ++PixelLength;
    PixelLength *= ImageHeight;

    // 读取像素数据
    PixelData = (GLubyte*)malloc(PixelLength);
    if (PixelData == nullptr)
        exit(0);
    fseek(pFile, 54, SEEK_SET);
    fread(PixelData, PixelLength, 1, pFile);

    // 关闭文件
    fclose(pFile);

    //加载纹理
    glGenTextures(1, &texture);//用来生成纹理的数量  存储纹理索引的第一个元素指针
    glBindTexture(GL_TEXTURE_2D, texture);//绑定（指定）纹理对象
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   //纹理参数：坐标超过0.0~1.0f时的处理方式--重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  //纹理参数：贴图插值方式--最近邻 / 线性插值 GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImageWidth, ImageHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, PixelData);

    //生成一个2D纹理（Texture）。bmp图片的像素顺序是BGR所以用GL_BGR_EXT来反向加载数据

}

void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);//清除颜色缓冲区，设置颜色
    glShadeModel(GL_FLAT);//设置着色模式 GL_FLAT 恒定着色，GL_SMOOTH光滑着色
    glEnable(GL_DEPTH_TEST);
    g_text = gluNewQuadric();//这句代码需放置在绘图及显示之前
    loadTexture("../resources/floor.bmp",texName[0]);//图片与程序源代码放置在同一目录下即可
    loadTexture("../resources/wall.bmp", texName[1]);
    loadTexture("../resources/ceiling.bmp",texName[2]);
    loadTexture("../resources/back.bmp", texName[3]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//所读取的图像数据的行对齐方式
    Model = GL3DS_initialize_model();  // 生成模型的显示列表，保存显示列表号
}

// 定义纹理模型并刷新双缓存
void display(){
    glColor3f(0.5, 0.5, 0.5);
    // 清除屏幕
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色缓冲以及深度缓冲
    glEnable(GL_BLEND);

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//纹理和材质混合方式

    // 创建背景球体
    glBindTexture(GL_TEXTURE_2D, texName[3]);
    gluSphere(g_text, 30, 15, 15);
    gluQuadricTexture(g_text, GLU_TRUE);             //建立纹理坐标
    gluQuadricDrawStyle(g_text, GLU_FILL);           //用面填充

    // 地面部分
    glBindTexture(GL_TEXTURE_2D, texName[0]);//floor
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, -10.0, 0.0);
    glTexCoord2f(0.0, 1.0);glVertex3f(-10.0, 10.0, 0.0);
    glTexCoord2f(1.0, 1.0);glVertex3f(10.0, 10.0, 0.0);
    glTexCoord2f(1.0, 0.0);glVertex3f(10.0, -10.0, 0.0);
    glEnd();

    // 墙体部分
    glBindTexture(GL_TEXTURE_2D, texName[1]);//wall
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, -10.0, 0.0);
    glTexCoord2f(0.0, 10.0);glVertex3f(-10.0, -10.0, 10.0);
    glTexCoord2f(10.0, 10.0);glVertex3f(-10.0, 10.0, 10.0);
    glTexCoord2f(10.0, 0.0);glVertex3f(-10.0, 10.0, 0);

    glTexCoord2f(0.0, 0.0);glVertex3f(10.0, -10.0, 0.0);
    glTexCoord2f(0.0, 10.0);glVertex3f(10.0, -10.0, 10.0);
    glTexCoord2f(10.0, 10.0);glVertex3f(10.0, 10.0, 10.0);
    glTexCoord2f(10.0, 0.0);glVertex3f(10.0, 10.0, 0);

    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, 10.0, 0.0);
    glTexCoord2f(0.0, 10.0);glVertex3f(-10.0, 10.0, 10.0);
    glTexCoord2f(10.0, 10.0);glVertex3f(10.0, 10.0, 10.0);
    glTexCoord2f(10.0, 0.0);glVertex3f(10.0, 10.0, 0.0);

    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, -10.0, 0.0);
    glTexCoord2f(0.0, 10.0);glVertex3f(-10.0, -10.0, 10.0);
    glTexCoord2f(10.0, 10.0);glVertex3f(10.0, -10.0, 10.0);
    glTexCoord2f(10.0, 0.0);glVertex3f(10.0, -10.0, 0);
    glEnd();

    // 封顶部分
    glBindTexture(GL_TEXTURE_2D, texName[2]);//ceiling
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, -10.0, 10.0);
    glTexCoord2f(0.0, 1.0);glVertex3f(-10.0, 10.0, 10.0);
    glTexCoord2f(1.0, 1.0);glVertex3f(10.0, 10.0, 10.0);
    glTexCoord2f(1.0, 0.0);glVertex3f(10.0, -10.0, 10.0);
    glEnd();

    // 3ds model
    glCallList(Model);  // 调用显示列表

    glFlush();
    glutSwapBuffers();//交换双缓存
    glutPostRedisplay();//标志下次刷新，没有的话，程序打开后不会刷新界面
}
void display_old()
{
    // 清除屏幕
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色缓冲以及深度缓冲
    // 启用二维纹理
    glBindTexture(GL_TEXTURE_2D, texName[0]);//floor

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//纹理和材质混合方式
    glPushMatrix(); // 保存当前模型视图矩阵
    glRotatef((GLfloat)direction, 0.0, 0.0, 1.0);   // 用变换后(旋转或移动)模型坐标系建立模型
    glTranslated(Ox, Oy, 0.0);
//
//    // 创建背景球体
//    glBindTexture(GL_TEXTURE_2D, texName[3]);
//    gluSphere(g_text, 30, 15, 15);
//    gluQuadricTexture(g_text, GLU_TRUE);             //建立纹理坐标
//    gluQuadricDrawStyle(g_text, GLU_FILL);           //用面填充
//
//    // 地面部分
//    glBindTexture(GL_TEXTURE_2D, texName[0]);//floor
//    glBegin(GL_QUADS);
//    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, -10.0, 0.0);
//    glTexCoord2f(0.0, 1.0);glVertex3f(-10.0, 10.0, 0.0);
//    glTexCoord2f(1.0, 1.0);glVertex3f(10.0, 10.0, 0.0);
//    glTexCoord2f(1.0, 0.0);glVertex3f(10.0, -10.0, 0.0);
//    glEnd();
//
//    // 墙体部分
//    glBindTexture(GL_TEXTURE_2D, texName[1]);//wall
//    glBegin(GL_QUADS);
//    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, -10.0, 0.0);
//    glTexCoord2f(0.0, 10.0);glVertex3f(-10.0, -10.0, 10.0);
//    glTexCoord2f(10.0, 10.0);glVertex3f(-10.0, 10.0, 10.0);
//    glTexCoord2f(10.0, 0.0);glVertex3f(-10.0, 10.0, 0);
//
//    glTexCoord2f(0.0, 0.0);glVertex3f(10.0, -10.0, 0.0);
//    glTexCoord2f(0.0, 10.0);glVertex3f(10.0, -10.0, 10.0);
//    glTexCoord2f(10.0, 10.0);glVertex3f(10.0, 10.0, 10.0);
//    glTexCoord2f(10.0, 0.0);glVertex3f(10.0, 10.0, 0);
//
//    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, 10.0, 0.0);
//    glTexCoord2f(0.0, 10.0);glVertex3f(-10.0, 10.0, 10.0);
//    glTexCoord2f(10.0, 10.0);glVertex3f(10.0, 10.0, 10.0);
//    glTexCoord2f(10.0, 0.0);glVertex3f(10.0, 10.0, 0.0);
//
//    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, -10.0, 0.0);
//    glTexCoord2f(0.0, 10.0);glVertex3f(-10.0, -10.0, 10.0);
//    glTexCoord2f(10.0, 10.0);glVertex3f(10.0, -10.0, 10.0);
//    glTexCoord2f(10.0, 0.0);glVertex3f(10.0, -10.0, 0);
//    glEnd();
//
//    // 封顶部分
//    glBindTexture(GL_TEXTURE_2D, texName[2]);//ceiling
//    glBegin(GL_QUADS);
//    glTexCoord2f(0.0, 0.0);glVertex3f(-10.0, -10.0, 10.0);
//    glTexCoord2f(0.0, 1.0);glVertex3f(-10.0, 10.0, 10.0);
//    glTexCoord2f(1.0, 1.0);glVertex3f(10.0, 10.0, 10.0);
//    glTexCoord2f(1.0, 0.0);glVertex3f(10.0, -10.0, 10.0);
//    glEnd();

    glCallList(Model);  // 调用显示列表

    glPopMatrix();  // 恢复保存的视角
    glFlush();//用于强制刷新缓存
    glutSwapBuffers();//交换双缓存
    glutPostRedisplay();//标志下次刷新，没有的话，程序打开后不会刷新界面
}

// 在窗口改变时调整投影矩阵(视角)，模型矩阵(glulookat)
// w, h 是窗口的宽高
void reshape(int w, int h)
{
// viewport settings
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);//(设置显示区域的大小)在默认情况下，视口被设置为占据打开窗口的整个像素矩形

// projection settings
    glMatrixMode(GL_PROJECTION);//选择投影矩阵
    glLoadIdentity();//重置当前指定的矩阵为单位矩阵
    gluPerspective(90.0, (GLfloat)w / (GLfloat)h, 0.1, 10000.0);//(设置虚拟投影面的宽高比)
    // 一般和视口尺寸一致，否则会引起拉伸

// model / viewpoint settings
    glMatrixMode(GL_MODELVIEW);//模型视景矩阵||GL_TEXTURE,对纹理矩阵堆栈 应用随后的矩阵操作.
    glLoadIdentity();
    //通过设置视点来设置模型坐标系
    //glScalef(3, 3, 1);
    //gluLookAt(0, -40, 45, 0, 0, 0, 0, 1, 0);
    gluLookAt(0, 0, 3, 0,  10, 3, 0.0, 0.0, 1.0);
}

// 定义键盘事件时的动作
void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 'w':
        case 'W':
            Ox -= step*sin(direction / 180 * PI);
            Oy -= step*cos(direction / 180 * PI);
            glutPostRedisplay();
            break;
        case 's':
        case 'S':
            Ox += step*sin(direction/180*PI);
            Oy += step*cos(direction/180*PI);
            glutPostRedisplay();
            break;
        case 'a':
        case 'A':
            direction = direction - 2;
            glutPostRedisplay();
            break;
        case 'd':
        case 'D':
            direction = direction + 2;
            glutPostRedisplay();
            break;
        case 27:
            exit(0);
        default:
            break;
    }
}

void print_version(){
    const GLubyte* name = glGetString(GL_VENDOR); //返回负责当前OpenGL实现厂商的名字
    const GLubyte* biaoshifu = glGetString(GL_RENDERER); //返回一个渲染器标识符，通常是个硬件平台
    const GLubyte* OpenGLVersion =glGetString(GL_VERSION); //返回当前OpenGL实现的版本号
    const GLubyte* gluVersion= gluGetString(GLU_VERSION); //返回当前GLU工具库版本
    GLint max;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
    printf("OpenGL实现厂商的名字：%s\n", name);
    printf("渲染器标识符：%s\n", biaoshifu);
    printf("OOpenGL实现的版本号：%s\n",OpenGLVersion );
    printf("OGLU工具库版本：%s\n", gluVersion);
    printf("最大支持图片像素个数：%d\n", max);
//    printf("GLFloat size: %d\n", sizeof(GL_FLOAT));
}

int main(int argc, char* argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 800);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);

    init();
    print_version();
    glutDisplayFunc(&display_old);
//    glutIdleFunc(&display);
    glutReshapeFunc(&reshape);
    glutKeyboardFunc(&keyboard);
    glutMainLoop();

    return 0;
}