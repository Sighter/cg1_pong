/*
 * Includes
 * {{{1 
 */
#include <stdlib.h>         // for rand()
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

// TODO check if this is compatible with windows
// http://www.cplusplus.com/forum/unices/60161/
//#include <unistd.h>


#define PI 3.14159265
// Our includes.
#include "CG1Helper.h"
#include "CGContext.h"
#include "CGMatrix.h"
#include "CGLightSource.h"
#include "deerVertexData.h" // vertex data is defined here
#include "CG1Application_renderSphere.h"
#include "CGMatrix.h"
#include "CGMath.h"
#include "CGQuadric.h"
// }}}

/*
 *  Defines, globals, etc.
 *  {{{1
 */
#define FRAME_WIDTH  480
#define FRAME_HEIGHT 480 //
#define FRAME_SCALE  1   // Integer scaling factors (zoom).

// global context pointer
CGContext *ourContext;

bool anim_cam;
bool use_light;
float cam_anim = 2.0;

enum sides {LEFT, RIGHT};
enum intel {HUMAN, MIGHTYBOT, WEAKBOT};
enum camera_mode {ORTHO, PERSP};

/* the acceleration factor */
#define ACCEL_FAC   1.1f

//numbers
#define NUMBER_ONE   0x30
#define NUMBER_TWO   0x5B
#define NUMBER_THREE 0x79
#define NUMBER_FOUR  0x74
#define NUMBER_FIVE  0x6D
#define NUMBER_SIX   0x6F
#define NUMBER_SEVEN 0x38
#define NUMBER_EIGHT 0x7F
#define NUMBER_NINE  0x5F
#define NUMBER_ZERO  0x3F

// }}}


/*****************************************************************************/
/**************************** CAMERA *****************************************/
/*****************************************************************************/

/*
 * global camera struct
 */
struct CGCamera { // {{{1
    CGMatrix4x4 projectionMatrix, viewMatrix;
    struct {
        CGVec4 n;
        float d;
    } clip_plane[2];

    camera_mode mode;
} ourCamera; // }}}

/*
 * method for setting the camera transformation
 */
CGMatrix4x4 cguLookAt(float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ) //{{{1
{
    // A4 a)
    CGMatrix4x4 V;
    float f[3];

    f[0]=centerX - eyeX;
    f[1]=centerY - eyeY;
    f[2]=centerZ - eyeZ;
    float fn=sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);

    for(int i=0; i<3; i++)
        f[i]=f[i]/fn;

    float s[3];
    s[0]=f[1]*upZ - f[2]*upY;
    s[1]=f[2]*upX - f[0]*upZ;
    s[2]=f[0]*upY - f[1]*upX;
    float sn= sqrt(s[0]*s[0]+s[1]*s[1]+s[2]*s[2]);
    for(int i=0; i<3; i++)
        s[i]=s[i]/sn;
    float u[3];

    u[0]=s[1]*f[2] - s[2]*f[1];
    u[1]=s[2]*f[0] - s[0]*f[2];
    u[2]=s[0]*f[1] - s[1]*f[0];

    float matrixr[16];
    matrixr[0]=s[0];matrixr[1]=s[1];matrixr[2]=s[2];matrixr[3]=0;
    matrixr[4]=u[0];matrixr[5]=u[1];matrixr[6]=u[2];matrixr[7]=0;
    matrixr[8]=-f[0];matrixr[9]=-f[1];matrixr[10]=-f[2];matrixr[11]=0;
    matrixr[12]=0;matrixr[13]=0;matrixr[14]=0;matrixr[15]=1;

    V.setFloatsFromRowMajor(matrixr);
    V=V*CGMatrix4x4::getTranslationMatrix(-eyeX,-eyeY,-eyeZ);

    return V;
} //}}}

/*
 * set perspective matrix
 */
CGMatrix4x4 cguPerspective(float fov_y, float aspect, float zNear, float zFar) // {{{1
{
    // ZA6 a)
    CGMatrix4x4 P;
    return P;
} // }}}

/*
 * TODO: find out what this fkt does
 */
void setCamera() //{{{1
{
    CGVec4 p_dev[6]={
        {{-1.0, -1.0, -1.0, 1.0}}, // left bottom near
        {{-1.0, -1.0, 1.0, 1.0}}, // left bottom far
        {{-1.0, 1.0, -1.0, 1.0}}, // left top near
        {{ 1.0, -1.0, -1.0, 1.0}}, // right bottom near
        {{ 1.0, -1.0, 1.0, 1.0}}, // right bottom far
        {{ 1.0, 1.0, -1.0, 1.0}}
        }; // right top near
        CGVec4 p_eye[6];
        // Set up PROJECTION matrix (very narrow field of view this time).
        ourCamera.projectionMatrix = CGMatrix4x4::getFrustum(-0.1f, 0.1f, -0.1f, 0.1f, 1.0f, 50.0f);
        // A2b) inverse Projektionsmatrix berechnen, p_dev nach p_eye umrechnen
        CGMatrix4x4 tempmatrix= ourCamera.projectionMatrix;
        tempmatrix.invert();
        for(int i=0;i<6;i++){
            p_eye[i]= tempmatrix * p_dev[i];
            CGMath::divw(p_eye[i]);
        }
        // linke Clip Plane ourCamera.clip_plane[0] aus p_eye[0] bis p_eye[2] berechnen
        ourCamera.clip_plane[0].n = CGMath::cross(CGMath::normalize(CGMath::sub(p_eye[0],p_eye[1])),CGMath::normalize(CGMath::sub(p_eye[0],p_eye[2])));
        ourCamera.clip_plane[0].d = CGMath::dot( ourCamera.clip_plane[0].n, p_eye[0]);
        // rechte Clip Plane ourCamera.clip_plane[1] aus p_eye[3] bis p_eye[5] berechnen
        ourCamera.clip_plane[1].n = CGMath::cross(CGMath::normalize(CGMath::sub(p_eye[3],p_eye[4])),CGMath::normalize(CGMath::sub(p_eye[3],p_eye[5])));
        ourCamera.clip_plane[0].d = CGMath::dot( ourCamera.clip_plane[1].n, p_eye[3]);


        float proj[16]; ourCamera.projectionMatrix.getFloatsToColMajor(proj);
        ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX,1,false,proj);
        // Set up (MODEL)VIEW matrix.
        static float ang = 0.0;
        if (anim_cam) ang+=0.003;
        ourCamera.viewMatrix = cguLookAt(0,1,20, cos(ang)*10.0f,1,0, 0,1,0);
} // }}}


/*****************************************************************************/
/**************************** HELPER *****************************************/
/*****************************************************************************/

/*
 * get geometry data for a quad
 */ 
void help_get_quad_geo(float x, float y, float* floatValues) // {{{1
{
    // {-10,0,-10, 10,0,10, 10,0,-10, -10,0,-10, -10,0,10, 10,0,10,};
    floatValues[0]  = 0  ; floatValues[1]  = 0  ; floatValues[2] = 0  ;
    floatValues[3]  = x   ; floatValues[4] = 0  ; floatValues[5] = 0   ;
    floatValues[6]  = x   ; floatValues[7] = y  ; floatValues[8] = 0  ;
    floatValues[9]  = 0  ; floatValues[10] = 0 ; floatValues[11] = 0 ;
    floatValues[12] = x ; floatValues[13]  = y ; floatValues[14] = 0  ;
    floatValues[15] = 0  ; floatValues[16] = y ; floatValues[17] = 0  ;
}; // }}}

/*
 * simple draw helper
 */
void help_draw(float* vertArray, float* colorArray, int count, CGMatrix4x4& trans) // {{{1
{
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertArray);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colorArray);
    float floatValues[16];
    trans.getFloatsToColMajor(floatValues);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,floatValues);
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, count);
} // }}}

/*
 * helper to render a quadric
 */
void renderQuadric(CGQuadric &quadric) //{{{1
{
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE,quadric.getPositionArray());
    ourContext->cgVertexAttribPointer(CG_NORMAL_ATTRIBUTE, quadric.getNormalArray());
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, quadric.getColorArray());
    ourContext->cgVertexAttribPointer(CG_TEXCOORD_ATTRIBUTE,quadric.getTexCoordArray());
    ourContext->cgDrawArrays(GL_TRIANGLES,0,quadric.getVertexCount());
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, NULL);
    ourContext->cgVertexAttribPointer(CG_NORMAL_ATTRIBUTE, NULL);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, NULL);
    ourContext->cgVertexAttribPointer(CG_TEXCOORD_ATTRIBUTE, NULL);
}; //}}}

/*
 * desc
 */
void help_draw1(CGMatrix4x4 viewT) // {{{1
{
    viewT = viewT * CGMatrix4x4::getScaleMatrix(0.3f, 0.2, 2.0);
    float mv[16];
    viewT.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    CGQuadric vline;
    vline.createBox();
    renderQuadric(vline);
} // }}}

/*
 * desc
 */
void help_print_keys() // {{{1
{
    printf("\n\nYou can use the following keys:\n");
    printf("===============================\n\n");
    printf("Player 2:\n");
    printf("Toggle AI      --> q\n");
    printf("Move Up        --> e\n");
    printf("Move Down      --> d\n\n");
    printf("Player 1:\n");
    printf("Toggle AI      --> -\n");
    printf("Move Up        --> UP\n");
    printf("Move Down      --> DOWN\n\n");
    printf("General:\n");
    printf("Toggle Camera  --> t\n");
    printf("Toggle Light   --> l\n");
    printf("Start Puk      --> SPACE\n");
} // }}}

/* function to draw specified segment
 *
 * do this by using a bitmask. Each bit represents one segment.
 * e.g. 00000011 draws the bottom and bottom left segment
 */ 
void draw_segment_number(int bitMask, float startX, float startY) // {{{1
{
    int line_lenght = 4;
    
    float vertex[2][3];
    float color[2][4];

    for (int i = 0; i < 2; ++i)
    {
        color[i][0] = 1.0f;
        color[i][1] = 1.0f;
        color[i][2] = 1.0f;
        color[i][3] = 1.0f;
    }


    // bottom
    if (bitMask & 1)
    {
        vertex[0][0]= startX + 1.0f;
        vertex[0][1]= startY;
        vertex[0][2]=0.0f;
        vertex[1][0]= startX + 1.0f + line_lenght - 1;
        vertex[1][1]= startY;
        vertex[1][2]=0.0f;
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
        ourContext->cgDrawArrays(CG_LINES, 0, 2);
    }

    // bottom left
    if (bitMask & 2)
    {
        vertex[0][0]= startX;
        vertex[0][1]= startY + 1.0f;
        vertex[0][2]=0.0f;
        vertex[1][0]= startX;
        vertex[1][1]= startY + 1.0f + line_lenght - 1;
        vertex[1][2]=0.0f;
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
        ourContext->cgDrawArrays(CG_LINES, 0, 2);
    }

    // top left
    if (bitMask & 4)
    {
        vertex[0][0]= startX;
        vertex[0][1]= startY + 2.0f + line_lenght;
        vertex[0][2]=0.0f;
        vertex[1][0]= startX;
        vertex[1][1]= startY + 2.0f + 2 * line_lenght - 1;
        vertex[1][2]=0.0f;
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
        ourContext->cgDrawArrays(CG_LINES, 0, 2);
    }

    // top
    if (bitMask & 8)
    {
        vertex[0][0]= startX + 1.0f;
        vertex[0][1]= startY + 2.0f + (2 * line_lenght);
        vertex[0][2]=0.0f;
        vertex[1][0]= startX + 1.0f + line_lenght - 1;
        vertex[1][1]= startY + 2.0f + (2 * line_lenght);
        vertex[1][2]=0.0f;
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
        ourContext->cgDrawArrays(CG_LINES, 0, 2);
    }

    // top right
    if (bitMask & 16)
    {
        vertex[0][0]= startX + 1.0f + line_lenght;
        vertex[0][1]= startY + 2.0f + line_lenght;
        vertex[0][2]=0.0f;
        vertex[1][0]= startX + 1.0f + line_lenght;
        vertex[1][1]= startY + 2.0f + 2 * line_lenght - 1;
        vertex[1][2]=0.0f;
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
        ourContext->cgDrawArrays(CG_LINES, 0, 2);
    }

    // bottom right
    if (bitMask & 32)
    {
        vertex[0][0]= startX + 1.0f + line_lenght;
        vertex[0][1]= startY + 1.0f;
        vertex[0][2]=0.0f;
        vertex[1][0]= startX + 1.0f + line_lenght;
        vertex[1][1]= startY + 1.0f + line_lenght - 1;
        vertex[1][2]=0.0f;
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
        ourContext->cgDrawArrays(CG_LINES, 0, 2);
    }

    // mid
    if (bitMask & 64)
    {
        vertex[0][0]= startX + 1.0f;
        vertex[0][1]= startY + 1.0f + line_lenght;
        vertex[0][2]=0.0f;
        vertex[1][0]= startX + 1.0f + line_lenght - 1;
        vertex[1][1]= startY + 1.0f + line_lenght;
        vertex[1][2]=0.0f;
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
        ourContext->cgDrawArrays(CG_LINES, 0, 2);
    }

} // }}}

/*
 * draw a segment number for the given integer
 */
void draw_number(int number) //{{{1
{
    /* Todo check correctness */
    switch(number)
    {
        case 1: draw_segment_number(NUMBER_ONE, 0, 0); break;
        case 2: draw_segment_number(NUMBER_TWO, 0, 0); break;
        case 3: draw_segment_number(NUMBER_THREE, 0, 0); break;
        case 4: draw_segment_number(NUMBER_FOUR, 0, 0); break;
        case 5: draw_segment_number(NUMBER_FIVE, 0, 0); break;
        case 6: draw_segment_number(NUMBER_SIX, 0, 0); break;
        case 7: draw_segment_number(NUMBER_SEVEN, 0, 0); break;
        case 8: draw_segment_number(NUMBER_EIGHT, 0, 0); break;
        case 9: draw_segment_number(NUMBER_NINE, 0, 0); break;
        case 0: draw_segment_number(NUMBER_ZERO, 0, 0); break;
    }
} //}}}

/*****************************************************************************/
/**************************** PLAY_GROUND ************************************/
/*****************************************************************************/

/*
 * main structure for the playground
 * declaration and definition
 */
struct play_ground_t // {{{1
{
    float dimX;
    float dimZ;
    float baseColor[4];
    float borderColor[4];
    float vertices[6*3];
    float colors[6*4];

    CGQuadric shape;
    CGQuadric borderShape;
};
play_ground_t playGround;
// }}}

/*
 * init the playground and calculate the vertex and 
 * color arrays
 */
void play_ground_init(play_ground_t& pg, float dimX, float dimZ, float* baseColor, float* borderColor) //{{{1
{
    pg.dimX = dimX;
    pg.dimZ = dimZ;

    float hx = dimX / 2;
    float hz = dimZ / 2;
    
    // {-10,0,-10, 10,0,10, 10,0,-10, -10,0,-10, -10,0,10, 10,0,10,};
    // TODO use helper function
    pg.vertices[0]  = -hx  ; pg.vertices[1]  = 0  ; pg.vertices[2] = -hz  ;
    pg.vertices[3]  = hx   ; pg.vertices[4]  = 0  ; pg.vertices[5] = hz   ;
    pg.vertices[6]  = hx   ; pg.vertices[7]  = 0  ; pg.vertices[8] = -hz  ;
    pg.vertices[9]  = -hx  ; pg.vertices[10] = 0 ; pg.vertices[11] = -hz ;
    pg.vertices[12] = -hx ; pg.vertices[13]  = 0 ; pg.vertices[14] = hz  ;
    pg.vertices[15] = hx  ; pg.vertices[16]  = 0 ; pg.vertices[17] = hz  ;
    
    for (int os = 0; os < 6; os++)
    {
        pg.colors[(os * 4) + 0] = baseColor[0];
        pg.colors[(os * 4) + 1] = baseColor[1];
        pg.colors[(os * 4) + 2] = baseColor[2];
        pg.colors[(os * 4) + 3] = baseColor[3];
    }
    pg.shape.setStandardColor(baseColor[0], baseColor[1], baseColor[2]);
    pg.shape.createBox(20, 2, 20);

    pg.borderShape.setStandardColor(borderColor[0], borderColor[1], borderColor[2]);
    pg.borderShape.createBox(20, 2, 4);

    /* set border color */
    for (int i = 0; i < 4; i++)
    {
        pg.borderColor[i] = borderColor[i];
    }
}; // }}}

/* 
 * draw playground with borders
 */
void play_ground_draw(play_ground_t& pg, CGMatrix4x4 viewT) //{{{1
{
    /* draw the ground */
    CGMatrix4x4 mvcopy = viewT;
    mvcopy = mvcopy * CGMatrix4x4::getTranslationMatrix(0, -1, 0);
    mvcopy = mvcopy * CGMatrix4x4::getScaleMatrix(10, 1, 10);
    float mv[16];
    mvcopy.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    renderQuadric(pg.shape);
    
    
    /* draw borders */
    mvcopy = viewT *  CGMatrix4x4::getTranslationMatrix(0, 0, (-pg.dimZ / 2) - 1);
    mvcopy = mvcopy * CGMatrix4x4::getScaleMatrix(10, 2, 1);
    mvcopy.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    renderQuadric(pg.borderShape);

    mvcopy = viewT *  CGMatrix4x4::getTranslationMatrix(0, 0, (pg.dimZ / 2) + 1);
    mvcopy = mvcopy * CGMatrix4x4::getScaleMatrix(10, 2, 1);
    mvcopy.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    renderQuadric(pg.borderShape);
}; //}}}


/*****************************************************************************/
/**************************** PUK ********************************************/
/*****************************************************************************/

struct puk_t // {{{1
{
    float posX;
    float posZ;

    float dirX;
    float dirZ;

    float dirXOld;
    float dirZOld;

    float radius;

    float baseColor[4];
    
    CGQuadric shape;
};
puk_t puk;
// }}}

/*
 * initialize
 */
void puk_init(puk_t& p, float x, float z, float dx, float dz, float* baseColor) // {{{1
{
    p.posX = x;
    p.posZ = z;
    p.dirX = dx;
    p.dirZ = dz;
    p.dirXOld = dx;
    p.dirZOld = dz;

    p.radius = 0.5;

    p.shape.setStandardColor(baseColor[0], baseColor[1], baseColor[2]);
    p.shape.createDisk(10, 2);
    
}; // }}}

/*
 * reset the puk
 */
void puk_reset(puk_t& p, float x, float z, float dx, float dz) // {{{1
{
    p.posX = x;
    p.posZ = z;
    p.dirX = dx;
    p.dirZ = dz;
} // }}}

/*
 * move the puk
 * that means calculate the new position of the puk in
 * relation to the movement vector
 */
void puk_move(puk_t& p) // {{{1
{
    p.posX = p.posX + p.dirX;
    p.posZ = p.posZ + p.dirZ;
}; // }}}

/*
 * speed up the given puk
 */
void puk_speedup(puk_t& p, float factor) // {{{1
{
    p.dirX = factor * p.dirX;
    p.dirZ = factor * p.dirZ;
} // }}}

/*
 * desc
 */
bool puk_in_start_position(puk_t& p) // {{{1
{
    if (p.dirZ == 0 && p.dirZOld == 0 && p.dirX == 0 && p.dirXOld == 0)
        return true;
    
    return false;
} // }}}
/*
 * make transformation and render the puk
 */
void puk_draw(puk_t& p, CGMatrix4x4 viewT) // {{{1
{
    viewT = viewT * CGMatrix4x4::getTranslationMatrix(p.posX, 0.5, p.posZ);
    viewT = viewT * CGMatrix4x4::getScaleMatrix(p.radius, p.radius, p.radius);
    viewT = viewT * CGMatrix4x4::getRotationMatrixX(-90);
    float mv[16];
    viewT.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    renderQuadric(p.shape);
} // }}}

/*
 * wrapper to satisfy the needs 
 */
void initBall(bool side) // {{{1
{
    srand (time(NULL));
    float puk_color[4] = {0.9f, 0.2f, 0.2f, 1.0f};
    if (side == true)
        puk_init(puk, -8, 0, 0, 0, puk_color);
    else
        puk_init(puk, 8, 0, 0, 0, puk_color);
} // }}}


/*****************************************************************************/
/**************************** PADDLE ****************************************/
/*****************************************************************************/

struct paddle_t // {{{1
{
    
    /* left buttom coord when you look from the top */
    float posX;
    float posZ;

    float xScale;
    float zScale;

    float baseColor[4];
    
    intel control;

    int points;
    
    CGQuadric shape;
};
// }}}
/* declare two players */
paddle_t player1;
paddle_t player2;

/*
 * initialize
 */
void paddle_init(paddle_t& p, float x, float z, float* baseColor, intel control) // {{{1
{
    p.posX = x;
    p.posZ = z;

    p.points = 0;

    p.control = control;

    p.xScale = 0.5f;
    p.zScale = 3.0f;

    p.shape.setStandardColor(baseColor[0], baseColor[1], baseColor[2]);
    p.shape.createBox(4,4,4);
    
}; // }}}

/*
 * reset a paddle
 */
void paddle_reset(paddle_t& p, float x, float z) // {{{1
{
    p.posX = x;
    p.posZ = z;
}; // }}}

/*
 * move the paddle
 * only z-axis movement is allowed here
 */
void paddle_move(paddle_t& p, float dz) // {{{1
{
    /* wall constraint */
    float new_pos = p.posZ + dz;
    if (new_pos < 0)
        new_pos *= -1;
    
    if (new_pos + (p.zScale) > 10)
        return;

    p.posZ = p.posZ + dz;
}; // }}}

/*
 * make transformation and render the paddle
 */
void paddle_draw(paddle_t& p, CGMatrix4x4 viewT) // {{{1
{
    viewT = viewT * CGMatrix4x4::getTranslationMatrix(p.posX, 0.0, p.posZ);
    viewT = viewT * CGMatrix4x4::getScaleMatrix(p.xScale, 1.0f, p.zScale);
    float mv[16];
    viewT.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    renderQuadric(p.shape);
} // }}}

/*
 * toggle player control
 */
void paddle_toggle_control(paddle_t& p) // {{{1
{
    if (p.control == HUMAN)
        p.control = WEAKBOT;
    else if (p.control == MIGHTYBOT)
        p.control = HUMAN;
    else if (p.control == WEAKBOT)
        p.control = HUMAN;
} // }}}



/*****************************************************************************/
/**************************** BOX ********************************************/
/*****************************************************************************/

struct box_t // {{{1
{
    
    /* left buttom coord when you look from the top */
    float posX;
    float posZ;

    float xScale;
    float zScale;

    float baseColor[4];

    CGQuadric shape;
};
// }}}
void box_init(box_t& p, float x, float z, float* baseColor) // {{{1
{
    p.posX = x;
    p.posZ = z;

    p.xScale = 1.0f;
    p.zScale = 2.0f;

    p.shape.setStandardColor(baseColor[0], baseColor[1], baseColor[2]);
    p.shape.createBox(4,4,4);
    
}; // }}}
void box_move(box_t& p, float dz) // {{{1
{
    /* wall constraint */
    float new_pos = p.posZ + dz;
    if (new_pos < 0)
        new_pos *= -1;
    
    if (new_pos + (p.zScale) > 10)
        return;

    p.posZ = p.posZ + dz;
}; // }}}
void box_draw(box_t& p, CGMatrix4x4 viewT) // {{{1
{
    viewT = viewT * CGMatrix4x4::getTranslationMatrix(p.posX, 0.5, p.posZ);
    viewT = viewT * CGMatrix4x4::getScaleMatrix(p.xScale, 1.0f, p.zScale);
    float mv[16];
    viewT.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    renderQuadric(p.shape);
} // }}}
box_t obstacle;

/*****************************************************************************/
/**************************** SIMULATION LOOP ********************************/
/*****************************************************************************/

/*
 * process all input and make changes directly
 */
void processUserInput() // {{{1
{
    if (puk_in_start_position(puk))
    {
        if (CG1Helper::isKeyReleased(' '))
        {
            printf("Starting the Puk!\n");
            puk.dirX = (float)(rand() % 10 + 1.0f) / 100.0f;
            puk.dirZ = (float)(rand() % 10 + 1.0f) / 100.0f;
            printf("Speed X: %f  Speed Z: %f\n", puk.dirX, puk.dirZ);
        }
    }
    if (CG1Helper::isKeyReleased(GLUT_KEY_F1))
        help_print_keys();

    /* paddle control player1 */
    if (player1.control == HUMAN)
    {
        if (CG1Helper::isKeyPressed(CG_KEY_UP))
            paddle_move(player1, -0.3);
        if (CG1Helper::isKeyPressed(CG_KEY_DOWN))
            paddle_move(player1, 0.3);
    }
    if (CG1Helper::isKeyReleased('-'))
        paddle_toggle_control(player1);


    /* paddle control player2 */
    if (player2.control == HUMAN)
    {
        if (CG1Helper::isKeyPressed('e'))
            paddle_move(player2, -0.3);
        if (CG1Helper::isKeyPressed('d'))
            paddle_move(player2, 0.3);
    }
    if (CG1Helper::isKeyReleased('q'))
        paddle_toggle_control(player2);

    /* cam toggle */
    if (CG1Helper::isKeyReleased('t'))
    {
        if (ourCamera.mode == PERSP)
            ourCamera.mode = ORTHO;
        else
            ourCamera.mode = PERSP;
    }
    /* cam movement */
    if (CG1Helper::isKeyPressed(CG_KEY_RIGHT))
        cam_anim -= 0.01;
    if (CG1Helper::isKeyPressed(CG_KEY_LEFT))
        cam_anim += 0.01;

    /* light toggle */
    if (CG1Helper::isKeyReleased('l'))
    {
        if (use_light == true)
        {
            use_light = false;
        }
        else
        {
            use_light = true;
        }
    }
} // }}}

/*
 * process ai movement
 */
void processAI(paddle_t& p) // {{{1
{
    if (p.control == WEAKBOT)
    {
        if (p.posZ < puk.posZ)
            paddle_move(p, 0.3f);
        else
            paddle_move(p, -0.3f);
    }
    else if (p.control == MIGHTYBOT)
    {
            p.posZ = puk.posZ;
    }
} // }}}

/*
 * desc
 */
void stop_game(sides s) // {{{1
{
    /* side is the edge-side where the puk was going out */
    if (s == RIGHT)
    {
        player2.points++;
        initBall(false);
    }
    else if (s == LEFT)
    {
        player1.points++;
        initBall(true);
    }
    paddle_reset(player1, 10, 0);
    paddle_reset(player2, -10, 0);
    printf("Score: %i : %i\n",player2.points, player1.points);
    
    printf("\n");
} // }}}

/*
 * process physics
 */
void processPhysics() // {{{1
{
    //check X
    //nicht nur Rand links und rechts prüfen, auch ob paddle an der richtigen stelle
    // TODO: dont use fix values
    sides side;
    
    /* save old speed */
    float speed = sqrt(puk.dirX * puk.dirX + puk.dirZ * puk.dirZ);

    /* check if someone scored */
    if (puk.posX + puk.dirX >= 10 - puk.radius || puk.posX + puk.dirX <= puk.radius - 10)
    {
        /* check for left or right */
        if (puk.dirX > 0)
        {
            side = RIGHT;
        }
        else
        {
            side = LEFT;
        }

        /* mirror vectors */
        if( side == RIGHT )
        {
            /* check for collusion with paddle */
            if ((puk.posZ < player1.posZ + (player1.zScale)) && (puk.posZ > player1.posZ - (player1.zScale)))
            {
                float fac = 1.0f;
                
                /* ball from top */
                if (puk.dirZ > 0)
                {
                    /* paddle top corner */
                    float dis = (player1.posZ - (player1.zScale / 2)) - puk.posZ;
                    fac = 2*dis / player1.zScale;
                    /* force possitive */
                    fac = (fac < 0 ? -fac : fac);
                }
                else
                {
                    /* paddle buttom corner */
                    float dis = (player1.posZ + (player1.zScale / 2)) - puk.posZ;
                    fac = 2*dis / player1.zScale;
                    /* force possitive */
                    fac = (fac < 0 ? -fac : fac);
                }
                puk.dirX = - puk.dirX;
                puk.dirZ = fac * puk.dirZ;

                // norm the vector
                float amount = sqrt(puk.dirX * puk.dirX + puk.dirZ * puk.dirZ);
                puk.dirX = puk.dirX / amount;
                puk.dirZ = puk.dirZ / amount;
                
                // add original speed
                puk.dirX = puk.dirX * speed;
                puk.dirZ = puk.dirZ * speed;
                puk_speedup(puk, ACCEL_FAC);
            }
            else
            {
                stop_game(RIGHT);
            }
        }
        else
        {
            if (puk.posZ < player2.posZ + (player2.zScale) && puk.posZ > player2.posZ - (player2.zScale))
            {
                float fac = 1.0f;
                
                /* ball from top */
                if (puk.dirZ > 0)
                {
                    /* paddle top corner */
                    float dis = (player2.posZ - (player2.zScale / 2)) - puk.posZ;
                    fac = 2*dis / player2.zScale;
                    /* force possitive */
                    fac = (fac < 0 ? -fac : fac);
                }
                else
                {
                    /* paddle buttom corner */
                    float dis = (player2.posZ + (player2.zScale / 2)) - puk.posZ;
                    fac = 2*dis / player2.zScale;
                    /* force possitive */
                    fac = (fac < 0 ? -fac : fac);
                }
                puk.dirX = - puk.dirX;
                puk.dirZ = fac * puk.dirZ;

                // norm the vector
                float amount = sqrt(puk.dirX * puk.dirX + puk.dirZ * puk.dirZ);
                puk.dirX = puk.dirX / amount;
                puk.dirZ = puk.dirZ / amount;
                
                // add original speed
                puk.dirX = puk.dirX * speed;
                puk.dirZ = puk.dirZ * speed;
                
                /* speed up */
                puk_speedup(puk, ACCEL_FAC);
            }
            else
            {
                stop_game(LEFT);
            }
        }
    }

    /* check Z aka the walls */
    if (puk.posZ + puk.dirZ >= 10 - puk.radius || puk.posZ + puk.dirZ <= puk.radius - 10)
    {
        puk.dirZ = - (puk.dirZ);
    }
    
    /* check the obstacle collusion */
    /* check if puk is in the box , add the puk radius here*/
    float p_new_xpos = puk.posX + puk.dirX;
    float p_new_zpos = puk.posZ + puk.dirZ;

    //if (p_new_xpos < 0)
    //    p_new_xpos -= puk.radius;
    //else
    //    p_new_xpos += puk.radius;

    //if (p_new_zpos < 0)
    //    p_new_zpos -= puk.radius;
    //else
    //    p_new_zpos += puk.radius;

    /* bounds */
    float xmax_bound = obstacle.posX + (obstacle.xScale / 2) + puk.radius;
    float xmin_bound = obstacle.posX - (obstacle.xScale / 2) - puk.radius;
    float zmax_bound = obstacle.posZ + (obstacle.zScale / 2) + puk.radius;
    float zmin_bound = obstacle.posZ - (obstacle.zScale / 2) - puk.radius;
    
    //printf("%f, %f, %f, %f\n", xmax_bound, xmin_bound, zmax_bound, zmin_bound);
    
    if (p_new_xpos <= xmax_bound && p_new_xpos >= xmin_bound &&
        p_new_zpos <= zmax_bound && p_new_zpos >= zmin_bound)
    {
        if (puk.posZ < zmin_bound || puk.posZ > zmax_bound)
            puk.dirZ = -puk.dirZ;
        else
            puk.dirX = -puk.dirX;
    }
        


} // }}}

/*
 * drawing 
 */
void drawFrame() // {{{1
{
    ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT|CG_DEPTH_BUFFER_BIT);
    ourContext->cgEnable(CG_DEPTH_TEST);
    ourContext->cgEnable(CG_CULL_FACE);
    ourContext->cgEnable(CG_USE_BRESENHAM);
    ourContext->cgEnable(CG_BLEND);
    ourContext->cgEnable(CG_COLOR_MATERIAL);
    ourContext->cgDisable(CG_PHONG_SHADING);

    if (use_light)
        ourContext->cgEnable(CG_LIGHTING);
    else
        ourContext->cgDisable(CG_LIGHTING);

    //CGMatrix4x4 projMat=CGMatrix4x4::getFrustum(-0.062132f, 0.062132f, -0.041421f, 0.041421f, 0.1f, 50.0f);

    CGLightSource lightSource[3];
    // Light 1 will be a spot light using the animated position as direction.
    
    CGMatrix4x4 viewT;
    CGMatrix4x4 projMat;

    if (ourCamera.mode == PERSP)
    {
        // Camera rotating around center on r=15 circle.
        projMat=CGMatrix4x4::getFrustum(-0.062132f, 0.062132f, -0.041421f, 0.041421f, 0.1f, 50.0f);
        float proj[16]; projMat.getFloatsToColMajor(proj);
        ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX,1,false,proj);

        float eyeX = cos(cam_anim)*25.0f, eyeY = 25.0f, eyeZ = sin(cam_anim)*25.0f;
        viewT = cguLookAt(eyeX,eyeY,eyeZ, 0,-4,0, 0,1,0);
    }
    else
    {
        projMat = CGMatrix4x4::getOrtho(-18, 18, -18, 18, 0.1f, 50.0f);
        float proj[16]; projMat.getFloatsToColMajor(proj);
        ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX,1,false,proj);

        float eyeX = 0.0f, eyeY = 40.0f, eyeZ = 0.0f;
        viewT = cguLookAt(eyeX,eyeY,eyeZ, 0,0,0, 0,0,-2);
    }
    
    /* turn on the light */
    if (use_light)
    {
        float spotCutoff = 80.0f,spotExp = 1.0f,conAtt=1.0f,linAtt=0.0f,quadAtt=0.0f;
        CGVec4 spotPos;
        spotPos.set(puk.posX,5,puk.posZ,1);
        CGVec4 spotDir;
        spotDir.set(0,-5,0,0);
        lightSource[0].setColorAmbient (0.1,0.1,0.1,1.0);
        lightSource[0].setColorDiffuse (0.5,0.5,0.5,1.0);
        lightSource[0].setColorSpecular(0.5,0.5,0.5,1.0);
        lightSource[0].setPosition((viewT*spotPos).elements);
        lightSource[0].setSpotDirection((viewT*spotDir).elements);
        lightSource[0].setSpotCutoff(spotCutoff);
        lightSource[0].setSpotExponent(spotExp);

        lightSource[0].setupUniforms(ourContext);


        // Setup materials: we use CG_COLOR_MATERIAL and thus do not need to
        // set ambient and diffuse values (will be overwritten by vertex color).
        float rgbaWhite[4] = {1,1,1,1}, rgbaBlack[4] = {0,0,0,1};
        float rgbaWhite005[4] = {0.05f,0.05f,0.05f,1.0};
        float shininess = 32.0f;
        ourContext->cgUniform4fv(CG_ULOC_MATERIAL_SPECULAR,1,rgbaWhite);
        ourContext->cgUniform1fv(CG_ULOC_MATERIAL_SHININESS,1,&shininess);
        ourContext->cgUniform4fv(CG_ULOC_MATERIAL_EMISSION,1,rgbaBlack);
        // Setyp scene ambient
        ourContext->cgUniform4fv(CG_ULOC_SCENE_AMBIENT,1,rgbaWhite005);
    }



    /* draw the score left side */
    CGMatrix4x4 mvcopy = viewT;
    mvcopy = mvcopy * CGMatrix4x4::getTranslationMatrix(-17, 0, 0);
    mvcopy = mvcopy * CGMatrix4x4::getRotationMatrixX(-90);
    float mv[16];
    mvcopy.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    draw_number(player2.points);

    /* draw the score right side */
    mvcopy = viewT;
    mvcopy = mvcopy * CGMatrix4x4::getTranslationMatrix(12, 0, 0);
    mvcopy = mvcopy * CGMatrix4x4::getRotationMatrixX(-90);
    mvcopy.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    draw_number(player1.points);

    play_ground_draw(playGround, viewT);
    CGMatrix4x4 modelviewT = viewT;

    paddle_draw(player1, viewT);
    paddle_draw(player2, viewT);
    
    box_draw(obstacle, viewT);
    
    puk_move(puk);
    puk_draw(puk, viewT);

    

} // }}}

/*
 * Program Step
 *
 * This draws the playground, the ball and the pedals
 */
void programStep() // {{{1
{
    /* input */
    processUserInput();

    processPhysics();

    processAI(player1);
    processAI(player2);

    drawFrame();
} //}}}



int main(int argc, char** argv)
{
    /* init cam */
    ourCamera.mode = ORTHO;
    
    use_light = true;

    /* init playground */
    float pl_color[4] = {0.6f, 0.8f, 0.9f, 1.0f};
    float pl_border_color[4] = {0.3f, 0.8f, 0.4f, 1.0f};
    play_ground_init(playGround, 20, 20, pl_color, pl_border_color);

    /* init ball */
    //float puk_color[4] = {0.9f, 0.2f, 0.2f, 1.0f};
    //puk_init(puk, 0, 0, 0.4, 0.3, puk_color);
    //puk_init(puk, 0, 0, 0, 0.7, puk_color);
    //puk_init(puk, 0, 3, 0,0.3, puk_color);
    initBall(true);

    /* init obstacle */
    box_init(obstacle, 0, 0, pl_border_color);
    

    /* init player 1 */
    float player1_color[4] = {0.2f, 0.2f, 0.8f, 1.0f};
    paddle_init(player1, 10, 0, player1_color, HUMAN);
    paddle_init(player2, -10, 0, player1_color, WEAKBOT);


    CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);
    CG1Helper::setProgramStep(programStep);
    CG1Helper::runApplication();
    return 0;
}
