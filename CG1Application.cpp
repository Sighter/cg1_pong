// Standard includes.
#include <stdlib.h>         // for rand()
#define _USE_MATH_DEFINES
#include <math.h>

#define PI 3.14159265

// Our includes.
#include "CG1Helper.h"
#include "CGContext.h"
#include "CGMatrix.h"
#include "CGLightSource.h"
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  480
#define FRAME_HEIGHT 480 //
#define FRAME_SCALE  1   // Integer scaling factors (zoom).


CGContext *ourContext;



//---------------------------------------------------------------------------
// This is something specifically needed for this application.
typedef struct {
    float dx;
    float dy;
    float x;
    float y;
} SpringenderPunkt;
SpringenderPunkt a={1.0f, 1.0f, 20.0f, 0.0f};
SpringenderPunkt b={0.5f, -0.75f, 70.0f, 50.0f};

// Vertex Attribute Arrays.
#define VERTEX_COUNT 2
float vertex[VERTEX_COUNT][3];    // x,y,z
float color[VERTEX_COUNT][4];      // r,g,b,a

//---------------------------------------------------------------------------
void animateSpringenderPunkt(SpringenderPunkt& p)
{
    if (p.x >=  FRAME_WIDTH-1) p.dx = -fabsf(p.dx);
    if (p.x <=           0.0f) p.dx =  fabsf(p.dx);
    if (p.y >= FRAME_HEIGHT-1) p.dy = -fabsf(p.dy);
    if (p.y <=           0.0f) p.dy =  fabsf(p.dy);
    p.x+=p.dx;
    p.y+=p.dy;
}
//---------------------------------------------------------------------------
// programStep_* functions are always our main application.
void programStep_Ueb1_SpringenderPunkt()
{
    if (CG1Helper::isKeyReleased(CG_KEY_LEFT))  a.dx = -1.0f;
    if (CG1Helper::isKeyReleased(CG_KEY_RIGHT)) a.dx =  1.0f;
    if (CG1Helper::isKeyReleased(CG_KEY_UP))    a.dy =  1.0f;
    if (CG1Helper::isKeyReleased(CG_KEY_DOWN))  a.dy = -1.0f;
    animateSpringenderPunkt(a);
    animateSpringenderPunkt(b);

    // render
    // Uebung01, Aufgabe 2a):
    ourContext->cgClearColor(0.5f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT);
    ourContext->cgEnable(CG_USE_BRESENHAM);

    // prepare vertex array for point a
    color[0][0]=1.0f;
    color[0][1]=1.0f;
    color[0][2]=1.0f;
    color[0][3]=1.0f;

    vertex[0][0]=a.x;
    vertex[0][1]=a.y;
    vertex[0][2]=0.0f;

    // prepare vertex array for point b
    color[1][0]=0.0f;
    color[1][1]=1.0f;
    color[1][2]=0.0f;
    color[1][3]=1.0f;

    vertex[1][0]=b.x;
    vertex[1][1]=b.y;
    vertex[1][2]=0.0f;

    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);

    // Uebung01, Aufgabe 3b):
    ourContext->cgDrawArrays(CG_LINES, 0, 2);
}
//---------------------------------------------------------------------------
void programStep_LineBenchmark()
{
    ourContext->cgEnable(CG_USE_BRESENHAM);
    ourContext->cgClearColor(0.5f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT);


    for (int i = 0; i < 10000;i++)
    {
        // prepare vertex array for point a
        color[0][0]=1.0f;
        color[0][1]=1.0f;
        color[0][2]=1.0f;
        color[0][3]=1.0f;

        vertex[0][0]=0.0f;
        vertex[0][1]=0.0f;
        vertex[0][2]=0.0f;

        // prepare vertex array for point b
        color[1][0]=1.0f;
        color[1][1]=1.0f;
        color[1][2]=1.0f;
        color[1][3]=1.0f;

        vertex[1][0]= FRAME_WIDTH - 1.0f;
        vertex[1][1]= FRAME_HEIGHT - 1.0f;
        vertex[1][2]=0.0f;

        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);

        // Uebung01, Aufgabe 3b):
        ourContext->cgDrawArrays(CG_LINES, 0, 2);

    }

}
//---------------------------------------------------------------------------
void rotate_vertex2d(float* vertex, float deg, float midx, float midy)
{
    float x = vertex[0] - midx;
    float y = vertex[1] - midy;

    x = (cos(deg) * x) + ((- sin(deg)) * y);
    y = (sin(deg) * x) + ((cos(deg)) * y);

    vertex[0] = x + midx;
    vertex[1] = y + midy;
}
//---------------------------------------------------------------------------
void programStep_LineAllSlopes()
{
    ourContext->cgEnable(CG_USE_BRESENHAM);
    ourContext->cgClearColor(0.5f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT);


    // color is every time the same
    color[0][0]=1.0f;
    color[0][1]=1.0f;
    color[0][2]=1.0f;
    color[0][3]=1.0f;

    color[1][0]=1.0f;
    color[1][1]=1.0f;
    color[1][2]=1.0f;
    color[1][3]=1.0f;

    // start vertex is every time the same
    vertex[0][0] = FRAME_WIDTH / 2.0f;
    vertex[0][1] = FRAME_HEIGHT / 2.0f;
    vertex[0][2] = 0.0f;

    // first line horizontal
    vertex[1][0] = vertex[0][0] + FRAME_WIDTH / 3.0f;
    vertex[1][1] = vertex[0][1];
    vertex[1][2] = 0.0f;
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
    ourContext->cgDrawArrays(CG_LINES, 0, 2);

    for (int i = 1; i < 7; i++)
    {
        rotate_vertex2d(vertex[1], (PI/8), FRAME_WIDTH / 2.0f, FRAME_HEIGHT / 2.0f);
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
        ourContext->cgDrawArrays(CG_LINES, 0, 2);
    }
}
//---------------------------------------------------------------------------
// programStep to test bresenham interpolation:
// Hint: set the resolution to 41x41 for this test.
float vertexPosition_TestBresenham[32*3]={
    20,30,0, 20,40,0, 24,29,0, 28,38,0, 27,27,0, 34,34,0, 29,24,0, 38,28,0,
    30,20,0, 40,20,0, 29,16,0, 38,12,0, 27,13,0, 34,6,0, 24,11,0, 28,2,0,
    20,10,0, 20,0,0, 16,11,0, 12,2,0, 13,13,0, 6,6,0, 11,16,0, 2,12,0,
    10,20,0, 0,20,0, 11,24,0, 2,28,0, 13,27,0, 6,34,0, 16,29,0, 12,38,0
};
float vertexColor_TestBresenham[32*4]={
    1,0,0,1, 1,1,1,1, 0,0,1,1, 1,1,0,1, 0,1,0,1, 1,0,1,1, 0,1,1,1, 1,0,0,1,
    1,0,0,1, 0,1,1,1, 1,0,1,1, 0,1,0,1, 1,1,0,1, 0,0,1,1, 1,1,1,1, 1,0,0,1,
    1,0,0,1, 0,0,1,1, 0,0,1,1, 0,1,0,1, 0,1,0,1, 0,1,1,1, 0,1,1,1, 1,0,0,1,
    1,0,0,1, 1,0,1,1, 1,0,1,1, 1,1,0,1, 1,1,0,1, 1,1,1,1, 1,1,1,1, 1,0,0,1
};
void programStep_TestBresenham()
{
    // clear
    ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT);
    // set capabilities and vertex pointers
    ourContext->cgEnable(CG_USE_BRESENHAM);
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertexPosition_TestBresenham);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, vertexColor_TestBresenham);
    // render
    ourContext->cgDrawArrays(CG_LINES, 0, 32); // 32 vertices for 16 lines.
}
//---------------------------------------------------------------------------
void programStep_TestTriangle()
{
    // clear
    ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT);
    // construct vertex data (*should* not happen each frame)
    float vertexPosition_TestTriangle[3*3]={0,0,0, 41-1,0,0, 41/2,41-1,0};
    float vertexColor_TestTriangle[3*4] ={1,0,0,1, 0,0,1,1, 0,1,0,1};
    // and set vertex pointers
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertexPosition_TestTriangle);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, vertexColor_TestTriangle);
    // render
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, 3); // 3 vertices for 1 triangle.
}
//---------------------------------------------------------------------------
void programStep_TestRotatingTriangle()
{
    // clear
    ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT);
    // Construct vertex data (*should* not happend each frame)
    static float a=0.0; a+=0.01;
    float ca = cos(a), sa = sin(a);
    float vertexPosition_TestRotTriangle[3*3]={20,20,0, ca*30+20,sa*30+20,0, -sa*20+20,ca*20+20,0};
    float vertexColor_TestRotTriangle[3*4]
    ={1,0,0,1, 0,0,1,1, 0,1,0,1};
    // and set vertex pointers
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertexPosition_TestRotTriangle);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE,
        vertexColor_TestRotTriangle);
    // render
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, 3); // 3 vertices for 1 triangle.
}
//---------------------------------------------------------------------------
void programStep_TestBFCandZTest()
{
    ourContext->cgEnable(CG_CULL_FACE);
    ourContext->cgEnable(CG_DEPTH_TEST);
    // Set the resolution of the context to 500 x 300!
    // clear
    ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT/*|CG_DEPTH_BUFFER_BIT*/);
    static float a=0.0; a+=0.01;
    float ca = cos(a), sa = sin(a);
    float PTS[24] = {-ca-sa*1.28,-0.6,sa-ca*1.28, ca-sa*1.28,-0.6,-sa-ca*1.28, ca-sa*0.6,1.28,-sa-ca*0.6,
        -ca-sa*0.6,1.28,sa-ca*0.6, -ca+sa*0.6,-1.28,sa+ca*0.6, ca+sa*0.6,-1.28,-sa+ca*0.6,
        ca+sa*1.28,0.6,-sa+ca*1.28, -ca+sa*1.28,0.6,sa+ca*1.28};
#define R3 1,0,0,1, 1,0,0,1, 1,0,0,1
#define G3 0,1,0,1, 0,1,0,1, 0,1,0,1
#define B3 0,0,1,1, 0,0,1,1, 0,0,1,1
#define C3 0,1,1,1, 0,1,1,1, 0,1,1,1
#define M3 1,0,1,1, 1,0,1,1, 1,0,1,1
#define Y3 1,1,0,1, 1,1,0,1, 1,1,0,1
#define P(i) PTS[3*i+0]*60+100,PTS[3*i+1]*50+200,PTS[3*i+2]*0.1
    // construct vertex data
    float vertices4triangles[54*3] = {0,0,-1.1, 50,0,-1.1, 0,50,-1.1, 50,0,+1.1, 50,50,+1.1, 0,50,+1.1,
        300,0,-0.5, 480,0,-0.5, 300,100,-0.5, 300,0,-0.5, 480,0,-0.5, 480,100,-0.5,
        250,180,0.5, 500,240,-0.5, 250,240,0.5, 300,280,0, 350,150,0, 400,260,0,
        P(0),P(1),P(2),P(0),P(2),P(3),P(1),P(5),P(6),P(1),P(6),P(2),P(3),P(2),P(6),P(3),P(6),P(7),
        P(0),P(5),P(1),P(0),P(4),P(5),P(0),P(3),P(7),P(0),P(7),P(4),P(4),P(7),P(6),P(4),P(6),P(5)};
    float colors4triangles[54*4] = {B3, B3, G3, R3, C3, Y3, G3, G3, R3, R3, B3, B3, Y3, Y3, C3, C3, M3, M3};
#undef P
#undef R3
#undef B3
#undef B3
#undef C3
#undef M3
#undef Y3
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4triangles);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4triangles);
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, 54);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void programStep_TestBlendingZSort()
{
    static bool anim_blend=false, anim_circle=true;
    if (CG1Helper::isKeyReleased('b')) anim_blend=!anim_blend;
    if (CG1Helper::isKeyReleased('c')) anim_circle=!anim_circle;
    const int N = 20; // We define a set of N triangles for a resolution of 500x300
    static float b=asin(0.4), c=0.0; b+=(anim_blend)?0.01:0.0; c+=(anim_circle)?0.01:0.0;
    float vertices[N*3*3], colors[N*3*4]; // again, don’t do this locally as we do!

    for(int i=0; i<N; i++) {
        float rgba[4]={(i+1)&0x01?1:0,(i+1)&0x02?1:0,(i+2)&0x04?1:0, sin(b)*0.5+0.5};
        float pos[3] = {i*10,i*10,-i*0.01};
        if(i>=10) { // all triangles >= 10 will be rotating ones.
            float a2 = c+float(i-10)/float(N-10)*6.283;
            pos[X]=300+sin(a2)*100.0; pos[Y]=100-cos(a2)*50.0; pos[Z]=cos(a2)*0.9;
        }
        for(int j=0; j<3; j++) memcpy(colors+12*i+4*j,rgba,sizeof(float)*4);
        for(int j=0; j<3; j++) memcpy(vertices+9*i+3*j,pos,sizeof(float)*3);
        vertices[9*i+6+X]+=25.0;
        vertices[9*i+3+X]+=50.0; vertices[9*i+6+Y]+=75.0;
    }

    // The index array describes the order of the triangles.
    // [0,2,1] would mean to render the first, third and then second triangle.
    int indexArray[N];
    for(int i=0; i<N; i++) indexArray[i]=i; // initialize with identity [0,1,2...]
    // *** Insert your code for sorting the index array here *** //
    // *** Sort by the z-component of the first corner of the *** //
    // *** i-th triangle (e.g. float zi = vertices[3*3*i+Z]). *** //

    // i=0 1 2 3 4 5
    //   8 4 2 7 9 1
    bool swap=true;int temp;
    while(swap)
    {
        swap=false;
        for(int i=0; i<N-1;i++)
            if(vertices[9*indexArray[i]+2]<vertices[9*(indexArray[i+1])+2])
            {
                temp=indexArray[i];
                indexArray[i]=indexArray[i+1];
                indexArray[i+1]=temp;
                swap=true;
            }
    }

    ourContext->cgClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT|CG_DEPTH_BUFFER_BIT);
    ourContext->cgEnable(CG_DEPTH_TEST);
    ourContext->cgEnable(CG_CULL_FACE);
    ourContext->cgEnable(CG_BLEND);
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors);
    // We use the index array by passing the correct vertex offset for each triangle.
    for(int i=0; i<N; i++)
        ourContext->cgDrawArrays(CG_TRIANGLES, 3*indexArray[i], 3);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void programStep_AwesomeTriangle()
{
    const int N = 20;
    static float a=0.0; a+=0.01;
    if (a >= 0.5) a = 0.0;
    float vertices[3*3*N], colors[3*4*N]; // again, don’t do this locally as we do!
    vertices[3*0+X] = 0; vertices[3*0+Y] = 0; vertices[3*0+Z] = 0;
    colors[4*0+R] = 1.0f; colors[4*0+G] = 0; colors[4*0+B] = 0; colors[4*0+A] = 0.2f;
    vertices[3*1+X] = 500; vertices[3*1+Y] = 0; vertices[3*1+Z] = 0;
    colors[4*1+R] = 0.0f; colors[4*1+G] = 1.0f; colors[4*1+B] = 0; colors[4*1+A] = 0.2f;
    vertices[3*2+X] = 250; vertices[3*2+Y] = 433; vertices[3*2+Z] = 0;
    colors[4*2+R] = 0.0f; colors[4*2+G] = 0; colors[4*2+B] = 1.0f; colors[4*2+A] = 0.2f;
    float tempVertices[3*3];
    float tempColors[3*4];
    for (int i = 0; i < 19; i++)
    {
        memcpy(tempColors,&colors[3*4*i],3*4*sizeof(float));
        tempVertices[3*0+X] = a*vertices[3*3*i+3*1+X]+(1-a)*vertices[3*3*i+3*0+X];
        tempVertices[3*0+Y] = a*vertices[3*3*i+3*1+Y]+(1-a)*vertices[3*3*i+3*0+Y];
        tempVertices[3*0+Z] = a*vertices[3*3*i+3*1+Z]+(1-a)*vertices[3*3*i+3*0+Z];
        tempVertices[3*1+X] = a*vertices[3*3*i+3*2+X]+(1-a)*vertices[3*3*i+3*1+X];
        tempVertices[3*1+Y] = a*vertices[3*3*i+3*2+Y]+(1-a)*vertices[3*3*i+3*1+Y];
        tempVertices[3*1+Z] = a*vertices[3*3*i+3*2+Z]+(1-a)*vertices[3*3*i+3*1+Z];
        tempVertices[3*2+X] = a*vertices[3*3*i+3*0+X]+(1-a)*vertices[3*3*i+3*2+X];
        tempVertices[3*2+Y] = a*vertices[3*3*i+3*0+Y]+(1-a)*vertices[3*3*i+3*2+Y];
        tempVertices[3*2+Z] = a*vertices[3*3*i+3*0+Z]+(1-a)*vertices[3*3*i+3*2+Z];
        memcpy(&vertices[3*3*(i+1)],tempVertices,3*3*sizeof(float));
        memcpy(&colors[3*4*(i+1)],tempColors,3*4*sizeof(float));
    }
    ourContext->cgClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT|CG_DEPTH_BUFFER_BIT);
    //ourContext->cgEnable(CG_DEPTH_TEST);
    //ourContext->cgEnable(CG_CULL_FACE);
    ourContext->cgEnable(CG_BLEND);
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors);
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, 20*3);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void programStep_ProjectionTest()
{
    // Create a unit-cube which is later (manually) transformed.
    // Hint: this is just a hack as long as we have no model transformation,
    // i. e. do not attempt this at home!
    float vertices4cube[12*3*3]={0,0,0,1,1,0,1,0,0,0,0,0,0,1,0,1,1,0, // FRONT
        0,0,1,1,0,1,1,1,1,0,0,1,1,1,1,0,1,1, // BACK
        0,0,1,0,1,0,0,0,0,0,0,1,0,1,1,0,1,0, // LEFT
        1,0,1,1,0,0,1,1,0,1,0,1,1,1,0,1,1,1, // RIGHT
        0,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,1,0, // TOP
        0,0,1,1,0,0,1,0,1,0,0,1,0,0,0,1,0,0}; // BOTTOM
    float colors4cube[12*3*4];

    for(int i=0; i<36; i++) {
        // compute colors from temporary vertex positions:
        colors4cube[4*i+0]=vertices4cube[3*i+0];
        colors4cube[4*i+1]=vertices4cube[3*i+1];
        colors4cube[4*i+2]=vertices4cube[3*i+2];
        colors4cube[4*i+3]=1.0f;
        // compute (manually!) transformed vertex positions:
        vertices4cube[3*i+0]=vertices4cube[3*i+0]*0.5f+0.5f;
        vertices4cube[3*i+1]=vertices4cube[3*i+1]*0.5f-1.0f;
        vertices4cube[3*i+2]=vertices4cube[3*i+2]*0.5f-2.0f;
    }
    // ground plane
    CGMatrix4x4 projMat;
    float f=1.1;

    projMat=CGMatrix4x4::getFrustum(-0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 2.0f);
    float proj[16]; projMat.getFloatsToColMajor(proj);

    ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX,1,false,proj);
    ourContext->cgClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT|CG_DEPTH_BUFFER_BIT);
    ourContext->cgEnable(CG_DEPTH_TEST);
    ourContext->cgEnable(CG_CULL_FACE); // !
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4cube);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4cube);
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, 12*3);

    // Plus: draw a ground plane:
    float vertices4ground[6*3] = {-1,-1,-1,1,-1,-1,1,-1,-3,-1,-1,-1,1,-1,-3,-1,-1,-3};
    float colors4ground[6*4];
    for(int i=6;i--;) { float* c=colors4ground+4*i; c[0]=c[1]=c[2]=0.8f; c[3]=1.0f; }
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4ground);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4ground);
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, 2*3);
}
//---------------------------------------------------------------------------


float vertices4trunk[3*3] = {-0.2,0,0.1, 0.2,0,0.1, 0,2,0};
float colors4trunk[3*4] = {0.5,0.25,0,1, 0.5,0.25,0,1, 0.5,0.25,0,1};
float vertices4leafs[3*3] = {0,0,0, 2,0,0.2, 0,2,0.2};
float colors4leafs[3*4] = {1,1,1,1, 0,0.8,0,1, 0,0.5,0,1};
float vertices4ground[6*3]= {-10,0,-10, 10,0,10, 10,0,-10, -10,0,-10, -10,0,10, 10,0,10,};
float colors4ground[6*4]= {0,1,0,1, 1,1,1,1, 0,1,0,1, 0,1,0,1, 1,1,1,1, 1,1,1,1};
//---------------------------------------------------------------------------
void drawGround(CGMatrix4x4 viewT)
{
    // GROUND
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4ground);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4ground);
    float floatValues[16]; viewT.getFloatsToColMajor(floatValues);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,floatValues);
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, 6);
}
//---------------------------------------------------------------------------
void drawTree(CGMatrix4x4 transform)
{
    CGMatrix4x4 Ttrunk, Tleaf1, Tleaf2, Tleaf3;
    float floatValues[16];
    // ZA5 a) // mehrere Layer davon zeichnen
    // A3 )
    Ttrunk=transform*Ttrunk;
    //Tleaf1=
    Tleaf1 = transform*CGMatrix4x4::getTranslationMatrix(0,3.5,0) * CGMatrix4x4::getRotationMatrixZ(225) * CGMatrix4x4::getScaleMatrix(0.5,0.5,0.5);
    //Tleaf2=
    Tleaf2=transform* CGMatrix4x4::getTranslationMatrix(0,3.0,0)*CGMatrix4x4::getRotationMatrixZ(225)* CGMatrix4x4::getScaleMatrix(0.8,0.8,0.8);
    //Tleaf3=
    Tleaf3=transform*CGMatrix4x4::getTranslationMatrix(0,2.5,0)*CGMatrix4x4::getRotationMatrixZ(225)*Tleaf3;

    // TRUNK
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4trunk);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4trunk);
    Ttrunk.getFloatsToColMajor(floatValues);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,floatValues);
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, 3);

    // LEAVES
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4leafs);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4leafs);

    for(int i=3; i--;) {
        if(i==0)
            Tleaf1.getFloatsToColMajor(floatValues);
        else if (i==1)
            Tleaf2.getFloatsToColMajor(floatValues);
        else
            Tleaf3.getFloatsToColMajor(floatValues);

        ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,floatValues);
        ourContext->cgDrawArrays(CG_TRIANGLES, 0, 3);
    }

}
//---------------------------------------------------------------------------
CGMatrix4x4 cguLookAt(float eyeX, float eyeY, float eyeZ,
    float centerX, float centerY, float centerZ,
    float upX, float upY, float upZ)
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
}
//---------------------------------------------------------------------------
CGMatrix4x4 cguPerspective(float fov_y, float aspect, float zNear, float zFar)
{
    // ZA6 a)
    CGMatrix4x4 P;
    return P;
}
//---------------------------------------------------------------------------
void programStep_HappyHolidays()
{
    ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT|CG_DEPTH_BUFFER_BIT);
    ourContext->cgEnable(CG_DEPTH_TEST);
    ourContext->cgEnable(CG_CULL_FACE);
    CGMatrix4x4 projMat=CGMatrix4x4::getFrustum(-0.062132f, 0.062132f, -0.041421f, 0.041421f,
        0.1f, 50.0f);
    float proj[16]; projMat.getFloatsToColMajor(proj);
    ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX,1,false,proj);
    // A4 b)
    //Camera rotating around center on r=15 circle.
    static float anim = 0.0; anim+=0.01;
    float eyeX = cos(anim)*15.0f, eyeY = 15.0f, eyeZ = sin(anim)*15.0f;
    CGMatrix4x4 viewT = cguLookAt(eyeX,eyeY,eyeZ, 0,2,0, 0,1,0);

    //CGMatrix4x4 viewT = CGMatrix4x4::getTranslationMatrix(0.0f,-5.0,-25.0f);
    drawGround(viewT);
    for(int i=10; i--; ) {
        float prX = float(i%7)/6.0f*16.0f-8.0f, // [0,6]->[-8,+8]
            prZ = float(i%4)/3.0f*16.0f-8.0f; // [0,3]->[-8,+8]
        CGMatrix4x4 treeT = viewT * CGMatrix4x4::getTranslationMatrix(prX,0,prZ);
        drawTree(treeT);
    }
}
/*
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

CG1Helper::setProgramStep(programStep_HappyHolidays);

CG1Helper::runApplication();

return 0;
}
//---------------------------------------------------------------------------
*/

#include <stdlib.h> // for rand() and srand()
#include <time.h> // for time() in srand()
#include "deerVertexData.h" // vertex data is defined here
#include "CG1Application_renderSphere.h"
#include "CGMatrix.h"
#include "CGMath.h"


//---------------------------------------------------------------------------
// Structure for a deer object with a position and (bounding sphere) radius.
struct CGDeer {
    CGVec4 pos;
    float *positions, *colors, radius;
    int num_vertices;
    CGDeer() {
        // Assemble deer (include deerVertexData.h!)
        num_vertices = NV;
        positions = (float*)malloc(sizeof(float)*NV*3);
        colors = (float*)malloc(sizeof(float)*NV*4);
        for(int i=NV; i--;) {
            positions[3*i+0]=vertices4deer[3*i+0]*0.02-0.3;
            positions[3*i+1]=vertices4deer[3*i+1]*0.02-1.0;
            positions[3*i+2]=vertices4deer[3*i+2]*0.02-0.7;
        }
        float r1=(float)rand()/RAND_MAX,r2=(float)rand()/RAND_MAX;
        pos[X]=r1*20-10; pos[Y]=1.0; pos[Z]=r2*20-10; pos[W]=1.0f;
        for(int i=NV; i--;) {
            colors[4*i+0]=colors4deer[i]*0.1*r1;
            colors[4*i+1]=colors4deer[i]*0.1*r2;
            colors[4*i+2]=colors4deer[i]*0.1*(1.0-r1);
            colors[4*i+3]=0.5;
        }
        float radtemp;
        radius=0;
        // A2a) Radius berechnen
        for(int i=0; i< NV;i++){
            radtemp = sqrt(positions[3*i]*positions[3*i]+positions[3*i+1]*positions[3*i+1] +positions[3*i+2]*positions[3*i+2]);
            if(radtemp>radius)
                radius=radtemp;
        }
    }
    ~CGDeer() {free(positions); free(colors); }
};
//---------------------------------------------------------------------------
// Global herd.
const int NDEER = 20;
CGDeer *deer = 0;
int view_sphere=0; //0: no sphere, 1: 2D circle, 2: 3D sphere
bool anim_cam=true;
//---------------------------------------------------------------------------
void drawCGDeer(CGMatrix4x4 transform, CGDeer &deer) {
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, deer.positions);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, deer.colors);
    CGMatrix4x4 T = CGMatrix4x4::getTranslationMatrix(deer.pos[0],deer.pos[1],deer.pos[2]);
    float mv[16]; (transform*T).getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, deer.num_vertices);
    // Draw bounding sphere
    if (view_sphere) {
        ourContext->cgEnable(CG_BLEND); //blending on
        if (view_sphere == 1)
            drawTestSphere2D(ourContext, transform*T, deer.radius);
        else if (view_sphere == 2)
            renderTestSphere(ourContext, transform*T, deer.radius);
        ourContext->cgDisable(CG_BLEND); //blending off (default)
    }
}
//---------------------------------------------------------------------------
struct CGCamera {
    CGMatrix4x4 projectionMatrix, viewMatrix;
    struct {
        CGVec4 n;
        float d;
    } clip_plane[2];
} ourCamera;
//---------------------------------------------------------------------------
void setCamera() {
    CGVec4 p_dev[6]={
        {{-1.0, -1.0, -1.0, 1.0}}, // left bottom near
        {{-1.0, -1.0, 1.0, 1.0}}, // left bottom far
        {{-1.0, 1.0, -1.0, 1.0}}, // left top near
        {{ 1.0, -1.0, -1.0, 1.0}}, // right bottom near
        {{ 1.0, -1.0, 1.0, 1.0}}, // right bottom far
        {{ 1.0, 1.0, -1.0, 1.0}}}; // right top near
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
}
//---------------------------------------------------------------------------
bool isSphereInFrustum(CGVec4 posES, float radius)
{
    // A2c) Testen, ob Bounding Sphere im Frustum liegt
    if(CGMath::dot(ourCamera.clip_plane[0].n,posES)-ourCamera.clip_plane[0].d-radius >0)
        return false;
    if(CGMath::dot(ourCamera.clip_plane[1].n,posES)-ourCamera.clip_plane[1].d-radius >0)
        return false;
    return true;
}
//---------------------------------------------------------------------------
void programStep_FrustumCulling()
{
    if (CG1Helper::isKeyReleased('s')) view_sphere=(view_sphere+1)%3;
    if (CG1Helper::isKeyReleased('a')) anim_cam = !anim_cam;
    ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT|CG_DEPTH_BUFFER_BIT);
    ourContext->cgEnable(CG_DEPTH_TEST);
    ourContext->cgEnable(CG_CULL_FACE);
    ourContext->cgDisable(CG_BLEND);
    ourContext->cgEnable(CG_USE_BRESENHAM);
    setCamera();
    int visible=0;
    for(int i=0; i<NDEER; i++) {
        // Transform the (object space) position of the deer into eye space.
        // A2d) Eye Space position des Mittelpunkts der Sphere bestimmen
        CGVec4 posES = ourCamera.viewMatrix * deer[i].pos;

        // A2d) nur zeichnen, wenn nicht ausserhalb
        if(isSphereInFrustum(posES,deer[i].radius)){
            drawCGDeer(ourCamera.viewMatrix, deer[i]);
            visible++;
        }
    }
    printf("visible: %d: culled: %d\n",visible,NDEER-visible);
}
//---------------------------------------------------------------------------
/*int main(int argc, char** argv)
{
srand(time(0)); //init random seed
deer = new CGDeer[NDEER]; //create herd
createTestSphere(3); //create bounding sphere vertices
createTestSphere2D(60); //create bounding circle vertices
CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);

CG1Helper::setProgramStep(programStep_FrustumCulling);
CG1Helper::runApplication();
delete[] deer;
return 0;
}*/
//---------------------------------------------------------------------------

#include "CGQuadric.h"
//---------------------------------------------------------------------------
// Light and material properties, which will later be passed as uniforms.
float rgbaWhite10[4] = {1,1,1,1},
    rgbaWhite01[4] = {0.1,0.1,0.1,1.0},
    rgbaWhite05[4] = {0.5,0.5,0.5,1.0},
    rgbaGreen[4] = {0,1,0,1.0},
    rgbaRed[4] = {1,0,0,1},
    rgbaBlack[4] = {0,0,0,1};
float shininess = 32.0f;
//---------------------------------------------------------------------------

void renderQuadric(CGQuadric &quadric)
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
}


void programStep_Lighting()
{
    // toglle phong-shading and gouraud-shading
    if (CG1Helper::isKeyReleased('p'))
        ourContext->cgEnable(CG_PHONG_SHADING);
    if (CG1Helper::isKeyReleased('g'))
        ourContext->cgDisable(CG_PHONG_SHADING);

    ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT|CG_DEPTH_BUFFER_BIT);
    ourContext->cgEnable(CG_DEPTH_TEST);
    ourContext->cgEnable(CG_CULL_FACE);
    ourContext->cgEnable(CG_LIGHTING);
    ourContext->cgEnable(CG_COLOR_MATERIAL);
    // set LIGHT properties (uniforms) with
    // - low (white) ambient intensity (global diffuse lighting)
    // - medium (white) diffuse and specular intensity
    ourContext->cgUniform4fv(CG_ULOC_LIGHT0_AMBIENT,1,rgbaWhite01);
    ourContext->cgUniform4fv(CG_ULOC_LIGHT0_DIFFUSE,1,rgbaWhite05);
    ourContext->cgUniform4fv(CG_ULOC_LIGHT0_SPECULAR,1,rgbaWhite05);
    // set MATERIAL properties (uniforms) with
    // - (red only) ambient and diffuse reflectance
    // - (all channels) specular reflectance plus shininess
    // - disable emission
    ourContext->cgUniform4fv(CG_ULOC_MATERIAL_AMBIENT,1,rgbaRed); // TRY: enabling
    ourContext->cgUniform4fv(CG_ULOC_MATERIAL_DIFFUSE,1,rgbaRed); // CG_COLOR_MATERIAL
    ourContext->cgUniform4fv(CG_ULOC_MATERIAL_SPECULAR,1,rgbaWhite10);// TRY: rgbaGreen
    ourContext->cgUniform1fv(CG_ULOC_MATERIAL_SHININESS,1, &shininess);
    ourContext->cgUniform4fv(CG_ULOC_MATERIAL_EMISSION,1,rgbaBlack); // TRY: rgbaWhite01
    // set projection matrix
    float proj[16];
    CGMatrix4x4::getFrustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 50.0f).getFloatsToColMajor(proj);
    ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX,1,false,proj);
    // set modelview matrix and normal matrix
    float mv[16], nm[16];
    CGMatrix4x4 viewT = cguLookAt(0,2,2.5, 0,0,0, 0,1,0);
    CGMatrix4x4 modelT = CGMatrix4x4::getRotationMatrixY(40);
    CGMatrix4x4 modelviewT = viewT*modelT;
    modelviewT.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    modelviewT.getFloatsToColMajor(nm); // Get normal matrix (column major!).
    nm[12] = nm[13] = nm[14] = 0.0f; // Reduce to 3x3 matrix (column major!).
    nm[3] = nm[7] = nm[11] = 0.0f;
    nm[15] = 1.0f;
    CGMatrix4x4 normalMatrix; normalMatrix.setFloatsFromColMajor(nm);
    normalMatrix.invert(); normalMatrix.transpose();
    normalMatrix.getFloatsToColMajor(nm); // Get the correct values of (MV.3x3)^-1^T
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,nm);
    renderTestSphere(ourContext,modelviewT,1.0f);
    // Specify light in Eye Space here
    static float anim=0.0f; anim+=0.01f;
    CGVec4 lightPos; lightPos.set(cos(anim)*1.4,1.4,sin(anim)*1.4,1);
    CGVec4 lightPosES = modelviewT*lightPos;
    ourContext->cgUniform4fv(CG_ULOC_LIGHT0_POSITION,1,lightPosES.elements);
    /// Visualize the light source smaller sphere
    modelviewT=modelviewT*CGMatrix4x4::getTranslationMatrix(lightPos[0], lightPos[1], lightPos[2]);
    modelviewT.getFloatsToColMajor(mv);
    ourContext->cgUniform4fv(CG_ULOC_MATERIAL_AMBIENT,1,rgbaBlack);
    ourContext->cgUniform4fv(CG_ULOC_MATERIAL_DIFFUSE,1,rgbaBlack);
    ourContext->cgUniform4fv(CG_ULOC_MATERIAL_SPECULAR,1,rgbaBlack);
    ourContext->cgUniform4fv(CG_ULOC_MATERIAL_EMISSION,1,rgbaWhite10);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    renderTestSphere(ourContext,modelviewT,0.05f);
}
//---------------------------------------------------------------------------
// Define all global values which are changed by the user input.
int currentLight = 0;
float spotCutoff = 20.0f,spotExp = 30.0f,conAtt=1.0f,linAtt=0.0f,quadAtt=0.0f;
CGQuadric base, cube, sphere, spot, spotDisk;
//---------------------------------------------------------------------------
void programStep_LightingHomework()
{
    ourContext->cgClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    ourContext->cgClear(CG_COLOR_BUFFER_BIT|CG_DEPTH_BUFFER_BIT);
    ourContext->cgEnable(CG_DEPTH_TEST);
    ourContext->cgEnable(CG_CULL_FACE);
    ourContext->cgEnable(CG_BLEND);
    ourContext->cgEnable(CG_LIGHTING);
    ourContext->cgEnable(CG_COLOR_MATERIAL);
    // Set camera with view (no model yet) transformation and projection matrix.
    CGMatrix4x4 projMat=CGMatrix4x4::getFrustum(-0.5f,0.5f,-0.5f,0.5f,1.0f,50.0f);
    float proj[16]; projMat.getFloatsToColMajor(proj);
    ourContext->cgUniformMatrix4fv(CG_ULOC_PROJECTION_MATRIX,1,false,proj);
    CGMatrix4x4 viewT = cguLookAt(10,10,0, 0,0,0, 0,1,0);
    // We need MV later to transform the lights into eyespace.
    CGMatrix4x4 modelviewT = viewT;
    // HANDLE INPUT
    if (CG1Helper::isKeyPressed('s')) spotCutoff --;
    if (CG1Helper::isKeyPressed('S')) spotCutoff ++;
    if (CG1Helper::isKeyPressed('d')) spotExp --;
    if (CG1Helper::isKeyPressed('D')) spotExp ++;
    if (CG1Helper::isKeyPressed('y')) conAtt -= 0.01;
    if (CG1Helper::isKeyPressed('Y')) conAtt += 0.01;
    if (CG1Helper::isKeyPressed('x')) linAtt -= 0.01;
    if (CG1Helper::isKeyPressed('X')) linAtt += 0.01;
    if (CG1Helper::isKeyPressed('c')) quadAtt -= 0.01;
    if (CG1Helper::isKeyPressed('C')) quadAtt += 0.01;
    if (CG1Helper::isKeyReleased('l')) currentLight = (currentLight+1)%3;

    // toglle phong-shading and gouraud-shading
    if (CG1Helper::isKeyReleased('p'))
        ourContext->cgEnable(CG_PHONG_SHADING);
    if (CG1Helper::isKeyReleased('g'))
        ourContext->cgDisable(CG_PHONG_SHADING);
    // LIGHT SETUP
    // Animation parameter anim (again, this is not the way to do animation
    // since it is framerate dependent. Better use a time dependency!)
    static float anim = 0.0; anim+=0.01;
    // We define 3 light sources from which only one will be used
    // (except if you implement the extra credit task).
    CGLightSource lightSource[3];
    // Light 0 will be a point light with attenuation.
    CGVec4 lightPos; lightPos.set(cos(anim)*5,3,sin(anim)*5,1);
    lightSource[0].setColorAmbient (0.1,0.1,0.1,1.0);
    lightSource[0].setColorDiffuse (0.5,0.5,0.5,1.0);
    lightSource[0].setColorSpecular(0.5,0.5,0.5,1.0);
    lightSource[0].setPosition((modelviewT*lightPos).elements);
    lightSource[0].setConstantAttenuation(conAtt);
    lightSource[0].setLinearAttenuation(linAtt);
    lightSource[0].setQuadraticAttenuation(quadAtt);
    // Light 1 will be a spot light using the animated position as direction.
    CGVec4 spotPos; spotPos.set(-5,3,-5,1);
    CGVec4 spotDir = CGMath::sub(lightPos,spotPos);
    spotDir[Y]=-3; // direct downwards
    lightSource[1].setColorAmbient (0.1,0.1,0.1,1.0);
    lightSource[1].setColorDiffuse (0.5,0.5,0.5,1.0);
    lightSource[1].setColorSpecular(0.5,0.5,0.5,1.0);
    lightSource[1].setPosition((modelviewT*spotPos).elements);
    lightSource[1].setSpotDirection((modelviewT*spotDir).elements);
    lightSource[1].setSpotCutoff(spotCutoff);
    lightSource[1].setSpotExponent(spotExp);
    // Light 2 will be a directional light.
    lightPos[W] = 0.0f; // Use the position as vector.
    lightSource[2].setColorAmbient (0.1,0.1,0.1,1.0);
    lightSource[2].setColorDiffuse (0.5,0.5,0.5,1.0);
    lightSource[2].setColorSpecular(0.5,0.5,0.5,1.0);
    lightSource[2].setPosition((modelviewT*lightPos).elements);
    lightPos[W] = 1.0f; // Restore the position variable.
    // Set only the current light source.
    lightSource[currentLight].setupUniforms(ourContext);
    // RENDER OBJECTS.
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
    // Setup and render three different objects with their model transformations.
    modelviewT = viewT;
    modelviewT=modelviewT*CGMatrix4x4::getScaleMatrix(10,10,10);
    modelviewT=modelviewT*CGMatrix4x4::getRotationMatrixX(-90); // equals V*S*R
    float mv[16]; modelviewT.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    // We can use the MV matrix as NM (b/c no uniform scalings, see unit 10).
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    renderQuadric(base);
    modelviewT = viewT;
    modelviewT=modelviewT*CGMatrix4x4::getTranslationMatrix(0,1,-5);
    modelviewT.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    renderQuadric(cube);
    modelviewT = viewT;
    modelviewT=modelviewT*CGMatrix4x4::getTranslationMatrix(0,1,5);
    modelviewT.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_NORMAL_MATRIX,1,false,mv);
    renderQuadric(sphere);
    // RENDER LIGHT SOURCES (without lighting).
    ourContext->cgDisable(CG_LIGHTING);
    if (currentLight == 0) {
        modelviewT = viewT;
        modelviewT=modelviewT*CGMatrix4x4::getTranslationMatrix(lightPos[X],lightPos[Y],lightPos[Z]);
        modelviewT=modelviewT*CGMatrix4x4::getScaleMatrix(0.1,0.1,0.1);
        modelviewT.getFloatsToColMajor(mv);
        ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
        // No need to pass the normal matrix *if* no normals used.
        renderQuadric(sphere);
    }
    else if (currentLight == 1) {
        // We use the cguLookAt here for a model transformation, not for the view!
        CGMatrix4x4 spotMat = cguLookAt(spotPos[X],spotPos[Y],spotPos[Z],
                lightPos[X],lightPos[Y]-3,lightPos[Z],
                0,0,1);
        spotMat.invert();
        modelviewT = viewT * spotMat;
        float scaleFactor = tan((spotCutoff*M_PI)/180);
        modelviewT=modelviewT*CGMatrix4x4::getScaleMatrix(scaleFactor,scaleFactor,1.0);
        modelviewT.getFloatsToColMajor(mv);
        ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
        renderQuadric(spot);
        modelviewT=modelviewT*CGMatrix4x4::getRotationMatrixX(180);
        modelviewT.getFloatsToColMajor(mv);
        ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
        renderQuadric(spotDisk);
    }
    else {
        // Use a single line primitive to represent light direction.
        float pos[6] = {0.0f,0.0f,0.0f, lightPos[X],lightPos[Y],lightPos[Z]};
        float col[8] = {1.0f,1.0f,1.0f,1.0f, 1.0f,1.0f,1.0f,1.0f};
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE,pos);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, col);
        ourContext->cgEnable(CG_USE_BRESENHAM); // If you still use this.
        modelviewT = viewT;
        modelviewT.getFloatsToColMajor(mv);
        ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
        ourContext->cgDrawArrays(CG_LINES,0,2);
        ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, NULL);
        ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, NULL);
    }
    ourContext->cgDisable(CG_DEPTH_TEST);
}
//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    srand(time(0));
    cube.setStandardColor(1,0,0);
    cube.createBox(5,5,5);
    //cylinder.createCylinder(15,5);
    base.setStandardColor(1,1,0);
    base.createDisk(72,40);
    sphere.setStandardColor(1,1,1);
    sphere.createIcoSphere(3);
    spot.setStandardColor(0.2,0.2,0.2);
    spot.createCone(32,1);
    spotDisk.setStandardColor(1,1,1);
    spotDisk.createDisk(32,1);
    CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE);
    CG1Helper::setProgramStep(programStep_LightingHomework);
    CG1Helper::runApplication();
    return 0;
}
