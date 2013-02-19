// Standard includes.
#include <stdlib.h>         // for rand()
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

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

//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  480
#define FRAME_HEIGHT 480 //
#define FRAME_SCALE  1   // Integer scaling factors (zoom).


// global context pointer
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


float vertices4trunk[3*3] = {-0.2,0,0.1, 0.2,0,0.1, 0,2,0};
float colors4trunk[3*4] = {0.5,0.25,0,1, 0.5,0.25,0,1, 0.5,0.25,0,1};
float vertices4leafs[3*3] = {0,0,0, 2,0,0.2, 0,2,0.2};
float colors4leafs[3*4] = {1,1,1,1, 0,0.8,0,1, 0,0.5,0,1};
float vertices4ground[6*3]= {-10,0,-10, 10,0,10, 10,0,-10, -10,0,-10, -10,0,10, 10,0,10,};
float colors4ground[6*4]= {0,1,0,1, 1,1,1,1, 0,1,0,1, 0,1,0,1, 1,1,1,1, 1,1,1,1};


/**************************** HELPER *****************************************/

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


/**************************** PLAY_GROUND ************************************/

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
    help_draw(pg.vertices, pg.colors, 6, viewT);
    
    float border_vertices[6*3];
    help_get_quad_geo(pg.dimX, 2, border_vertices);

    float border_color[6*4];
    for (int os = 0; os < 6; os++)
    {
        border_color[(os * 4) + 0] = pg.borderColor[0];
        border_color[(os * 4) + 1] = pg.borderColor[1];
        border_color[(os * 4) + 2] = pg.borderColor[2];
        border_color[(os * 4) + 3] = pg.borderColor[3];
    }

    /* transform to the border */
    ourContext->cgDisable(CG_CULL_FACE);
    viewT = viewT *  CGMatrix4x4::getTranslationMatrix(-pg.dimX / 2, 0, -pg.dimZ / 2);
    help_draw(border_vertices, border_color, 6, viewT);

    viewT = viewT *  CGMatrix4x4::getTranslationMatrix(0 , 0, pg.dimZ);
    help_draw(border_vertices, border_color, 6, viewT);

    ourContext->cgEnable(CG_CULL_FACE);
}; //}}}


/**************************** PUK ********************************************/

struct puk_t // {{{1
{
    float posX;
    float posZ;

    float dirX;
    float dirZ;

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

    p.shape.setStandardColor(baseColor[0], baseColor[1], baseColor[2]);
    p.shape.createDisk(10, 2);
    
}; // }}}

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
 * make transformation and render the puk
 */
void puk_draw(puk_t& p, CGMatrix4x4 viewT) // {{{1
{
    viewT = viewT * CGMatrix4x4::getTranslationMatrix(p.posX, 0.5, p.posZ);
    viewT = viewT * CGMatrix4x4::getScaleMatrix(0.5, 0.5, 0.5);
    viewT = viewT * CGMatrix4x4::getRotationMatrixX(-90);
    float mv[16];
    viewT.getFloatsToColMajor(mv);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,mv);
    renderQuadric(p.shape);
} // }}}










void drawGround(CGMatrix4x4 viewT) //{{{1
{
    // GROUND
    ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, vertices4ground);
    ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, colors4ground);
    float floatValues[16]; viewT.getFloatsToColMajor(floatValues);
    ourContext->cgUniformMatrix4fv(CG_ULOC_MODELVIEW_MATRIX,1,false,floatValues);
    ourContext->cgDrawArrays(CG_TRIANGLES, 0, 6);
}
/*
 * TODO use this function for obstacles
 */
void drawTree(CGMatrix4x4 transform) //{{{1
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

/*
 * method for setting the camera transformation
 */

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

/*
 * set perspective matrix
 */
CGMatrix4x4 cguPerspective(float fov_y, float aspect, float zNear, float zFar)
{
    // ZA6 a)
    CGMatrix4x4 P;
    return P;
}

/*
 * Program Step
 *
 * This draws the playground, the ball and the pedals
 */
void programStep()
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
    static float anim = 0.0;
    anim += 0.01;
    float eyeX = cos(anim)*15.0f, eyeY = 15.0f, eyeZ = sin(anim)*15.0f;
    CGMatrix4x4 viewT = cguLookAt(eyeX,eyeY,eyeZ, 0,2,0, 0,1,0);

    //CGMatrix4x4 viewT = CGMatrix4x4::getTranslationMatrix(0.0f,-5.0,-25.0f);
    // drawGround(viewT);
    play_ground_draw(playGround, viewT);

    CGMatrix4x4 modelviewT = viewT;
    
    puk_move(puk);
    puk_draw(puk, viewT);
    

    for(int i=10; i--; ) {
        float prX = float(i%7)/6.0f*16.0f-8.0f, // [0,6]->[-8,+8]
            prZ = float(i%4)/3.0f*16.0f-8.0f; // [0,3]->[-8,+8]
        CGMatrix4x4 treeT = viewT * CGMatrix4x4::getTranslationMatrix(prX,0,prZ);
        drawTree(treeT);
    }
}


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
// Light and material properties, which will later be passed as uniforms.
float rgbaWhite10[4] = {1,1,1,1},
    rgbaWhite01[4] = {0.1,0.1,0.1,1.0},
    rgbaWhite05[4] = {0.5,0.5,0.5,1.0},
    rgbaGreen[4] = {0,1,0,1.0},
    rgbaRed[4] = {1,0,0,1},
    rgbaBlack[4] = {0,0,0,1};
float shininess = 32.0f;
//---------------------------------------------------------------------------



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
    /* init our structures */
    float pl_color[4] = {0.6f, 0.8f, 0.9f, 1.0f};
    float pl_border_color[4] = {0.3f, 0.8f, 0.4f, 1.0f};
    play_ground_init(playGround, 20, 20, pl_color, pl_border_color);

    float puk_color[4] = {0.9f, 0.2f, 0.2f, 1.0f};
    puk_init(puk, 0, 0, 0.2, 0.1, puk_color);

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
    CG1Helper::setProgramStep(programStep);
    CG1Helper::runApplication();
    return 0;
}
