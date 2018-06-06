#ifndef QGLWIDGETIMPL_H
#define QGLWIDGETIMPL_H

#include <GL/glew.h>
#include <qopenglwidget.h>

// GL PixelFormat extend
#define GL_I420				0x1910

#define A(color) (((color) >> 24) & 0xFF)
#define R(color) (((color) >> 16) & 0xFF)
#define G(color) (((color) >>  8) & 0xFF)
#define B(color) ((color) & 0xFF)

struct Texture{
    GLuint texID; // glGenTextures分配的ID
    GLuint type; // 数据类型如GL_RGB
    GLint width;
    GLint height;
    GLint bpp;
    GLubyte* data; // 像素数据

    Texture(){
        texID = 0;
        type = GL_RGBA;
        width = 0;
        height = 0;
        bpp = 0;
        data = NULL;
    }

    ~Texture(){
        release();
    }

    void release(){
        if (data != NULL){
            free(data);
            data = NULL;
        }
        width = 0;
        height = 0;
        if (texID != 0){
            glDeleteTextures(1,&texID);
        }
    }

    bool alloc(int w, int h, int type = GL_I420){
        if (w == 0 || h == 0)
            return false;

        int stdw = w >> 3 << 3;
        int stdh = h >> 1 << 1;
        if (data && width == stdw && height == stdh)
            return true;

        release();
        width = stdw;
        height = stdh;
        type = GL_I420;
        data = (unsigned char *)malloc(width * height * 3 / 2);
        return data != NULL;
    }
};

struct DrawInfo{
    int left;
    int right;
    int top;
    int bottom;
    GLuint color;
};

void bindTexture(Texture* tex, QImage* img);

class QGLWidgetImpl : public QOpenGLWidget
{
    Q_OBJECT
public:
    QGLWidgetImpl(QWidget* parent = Q_NULLPTR);
    virtual ~QGLWidgetImpl();

    void setVertices(double ratio);
    void setVertices(QRect rc);

protected:
    static void loadYUVShader();
    void initVAO();
    void drawYUV(Texture* tex);
    void drawTex(Texture* tex, DrawInfo* di);
    void drawRect(DrawInfo* di, int linewidth = 1, bool bFill = false);

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL(){}

protected:
    static bool s_bInitGLEW;
    static GLuint prog_yuv;
    static GLuint texUniformY;
    static GLuint texUniformU;
    static GLuint texUniformV;

    GLuint tex_yuv[3];
    enum E_VER_ATTR{ver_attr_ver = 3, ver_attr_tex = 4, ver_attr_num};

    GLfloat vertices[8];
};

#endif // QGLWIDGETIMPL_H
