/*
 * Based on main.cpp, content substituted with rawcode
 * 原版 main 的逻辑加入 rawcode 中使用碗状模型和贴图的部分
 */
//#include <glad/glad.h>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include "main.h"
#include "model.h"
#include "bowl.h"
#include "newbowl.h"
#include "coord_calculator.h"
#define PI 3.14159265359
using namespace std;

void coord_init();
static GLuint texName[4];//存放纹理
static GLint Model = 0; // 模型 calllist 号
static double Ox = 0;   // 视角旋转参数
static double Oy = 0;
static double direction = 0; //站立位置及前进方向；
static double step = 0.5;
GLUquadricObj*g_text;//曲面，制作一个包围房间的大球体做背景

static GLfloat m[540];  // 180个点
static GLfloat s[360];

/*
 * 加载单张图片，作为纹理资源
 */
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);   //纹理参数：坐标超过0.0~1.0f时的处理方式--重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  //纹理参数：贴图插值方式--最近邻 / 线性插值 GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ImageWidth, ImageHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, PixelData);

    //生成一个2D纹理（Texture）。bmp图片的像素顺序是BGR所以用GL_BGR_EXT来反向加载数据

}

/*
 * 初始化基本设置以及各项资源
 */
void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);//清除颜色缓冲区，设置颜色
    glShadeModel(GL_FLAT);//设置着色模式 GL_FLAT 恒定着色，GL_SMOOTH光滑着色
    glEnable(GL_DEPTH_TEST);
    g_text = gluNewQuadric();//这句代码需放置在绘图及显示之前
    loadTexture("../resources/room/0.bmp",texName[0]);//图片与程序源代码放置在同一目录下即可
    loadTexture("../resources/left0.bmp", texName[1]);
    loadTexture("../resources/right0.bmp",texName[2]);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);//所读取的图像数据的行对齐方式
    // 自定义框架模型的显示列表，保存显示列表号
//    Model = GL3DS_initialize_model();
//    Model = GL3DS_initialize_bowl();
    Model = GL3DS_initialize_newbowl();
    if(Model==0){
        printf("读取模型失败");
        exit(1);
    }
    // 计算映射所需数组
    coord_init();
}

/*
 * 初始化顶点坐标，并计算映射关系
 */
void coord_init() {
    // 顶点初始化
    get_vertices(m);
    // 加载参数
    cv::Mat K, D, T;
    cv::FileStorage parser;
    string filepath = "../resources/room/2.ymal";
    if (parser.open(filepath, cv::FileStorage::READ)) {
        string camerastr="cameraMatrix";
        string disstr="distCoeffs";
        parser[camerastr]>>K;
        parser[disstr]>>D;
        parser.release();
    }
    filepath = "../resources/room/trans_2.ymal";
    if (parser.open(filepath, cv::FileStorage::READ)) {
        parser["trans"]>>T;
        parser.release();
    }
    // 计算映射
    cv::Size sz(1280, 2560);
    coord_calculator::calc_coord(K, D, T, sz, 0.01, m, s, 180);
}

/*
 * 绘制目标模型，以及贴图等
 */
void draw_objects(){
    // 1. 获取显示列表中所有点的坐标，并分到left, right, front, rear 四个数组（可先处理一个）（init 中）
    // 2. 计算4个数组分别对应的像素坐标  用 coord_calculator (init 中)
    // 3. 纹理映入，使用 glDrawArrays
    //纹理映入
    glBindTexture(GL_TEXTURE_2D, texName[0]); //1front //2left //0tail //3right
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

    glVertexPointer(3, GL_FLOAT, 0, m);
    glTexCoordPointer(2, GL_FLOAT, 0, s);
//    glDrawArrays(GL_TRIANGLE_STRIP, 0, 180);    // 此处应与newbowl.h
    glDrawElements(GL_TRIANGLES, 1872, GL_UNSIGNED_INT, &GL3DS_INDEX_newbowl[0]);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

//    glDisable(GL_BLEND);
    // 3ds model
//    glCallList(Model);  // 调用显示列表
    glDisable(GL_TEXTURE_2D);
}

/*
 * 注册的display函数，在OpenGL主函数中作为循环入口
 */
void display(){
    glColor3f(0.5, 0.5, 0.5);
//    // 清除屏幕
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//清除颜色缓冲以及深度缓冲
//    glEnable(GL_BLEND);
//    // 启用二维纹理
//    glBindTexture(GL_TEXTURE_2D, texName[0]);//floor
//    glEnable(GL_TEXTURE_2D);
//    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);//纹理和材质混合方式
//
    glPushMatrix(); // 保存当前模型视图矩阵
    glRotatef((GLfloat)direction, 0.0, 0.0, 1.0);   // 用变换后(旋转或移动)模型坐标系建立模型
    glTranslated(Ox, Oy, 0.0);

    // 设置面模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // 绘制模型和纹理
    draw_objects();

    glPopMatrix();  // 恢复保存的视角
    glFlush();
    glutSwapBuffers();//交换双缓存
    glutPostRedisplay();//标志下次刷新，没有的话，程序打开后不会刷新界面
}

/*
 * reshape 函数
 * 用于在窗口改变时调整投影矩阵(视角)，模型矩阵(glulookat)
 * w, h 是窗口的宽高
 */
void reshape(int w, int h)
{
// viewport settings
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);//(设置显示区域的大小)在默认情况下，视口被设置为占据打开窗口的整个像素矩形

// projection settings
    glMatrixMode(GL_PROJECTION);//选择投影矩阵(摄像机视角矩阵)
    glLoadIdentity();//重置当前指定的矩阵为单位矩阵
    gluPerspective(90.0, (GLfloat)w / (GLfloat)h, 0.1, 10000.0);//(设置虚拟投影面的宽高比)
    // 一般和视口尺寸一致，否则会引起拉伸

// model / viewpoint settings
    glMatrixMode(GL_MODELVIEW);//模型视景矩阵||GL_TEXTURE,对纹理矩阵堆栈 应用随后的矩阵操作.
    glLoadIdentity();
    //通过设置视点来设置模型坐标系
//    glScalef(0.1f, 0.1f, 0.1f);
//    gluLookAt(0, -40, 45, 0, 0, 0, 0, 1, 0);
    gluLookAt(0, 0, 20, 0,  10, 10, 0.0, 0.0, 1.0);
//    gluLookAt(0, 0, 50, 0,  0, 0, 0.0, 1.0, 0.0);

}

/*
 * 定义键盘事件时的动作
 */
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

/*
 * 输出版本信息
 */
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
    print_version();

    init(); // 初始化
    glutDisplayFunc(&display);  // 注册 display 函数
//    glutIdleFunc(&display);
    glutReshapeFunc(&reshape);  // 注册 reshape 函数
    glutKeyboardFunc(&keyboard);    // 注册键盘响应函数
    glutMainLoop(); // 开始主循环

    return 0;
}