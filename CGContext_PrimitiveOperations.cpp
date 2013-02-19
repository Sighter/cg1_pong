#include "CGContext.h"
#include <iostream>
#include <cassert>
#include <limits>

//---------------------------------------------------------------------------
// CLIPPING
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Helper data structures and functions
//------------------------------------------------------------------------------
namespace {//anonymous
  // These constants work as an interface to the CG.h
  enum {_VERTEX_ATTR_CNT = 4, _VERTEX_VARY_CNT = CG_VARYING_COUNT};

  struct ClipBound {
    enum {Left=0, Right, Bottom, Top, Near, Far, Cnt};
    // Pre-computation
    static const int BC[Cnt];        //boundary coordinate
    static const int Sign[Cnt];      //sign of boundary
  };
  const int ClipBound::BC[Cnt]   = {0,0,1,1,2,2}; //i/2
  const int ClipBound::Sign[Cnt] = {0,1,0,1,0,1}; //i%2

  inline bool isZero(float t)
  {
    return t < 1E-5 && t > -1E-5; //float epsilon
  }

  // Check if p is inside of the clipping boundary.
  inline int isInside(unsigned int clipBound, const CGVec4 &p)
  {
    float w = p[3];
    // Note:
    // Points with negative w are behind the camera (see the 4th row of projection matrix).
    // Division by a negative w produces incorrect x, y, z in R3, because
    // they will wrap around the negative infinitive and re-appear
    // in front of the camera.
    // To avoid this, points with a negative w are immediately considered outside of Near plane.
    // This rejection does not affect further processing of the clipping (such as intersection),
    // since a point behind the camera is also behind the actual Near plane, after all.
    if (clipBound == ClipBound::Near)
      return w>0 && p[2] >= -w;
    else
    {
      const int &bc = ClipBound::BC[clipBound];  // boundary coordinate
      return ClipBound::Sign[clipBound]?         // 1: right, top, and far planes, 0: other
        (w>0? p[bc] <=  w : p[bc] >=  w) :
        (w>0? p[bc] >= -w : p[bc] <= -w);
    }
  }

  // Intersection between the extended line ab and the clipping boundary clipBound.
  // Returns true if there is an intersection, false otherwise.
  // If intersecting, the intersection is at a+t*(b-a).
  // Note t is not necessarily in [0,1].
  inline bool intersectLine(unsigned int clipBound, const CGVec4 *pa, const CGVec4 *pb, float &t)
  {
    const CGVec4 &a = *pa;
    const CGVec4 &b = *pb;
    const int &bc   = ClipBound::BC[clipBound];
    const int &sign = ClipBound::Sign[clipBound];

    float aB, bB;        //distance from a to boundary, and from b to boundary
    if (sign) {
      aB = a[3]-a[bc];
      bB = b[3]-b[bc];
    }
    else {
      aB = -a[3]-a[bc];
      bB = -b[3]-b[bc];
    }

    float ab = aB - bB;  //signed distance from a to b in boundary coord.
    if (isZero(ab))
      return false;      //parallel to boundary

    // Compute ratio
    t = aB / ab;
    return true;
  }

  // Clip the line segment a+s*(b-a), s in [0,1] against the clipping boundary clipBound. 
  // Returns true if there is an intersection, false otherwise.
  // fa and fb indicate if a and b are inside of the boundary.
  // If intersecting, t is in [0,1] and the intersection is at a+t*(b-a).
  // If not intersecting, t is not in [0,1] and 
  // fa and fb both indicate if the whole line lies in or outside of the boundary.
  inline bool intersectSeg(unsigned int clipBound, const CGVec4 *pa, const CGVec4 *pb, 
    float &t, int &fa, int &fb)
  {
    const CGVec4 &a = *pa;
    const CGVec4 &b = *pb;
    fa = isInside(clipBound, a);
    fb = isInside(clipBound, b);

    if (!fa && !fb)
      return false;     //both outside

    const int &bc   = ClipBound::BC[clipBound];
    const int &sign = ClipBound::Sign[clipBound];

    float aB, bB;       //signed distance from a to boundary, and from b to boundary
    if (sign) {
      aB = a[3]-a[bc]; 
      bB = b[3]-b[bc];
    }
    else {
      aB = -a[3]-a[bc];
      bB = -b[3]-b[bc];
    }

    float ab = aB - bB; //signed distance from a to b in boundary coord.
    if (isZero(ab)) 
      return false;     //parallel to boundary

    // Compute ratio
    t = aB / ab;  
    return t>=0.0f && t<=1.0f;
  }

  // Copy all attributes and varyings
  inline void copyVert(const CGVertexData *vin, CGVertexData *vout)
  {
    if (vin != vout && vin && vout) {
      for (int i=0; i<_VERTEX_ATTR_CNT; ++i)
        vout->vertexAttributes[i] = vin->vertexAttributes[i];
      for (int i=0; i<_VERTEX_VARY_CNT; ++i)
        vout->vertexVaryings[i] = vin->vertexVaryings[i];
    }
  }

  // Attributes and varying will be linear interpolated in clip space, v = (1-t)*va+t*vb
  // In-place operation possible (i.e, vout can be vin0 or vin1)
  inline void interpolateVert(const CGVertexData *vin0, const CGVertexData *vin1, 
    float t, CGVertexData *vout)
  {
    if (vin0 && vin1 && vout) {
      for (int i=0; i<_VERTEX_ATTR_CNT; ++i)
        for (int j=0; j<4; ++j)
          vout->vertexAttributes[i][j] = 
          (1.0f-t)*vin0->vertexAttributes[i][j] + t*vin1->vertexAttributes[i][j]; 

      for (int i=0; i<_VERTEX_VARY_CNT; ++i)
        for (int j=0; j<4; ++j)
          vout->vertexVaryings[i][j] = 
          (1.0f-t)*vin0->vertexVaryings[i][j] + t*vin1->vertexVaryings[i][j]; 
    }
  }
}//namespace anonymous


//----------------------------------------------------------------------------
// Primitive clipper works in homogeneous clip space.
//----------------------------------------------------------------------------
class PrimClipper
{
public:
  // Bit mask, each bit for one clip boundary
  static unsigned int ClipBits;

  // Pointer to output vertices.
  static const CGVertexData* outBuf() {return &sm_voutBuf[0];}

  // A point clipper, it does actually culling
  static int clipPoint(CGVertexData *vinBuf, int vinCnt);

  // A line clipper
  // Return the vertex count after clipping.
  static int clipLine(CGVertexData *vinBuf, int vinCnt)   
  {return clipLine_LB(vinBuf, vinCnt);} //use Liang-Barsky line-clipping for now

  // A polygon clipper
  // Return the vertex count after clipping.
  static int clipPoly(CGVertexData *vinBuf, int vinCnt)   
  {return clipPoly_SH(vinBuf, vinCnt);} //use Sutherland-Hodgman polygon-clipping for now

protected:
  static CGVertexData sm_voutBuf[CG_MAX_VERTICES_IN_PIPELINE];  //output buffer, it is also the internal working buffer

  // Liang-Barsky line-clipping
  static int clipLine_LB(CGVertexData *vinBuf, int vinCnt);

  // Sutherland-Hodgman polygon-clipping
  static int clipPoly_SH(CGVertexData *vinBuf, int vinCnt);

  // Sutherland-Hodgman polygon-clipping against single clipping plane
  // voutBufSize is the maximal size of the voutBuf.
  // Return the number of output vertices.
  static int clipPoly_SH_single(unsigned int clipBound, 
    const CGVertexData *vinBuf, int nvin, 
    CGVertexData *voutBuf);
};
//----------------------------------------------------------------------------
// Init static members
//----------------------------------------------------------------------------
CGVertexData PrimClipper::sm_voutBuf[CG_MAX_VERTICES_IN_PIPELINE];
unsigned int PrimClipper::ClipBits = 0xffff;
//----------------------------------------------------------------------------
// Implementation of member functions
//----------------------------------------------------------------------------
int PrimClipper::clipPoint(CGVertexData *vinBuf, int vinCnt)
{
  // In-place clipping(culling)
  CGVertexData *vin = vinBuf;
  CGVertexData *vout = vinBuf;
  int cnt = 0;

  for (int i=0; i<vinCnt; ++i, vin++) {
    bool reject = false;
    for (unsigned int clipBound=0; !reject && clipBound<6; ++clipBound)   //check against each boundaries
      if (!isInside(clipBound, vin->vertexVaryings[CG_POSITION_VARYING]))
        reject = true;

    if (reject)
      continue;

    // Not rejected
    copyVert(vin, vout++);
    cnt++;
  }

  return cnt;
}
//----------------------------------------------------------------------------
int PrimClipper::clipLine_LB(CGVertexData *vinBuf, int vinCnt)
{
  CGVertexData *voutBuf = vinBuf;      //in-place clipping
  CGVertexData *vout = voutBuf;        //current output vertex
  int cnt = 0;                         //output vertex count
  CGVertexData *vs = 0, *ve = 0;       //the line segment to be checked is always vs->ve
  const CGVec4 *ps = 0, *pe = 0;       //positions of vertices, for boundary test and intersection
  CGVertexData vsclone;                //a copy of vs, for in-place modification of vs.
  int fs, fe;                          //inside-flags

  int lineCnt = vinCnt/2;              //extra vertices are ignored, if any
  for (int i=0; i<lineCnt; ++i)
  {
    bool reject = false;              
    vs = vinBuf+2*i;
    ve = vs+1;
    ps = &vs->vertexVaryings[CG_POSITION_VARYING];
    pe = &ve->vertexVaryings[CG_POSITION_VARYING];

    // Let PE and PL be the parameter t of potentially entering and leaving lines,
    // where t in [0,1]. We can find the largest PE, t_pe, and smallest PL, t_pl.
    // To maximize the visible part of the line, the clipped line corresponds 
    // to the parameter in the range [t_pe, t_pl].
    float t, t_pe = 0.0f, t_pl = 1.0f;

    // Clip against each boundary.
    for (unsigned int clipBound=0; !reject && clipBound<6; ++clipBound) {
      bool inter = intersectSeg(clipBound, ps, pe, t, fs, fe);
      unsigned int k = (fs << 1) | fe;

      switch (k) { //intersecting
      case 0 : //Whole segment outside
        reject = true;
        break;
      case 1 : //PE
        if (inter && t>t_pe && t<=1.0f)
          t_pe = t;
        break;
      case 2 : //PL
        if (inter && t<t_pl && t>=0.0f)
          t_pl = t;
        break;
      }

      if (t_pl<t_pe) //in this case, the whole line is outside
        reject = true;
    }

    if (reject)
      continue;

    // Clone the start point, only if needed
    bool cloned = false;
    if (t_pe>0.0f && t_pl<1.0f) {              //means the start or end point will be modified, cloning needed
      copyVert(vs, &vsclone);                           
      cloned = true;
    }

    // Output vertices
    if (t_pe>0.0f)
      interpolateVert(vs, ve, t_pe, vout++);   //modify start point
    else
      copyVert(vs, vout++);                    //copy start point

    if (t_pl<1.0f)
      interpolateVert(cloned? &vsclone:vs, ve, t_pl, vout++); //modify end point
    else
      copyVert(ve, vout++);                    //copy end point

    cnt+=2;
  }

  return cnt;
}
//----------------------------------------------------------------------------
int PrimClipper::clipPoly_SH(CGVertexData *vinBuf, int vinCnt)
{
  // Clip against the Near first to avoid potential big scalars
  static const unsigned int clipOrder[ClipBound::Cnt] = {
    ClipBound::Near, ClipBound::Left, 
    ClipBound::Right, ClipBound::Bottom, 
    ClipBound::Top, ClipBound::Far
  };
  // Init out buffer
  CGVertexData *voutBuf = &sm_voutBuf[0], *tmp = 0;

  // Do clipping
  for (unsigned int i=0; i<ClipBound::Cnt; ++i) {
    vinCnt = clipPoly_SH_single(clipOrder[i], vinBuf, vinCnt, voutBuf);
    // Swap in and out buffers
    tmp = vinBuf;
    vinBuf = voutBuf;
    voutBuf = tmp;
  }

  // If some clipping has been done and the result was output to the original "vinBuf",
  // (in case of an odd swapping count), we need to copy the result back to the sm_voutBuf.
  // Note that the voutBuf is now pointing the sm_voutBuf, because of the unnecessary swap
  // after the last clipping.
  if (ClipBits && voutBuf == &sm_voutBuf[0]) 
    for (int j=0; j<vinCnt; ++j)
      copyVert(vinBuf+j, sm_voutBuf+j);

  return vinCnt;
}
//----------------------------------------------------------------------------
int PrimClipper::clipPoly_SH_single(unsigned int clipBound, 
  const CGVertexData *vinBuf, int nvin, 
  CGVertexData *voutBuf)
{
  int voutCnt = 0;
  CGVertexData *vout = voutBuf;        //current output vertex
  float t;                             //ratio for interpolation
  const CGVertexData *vs = 0, *ve = 0; //the line segment to be checked is always vs->ve
  const CGVec4 *ps = 0, *pe = 0;       //positions of vertices, for boundary test and intersection
  int fs, fe;                          //flags indicating that start and end points are inside or not

  //  Situation is encoded as start_in | end_in
  //  00 : Wholly outside visible region - save nothing
  //  01 : Enter visible region - save intersection and endpoint
  //  10 : Exit visible region - save the intersection
  //  11 : Wholly inside visible region - save endpoint
  unsigned int k = 0;                
  static const unsigned int mask = 0x0003;

  // Initialize the info for start point
  vs = vinBuf+nvin-1; //the loop begin from the (n-1)-th vertex, to the 0-th vertex
  ps = &vs->vertexVaryings[CG_POSITION_VARYING];
  fs = isInside(clipBound, *ps);
  k |= (fs << 1);     //construct the code

  for (int j=0; j<nvin; ++j) {
    // Get the info for the endpoint
    ve = vinBuf+j;
    pe = &ve->vertexVaryings[CG_POSITION_VARYING];
    fe = isInside(clipBound, *pe);
    k |= fe;   //add the new right-most bit

    switch (k&mask) {
    case 1 : //Entering visible region
      {
        intersectLine(clipBound, ps, pe, t);
        interpolateVert(vs, ve, t, vout++);
        copyVert(ve, vout++);
        voutCnt+=2;
        break;
      }
    case 2 : //Exiting visible region
      {
        intersectLine(clipBound, ps, pe, t);
        interpolateVert(vs, ve, t, vout++);
        voutCnt++;
        break;
      }
    case 3 : //Whole segment inside
      {
        copyVert(ve, vout++);
        voutCnt++;
        break;
      }
    }

    // The end point becomes the new start point
    vs = ve;
    ps = pe;
    fs = fe;
    k <<= 1;  //shifting the right-most bit
  }

  return voutCnt;
}
//---------------------------------------------------------------------------
// CGCONTEXT (PRIMITIVE OPERATIONS)
//---------------------------------------------------------------------------
int CGContext::m_cgClipPrimitive()
{
  // take all vertices from m_pipelineVertices and clip (in clip space)
  // overwrite with new vertices. Could also handle QUADS and POLYGONS here!
  int newPrimCount = 1;
  switch(m_primitiveMode) {
  case CG_POINTS:
    m_pipelineVerticesCount =
      PrimClipper::clipPoint(m_pipelineVertices, m_pipelineVerticesCount);
    newPrimCount = m_pipelineVerticesCount;
    break;
  case CG_LINES:
    m_pipelineVerticesCount =
      PrimClipper::clipLine(m_pipelineVertices, m_pipelineVerticesCount);
    newPrimCount = m_pipelineVerticesCount/2;
    break;
  case CG_TRIANGLES:
    // Clipped vertices need to be re-assembled into triangles
    newPrimCount = PrimClipper::clipPoly(m_pipelineVertices, m_pipelineVerticesCount)-2;
    m_pipelineVerticesCount = 0; //reset the pipeline
    for (int i=0; i<newPrimCount; ++i) { //as triangle fan
      copyVert(PrimClipper::outBuf()+0, m_pipelineVertices+m_pipelineVerticesCount++);
      copyVert(PrimClipper::outBuf()+i+1, m_pipelineVertices+m_pipelineVerticesCount++);
      copyVert(PrimClipper::outBuf()+i+2, m_pipelineVertices+m_pipelineVerticesCount++);
    }
    break;
    // Insert other primitive types here.
  }
  // Returns the new primitive count
  return newPrimCount;
}
//---------------------------------------------------------------------------
// CGCONTEXT RASTERIZERS
//---------------------------------------------------------------------------
bool CGContext::m_cgBFCullTriangle(CGVec4 &vertex0Position, 
                                   CGVec4 &vertex1Position, 
                                   CGVec4 &vertex2Position)
{
  CGVec4 v1,v2, normal;
  v1[X] = vertex1Position[X]-vertex0Position[X];
  v1[Y] = vertex1Position[Y]-vertex0Position[Y];
  v1[Z] = vertex1Position[Z]-vertex0Position[Z];

  v2[X] = vertex2Position[X]-vertex0Position[X];
  v2[Y] = vertex2Position[Y]-vertex0Position[Y];
  v2[Z] = vertex2Position[Z]-vertex0Position[Z];

  normal[X] = v1[Y]*v2[Z]-v1[Z]*v2[Y];
  normal[Y] = v1[Z]*v2[X]-v1[X]*v2[Z];
  normal[Z] = v1[X]*v2[Y]-v1[Y]*v2[X];

  // View vector v = (0,0,+1) (until we have projections).
  // Cull if <n,-v> > 0.
  //float dotProd = normal[X]*0+normal[Y]*0+normal[Z]*(-1);
  float dotProd = -normal[Z];
  if (dotProd > 0) 
    return true; // Do cull.

  return false; // Not culled.
}
//---------------------------------------------------------------------------
// CGCONTEXT RASTERIZERS
//---------------------------------------------------------------------------
void CGContext::m_cgRasterize(int primitiveNumber)
{
  // Rasterize one primitive with the correct rasterizer.
  // Note: without clipping, this is always primitiveNumber=0, using
  // vertices (0) for points, (0,1) for lines and (0,1,2) for triangles.
  // BUT: with clipping, more primitives might be created and vertices
  // (N*i+0, N*i+1, N*i+2) must be used (where N=3 for triangles).
  
  switch(m_primitiveMode) {
    case CG_POINTS:     m_cgRasterizePoint(primitiveNumber*1);
                        break;
    case CG_LINES:      if (m_capabilities.useBresenham) 
                          m_cgRasterizeLineBresenham(primitiveNumber*2);
                        else 
                          m_cgRasterizeStupidLine(primitiveNumber*2);
                        break;
	case CG_TRIANGLES:  m_cgRasterizeTriangle(primitiveNumber*3);
                        break;
    // Insert other rasterizers here.
  }

  // Process any remaining fragments
  m_cgFlushFragments();
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizePoint(int pipelineVertexOffset)
{
  // This primitive produces only one fragment.

  // We are interested only in one vertex data:
  CGVertexData &vertex0 = m_pipelineVertices[pipelineVertexOffset+0];

  // Initialize the fragment.
  CGFragmentData fragment;
  m_cgSetFragment(fragment,vertex0);

  // And set coordinates. (SetFragment initializes just the attributes + varyings).
  fragment.fragmentCoordinates.set((int) vertex0.vertexVaryings[CG_POSITION_VARYING][X],
                                   (int) vertex0.vertexVaryings[CG_POSITION_VARYING][Y]);

  // Push the fragment into the pipeline.
  m_cgPushFragment(fragment);
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeStupidLine(int pipelineVertexOffset)
{
  // This primitive produces many fragments from 2 vertices:
  CGVertexData &vertex0 = m_pipelineVertices[pipelineVertexOffset+0];
  CGVertexData &vertex1 = m_pipelineVertices[pipelineVertexOffset+1];

  // draw a line from vertex0 to vertex1
  CGVec2i from,to;

  from.set((int)vertex0.vertexVaryings[CG_POSITION_VARYING][X],
           (int)vertex0.vertexVaryings[CG_POSITION_VARYING][Y]);
  
  to.set((int)vertex1.vertexVaryings[CG_POSITION_VARYING][X],
         (int)vertex1.vertexVaryings[CG_POSITION_VARYING][Y]);

  if (from[X] > to[X]) {
    // swap from and to 
    CGVec2i tmp;
    tmp=from;
    from=to;
    to=tmp;
  }

  // Fragment to work on (initialize from vertex, set coordinates, push).
  CGFragmentData fragment;
  m_cgSetFragment(fragment, vertex0);

  // draw a line from <from> to <to>
  CGVec2i delta;
  delta[X]=to[X]-from[X];
  delta[Y]=to[Y]-from[Y];

  // raster along x
  if (delta[X] == 0) {
    // avoid division by zero    
    delta[X]=1;
  }
  float m=(float)delta[Y]/(float)delta[X];
  for(int x=from[X]; x<=to[X]; x++) {
    int y=(int)((float)(x-from[X])*m) + from[Y]; 

    fragment.fragmentCoordinates[X] = x;
    fragment.fragmentCoordinates[Y] = y;

    // Push the fragment into the pipeline.
    m_cgPushFragment(fragment);
  }

  // Uebung01, Aufgabe 4a) (Zusatzaufgabe)
  // ...
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeLineBresenham(int pipelineVertexOffset)
{
  // This primitive produces many fragments from 2 vertices:
  CGVertexData &vertex0 = m_pipelineVertices[pipelineVertexOffset+0];
  CGVertexData &vertex1 = m_pipelineVertices[pipelineVertexOffset+1];

  // draw a line from vertex0 to vertex1
  CGVec2i from,to;
  from.set((int)vertex0.vertexVaryings[CG_POSITION_VARYING][X],
           (int)vertex0.vertexVaryings[CG_POSITION_VARYING][Y]);
  to.set((int)vertex1.vertexVaryings[CG_POSITION_VARYING][X],
         (int)vertex1.vertexVaryings[CG_POSITION_VARYING][Y]);

  // Fragment to work on (initialize from vertex, set coordinates, push).
  CGFragmentData fragment, fromFragment, toFragment;
  m_cgSetFragment(fromFragment, vertex0);
  m_cgSetFragment(toFragment, vertex1);
/*
  // measurements done using 100000 lines on Intel Core i7 3.4 GHz
  // naive implementation, 275 ms
  int x = from[X], y;
  int dx = to[X]-from[X];
  int dy = to[Y]-from[Y];
  float m = (float)dy/(float)dx;
  float n = (float)from[Y]-m*(float)from[X];
  while (x <= to[X]) {
    y = (int)(m*x+n+0.5f);
    fragment.fragmentCoordinates.set(x,y);
    m_cgPushFragment(fragment);
    ++x;
  }

  // incremental computation, 250 ms
  int x = from[X];
  float y = from[Y];
  int dx = to[X]-from[X];
  int dy = to[Y]-from[Y];
  float m = (float)dy/(float)dx;
  while (x <= to[X]) {
    fragment.fragmentCoordinates.set(x,(int)(y+0.5f));
    m_cgPushFragment(fragment);
    ++x;
	y += m;
  }

  // increment y on side switch, 235 ms
  int x = from[X];
  int y = from[Y];
  int dx = to[X]-from[X];
  int dy = to[Y]-from[Y];
  float m = (float)dy/(float)dx, d = m;
  while (x <= to[X]) {
    fragment.fragmentCoordinates.set(x,y);
    m_cgPushFragment(fragment);
    ++x;
    if(d < 0.5f)
      d += m;
    else {
      d += m-1.0f;
      ++y;
	}
  }

  // integer decision variable, 228 ms
  int x = from[X];
  int y = from[Y];
  int dx = to[X]-from[X];
  int dy = to[Y]-from[Y];
  int d = 2*dy;
  while (x <= to[X]) {
    fragment.fragmentCoordinates.set(x,y);
    m_cgPushFragment(fragment);
    ++x;
    if(d < dx)
      d += 2*dy;
    else {
      d += 2*(dy-dx);
      ++y;
	}
  }

  // precomputed increments and coparison to 0 -> Bresenham, 224 ms
  int x = from[X];
  int y = from[Y];
  int dx = to[X]-from[X];
  int dy = to[Y]-from[Y];
  int d = 2*dy-dx;
  int deltaE = 2*dy;
  int deltaNE = 2*(dy-dx);
  while (x <= to[X]) {
    fragment.fragmentCoordinates.set(x,y);
    m_cgPushFragment(fragment);
    ++x;
    if(d < 0)
      d += deltaE;
    else {
      d += deltaNE;
      ++y;
	}
  }
*/
  // generic Bresenham, 240 ms
  // account for negative increment
  int x = from[X];
  int y = from[Y];
  int dx = to[X]-from[X];
  int dy = to[Y]-from[Y];
  int dirx = (dx>=0) ? 1 : -1;
  int diry = (dy>=0) ? 1 : -1;

  // account for steep slopes
  if(abs(dx) > abs(dy))
  {
    // increment ratio from 0 to 1
    float ratio = 0.0f, ratiostep = 1.0f / float(abs(dx));
    int d = 2*(diry*dy)-(dirx*dx);
    int deltaE = 2*(diry*dy);
    int deltaNE = 2*((diry*dy)-(dirx*dx));
    while (dirx>0 && x <= to[X] || dirx<0 && x >= to[X]) {
      // draw interpolated fragment
      fragment.fragmentCoordinates.set(x,y);
      fragment.set(fromFragment, toFragment, ratio);
      m_cgPushFragment(fragment);
      ratio += ratiostep;

      // increment
      x += dirx;
      if(d < 0)
        d += deltaE;
      else {
        d += deltaNE;
        y += diry;
      }
    }
  }
  else
  {
    // increment ratio from 0 to 1
    float ratio = 0.0f, ratiostep = 1.0f / float(abs(dy));
    int d = 2*(dirx*dx)-(dy*diry);
    int deltaE = 2*(dirx*dx);
    int deltaNE = 2*((dirx*dx)-(dy*diry));
    while (diry>0 && y <= to[Y] || diry<0 && y >= to[Y]) {
      // draw interpolated fragment
      fragment.fragmentCoordinates.set(x,y);
      fragment.set(fromFragment, toFragment, ratio);
      m_cgPushFragment(fragment);
      ratio += ratiostep;

      // increment
      y += diry;
      if(d < 0)
        d += deltaE;
      else {
        d += deltaNE;
        x += dirx;
      }
    }
  }
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeWireTriangle(int pipelineVertexOffset)
{
  CGVertexData *vertex = m_pipelineVertices + pipelineVertexOffset;

  // Skip triangle if viewing back face.
  if(m_capabilities.cullFace &&
     m_cgBFCullTriangle(vertex[0].vertexVaryings[CG_POSITION_VARYING],
                        vertex[1].vertexVaryings[CG_POSITION_VARYING],
                        vertex[2].vertexVaryings[CG_POSITION_VARYING]))
    return;

  CGVec2i from,to;
  CGFragmentData fragment, fromFragment, toFragment;

  // draw a line for each edge
  for(int side=0; side<3; ++side)
  {
    m_cgSetFragment(fromFragment, vertex[side]);
    m_cgSetFragment(toFragment, vertex[(side+1)%3]);
    from.set( (int)(fromFragment.fragmentVaryings[CG_POSITION_VARYING][X]),
              (int)(fromFragment.fragmentVaryings[CG_POSITION_VARYING][Y]));
    to.set(   (int)(toFragment.fragmentVaryings[CG_POSITION_VARYING][X]),
              (int)(toFragment.fragmentVaryings[CG_POSITION_VARYING][Y]));

    // account for negative increment
    int x = from[X];
    int y = from[Y];
    int dx = to[X]-from[X];
    int dy = to[Y]-from[Y];
    int dirx = (dx>=0) ? 1 : -1;
    int diry = (dy>=0) ? 1 : -1;

    // account for steep slopes
    if(abs(dx) > abs(dy))
    {
      // increment ratio from 0 to 1
      float ratio = 0.0f, ratiostep = 1.0f / float(abs(dx));
      int d = 2*(diry*dy)-(dirx*dx);
      int deltaE = 2*(diry*dy);
      int deltaNE = 2*((diry*dy)-(dirx*dx));
      while (dirx>0 && x <= to[X] || dirx<0 && x >= to[X]) {
        // draw interpolated fragment
        fragment.fragmentCoordinates.set(x,y);
        fragment.set(fromFragment, toFragment, ratio);
        m_cgPushFragment(fragment);
        ratio += ratiostep;

        // increment
        x += dirx;
        if(d < 0)
          d += deltaE;
        else {
          d += deltaNE;
          y += diry;
        }
      }
    }
    else
    {
      // increment ratio from 0 to 1
      float ratio = 0.0f, ratiostep = 1.0f / float(abs(dy));
      int d = 2*(dirx*dx)-(dy*diry);
      int deltaE = 2*(dirx*dx);
      int deltaNE = 2*((dirx*dx)-(dy*diry));
      while (diry>0 && y <= to[Y] || diry<0 && y >= to[Y]) {
        // draw interpolated fragment
        fragment.fragmentCoordinates.set(x,y);
        fragment.set(fromFragment, toFragment, ratio);
        m_cgPushFragment(fragment);
        ratio += ratiostep;

        // increment
        y += diry;
        if(d < 0)
          d += deltaE;
        else {
          d += deltaNE;
          x += dirx;
        }
      }
    }
  }
}
//---------------------------------------------------------------------------
void CGContext::m_cgRasterizeTriangle(int pipelineVertexOffset)
{
  CGVertexData *vertex = m_pipelineVertices + pipelineVertexOffset;

  // Skip triangle if viewing back face.
  if(m_capabilities.cullFace &&
     m_cgBFCullTriangle(vertex[0].vertexVaryings[CG_POSITION_VARYING],
                        vertex[1].vertexVaryings[CG_POSITION_VARYING],
                        vertex[2].vertexVaryings[CG_POSITION_VARYING]))
    return;

  CGVec2i from,to;
  CGFragmentData fragment, fromFragment, toFragment;

  m_scanlines.clearLines();

  // draw a line for each edge
  for(int side=0; side<3; ++side)
  {
    m_cgSetFragment(fromFragment, vertex[side]);
    m_cgSetFragment(toFragment, vertex[(side+1)%3]);
    from.set( (int)(fromFragment.fragmentVaryings[CG_POSITION_VARYING][X]),
              (int)(fromFragment.fragmentVaryings[CG_POSITION_VARYING][Y]));
    to.set(   (int)(toFragment.fragmentVaryings[CG_POSITION_VARYING][X]),
              (int)(toFragment.fragmentVaryings[CG_POSITION_VARYING][Y]));
    int x = from[X];
    int y = from[Y];
    int dx = to[X]-from[X];
    int dy = to[Y]-from[Y];
    int dirx = (dx>=0) ? 1 : -1;
    int diry = (dy>=0) ? 1 : -1;
    if(abs(dx) > abs(dy))
    {
      float ratio = 0.0f, ratiostep = 1.0f / float(abs(dx));
      int d = 2*(diry*dy)-(dirx*dx);
      int deltaE = 2*(diry*dy);
      int deltaNE = 2*((diry*dy)-(dirx*dx));
      while (dirx>0 && x <= to[X] || dirx<0 && x >= to[X]) {
        fragment.fragmentCoordinates.set(x,y);
        fragment.set(fromFragment, toFragment, ratio);
        m_scanlines.addBoundaryFragment(fragment);
        ratio += ratiostep;
        x += dirx;
        if(d < 0)
          d += deltaE;
        else {
          d += deltaNE;
          y += diry;
        }
      }
    }
    else
    {
      float ratio = 0.0f, ratiostep = 1.0f / float(abs(dy));
      int d = 2*(dirx*dx)-(dy*diry);
      int deltaE = 2*(dirx*dx);
      int deltaNE = 2*((dirx*dx)-(dy*diry));
      while (diry>0 && y <= to[Y] || diry<0 && y >= to[Y]) {
        fragment.fragmentCoordinates.set(x,y);
        fragment.set(fromFragment, toFragment, ratio);
        m_scanlines.addBoundaryFragment(fragment);
        ratio += ratiostep;
        y += diry;
        if(d < 0)
          d += deltaE;
        else {
          d += deltaNE;
          x += dirx;
        }
      }
    }
  }

  // fill for all valid lines
  int ymin = m_scanlines.getLower(), ymax = m_scanlines.getUpper();
  for(int line=ymin; line<=ymax; line++) {
    if(!m_scanlines.isLineValid(line)) continue;
    const CGFragmentData &fromF = m_scanlines.getLineMin(line);
    const CGFragmentData &toF   = m_scanlines.getLineMax(line);
    int xmin = fromF.fragmentCoordinates[0],
        xmax = toF.fragmentCoordinates[0];
    float ratio = 0.0f, ratio_incr = (xmax!=xmin)?(1.0f/float(xmax-xmin)):0.0f;
    for(int x = xmin; x<=xmax; x++) {
      if(x>=m_frameBuffer.getWidth()) break;
      if(x>=0) {
        fragment.fragmentCoordinates.set(x,line);
        fragment.set(fromF,toF,ratio);
        m_cgPushFragment(fragment);
      }
      ratio += ratio_incr;
    }
  }
}
//---------------------------------------------------------------------------