#include "qglwidgetimpl.h"

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
bool QGLWidgetImpl::s_bInitGLEW = false;
GLuint QGLWidgetImpl::prog_yuv;
GLuint QGLWidgetImpl::texUniformY;
GLuint QGLWidgetImpl::texUniformU;
GLuint QGLWidgetImpl::texUniformV;

QGLWidgetImpl::QGLWidgetImpl(QWidget* parent)
    : QOpenGLWidget(parent)
{

}

QGLWidgetImpl::~QGLWidgetImpl()
{

}

#include <QApplication>
#include <QMessageBox>
void QGLWidgetImpl::initializeGL(){
    if (!s_bInitGLEW){
        if (glewInit() != 0){
            qFatal("glewInit failed");
            return;
        }

        loadYUVShader();
        s_bInitGLEW = true;
    }

    initVAO();
}

void QGLWidgetImpl::resizeGL(int w, int h){
    glViewport(0,0,w,h);
}

void QGLWidgetImpl::loadYUVShader(){
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

    char szVS[] = "				\
    attribute vec4 verIn;		\
    attribute vec2 texIn;		\
    varying vec2 texOut;		\
                                \
    void main(){				\
        gl_Position = verIn;	\
        texOut = texIn;			\
    }							\
    ";
    const GLchar* pszVS = szVS;
    GLint len = strlen(szVS);
    glShaderSource(vs, 1, (const GLchar**)&pszVS, &len);

    char szFS[] = "				\
    varying vec2 texOut;		\
    uniform sampler2D tex_y;	\
    uniform sampler2D tex_u;	\
    uniform sampler2D tex_v;	\
                                \
    void main(){				\
        vec3 yuv;				\
        vec3 rgb;				\
        yuv.x = texture2D(tex_y, texOut).r;			\
        yuv.y = texture2D(tex_u, texOut).r - 0.5;	\
        yuv.z = texture2D(tex_v, texOut).r - 0.5;	\
        rgb = mat3( 1,       1,         1,			\
            0,       -0.39465,  2.03211,			\
            1.13983, -0.58060,  0) * yuv;			\
        gl_FragColor = vec4(rgb, 1);				\
    }												\
    ";
    const GLchar* pszFS = szFS;
    len = strlen(szFS);
    glShaderSource(fs, 1, (const GLchar**)&pszFS, &len);

    glCompileShader(vs);
    glCompileShader(fs);

//#ifdef _DEBUG
    GLint iRet = 0;
    glGetShaderiv(vs, GL_COMPILE_STATUS, &iRet);
    qDebug("vs::GL_COMPILE_STATUS=%d", iRet);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &iRet);
    qDebug("fs::GL_COMPILE_STATUS=%d", iRet);
//#endif

    prog_yuv = glCreateProgram();

    glAttachShader(prog_yuv, vs);
    glAttachShader(prog_yuv, fs);

    glBindAttribLocation(prog_yuv, ver_attr_ver, "verIn");
    glBindAttribLocation(prog_yuv, ver_attr_tex, "texIn");

    glLinkProgram(prog_yuv);

//#ifdef _DEBUG
    glGetProgramiv(prog_yuv, GL_LINK_STATUS, &iRet);
    qDebug("prog_yuv=%d GL_LINK_STATUS=%d", prog_yuv, iRet);
//#endif

    glValidateProgram(prog_yuv);

    texUniformY = glGetUniformLocation(prog_yuv, "tex_y");
    texUniformU = glGetUniformLocation(prog_yuv, "tex_u");
    texUniformV = glGetUniformLocation(prog_yuv, "tex_v");

    qDebug("loadYUVShader ok");

    //glUseProgram(prog_yuv);
}

void QGLWidgetImpl::setVertices(double ratio){
    double w = 1.0, h = 1.0;
    if (ratio < 1.0){
        w = ratio;
    }else{
        h = 1.0 / ratio;
    }

    GLfloat tmp[] = {
            -w, -h,
             w, -h,
            -w,  h,
             w,  h,
    };

    memcpy(vertices, tmp, sizeof(GLfloat)*8);
}

void QGLWidgetImpl::setVertices(QRect rc){
    int wnd_w = width();
    int wnd_h = height();
    if (wnd_w <= 0 || wnd_h <= 0)
        return;
    GLfloat left = (GLfloat)rc.left() * 2 / wnd_w - 1;
    GLfloat right = (GLfloat)(rc.right()+1) * 2 / wnd_w - 1;
    GLfloat top = 1 - (GLfloat)rc.top() * 2 / wnd_h;
    GLfloat bottom = 1 - (GLfloat)(rc.bottom()+1) * 2 / wnd_h;
    qDebug("l=%d, r=%d, t=%d, b=%d", rc.left(), rc.right(), rc.top(), rc.bottom());
    qDebug("l=%f, r=%f, t=%f, b=%f", left, right, top, bottom);
    GLfloat tmp[] = {
        left,  bottom,
        right, bottom,
        left,  top,
        right, top
    };

    memcpy(vertices, tmp, sizeof(GLfloat)*8);
}

void QGLWidgetImpl::initVAO(){
//    static const GLfloat vertices[] = {
//        -1.0f, -1.0f,
//         1.0f, -1.0f,
//        -1.0f,  1.0f,
//         1.0f,  1.0f,
//    };

    setVertices(1.0);

    static const GLfloat textures[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    // reverse
//    static const GLfloat textures[] = {
//        0.0f, 0.0f,
//        1.0f, 0.0f,
//        0.0f, 1.0f,
//        1.0f, 1.0f,
//    };

    //glGenBuffers(buffer_num, g_buffers);
    //glBindBuffer(GL_ARRAY_BUFFER, g_buffers[buffer_ver]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    //glVertexAttribPointer(ver_attr_ver, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(ver_attr_ver, 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glEnableVertexAttribArray(ver_attr_ver);

    //glGenBuffers(buffer_num, g_buffers);
    //glBindBuffer(GL_ARRAY_BUFFER, g_buffers[buffer_tex]);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(textures), textures, GL_STATIC_DRAW);
    //glVertexAttribPointer(ver_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribPointer(ver_attr_tex, 2, GL_FLOAT, GL_FALSE, 0, textures);
    glEnableVertexAttribArray(ver_attr_tex);

    glGenTextures(3, tex_yuv);
    for (int i = 0; i < 3; i++){
        glBindTexture(GL_TEXTURE_2D, tex_yuv[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}

void QGLWidgetImpl::drawYUV(Texture* tex){
    glUseProgram(prog_yuv);

    int w = tex->width;
    int h = tex->height;
    int y_size = w*h;
    GLubyte* y = tex->data;
    GLubyte* u = y + y_size;
    GLubyte* v = u + (y_size>>2);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_yuv[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, y);
    glUniform1i(texUniformY, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex_yuv[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w/2, h/2, 0, GL_RED, GL_UNSIGNED_BYTE, u);
    glUniform1i(texUniformU, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, tex_yuv[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w/2, h/2, 0, GL_RED, GL_UNSIGNED_BYTE, v);
    glUniform1i(texUniformV, 2);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glUseProgram(0);
}

void QGLWidgetImpl::drawTex(Texture* tex, DrawInfo* di){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width(), height(), 0.0, -1.0, 1.0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->texID);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_QUADS);
    glTexCoord2d(0,0);glVertex2i(di->left, di->top);
    glTexCoord2d(1,0);glVertex2i(di->right, di->top);
    glTexCoord2d(1,1);glVertex2i(di->right, di->bottom);
    glTexCoord2d(0,1);glVertex2i(di->left, di->bottom);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

void QGLWidgetImpl::drawRect(DrawInfo* di, int linewidth, bool bFill){
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, width(), height(), 0.0, -1.0, 1.0);

    if (bFill){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }else{
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glLineWidth(linewidth);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4ub(R(di->color), G(di->color), B(di->color), A(di->color));
    glRecti(di->left, di->top, di->right, di->bottom);
    glColor4ub(255,255,255,255);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_BLEND);
}

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void bindTexture(Texture* tex, QImage* img){
    if (img->format() != QImage::Format_ARGB32)
        return;

    glGenTextures(1, &tex->texID);
    glBindTexture(GL_TEXTURE_2D, tex->texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    tex->width = img->width();
    tex->height = img->height();
    tex->type = GL_BGRA;
    tex->bpp = img->bitPlaneCount();
    gluBuild2DMipmaps(GL_TEXTURE_2D, tex->bpp/8, tex->width, tex->height, tex->type, GL_UNSIGNED_BYTE, img->bits());
    //glTexImage2D(GL_TEXTURE_2D, 0, tex->bpp/8, tex->width, tex->height, 0, tex->type, GL_UNSIGNED_BYTE, img->bits());
}
