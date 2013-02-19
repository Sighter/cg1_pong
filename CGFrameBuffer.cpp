#include "CGFrameBuffer.h"
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy

//---------------------------------------------------------------------------
// CGFRAMEBUFFER
//---------------------------------------------------------------------------
bool CGFrameBuffer::allocate(int width, int height)
{
  return (colorBuffer.reserve(width,height) 
       && depthBuffer.reserve(width,height));
}
//---------------------------------------------------------------------------
int CGFrameBuffer::getWidth()
{
  return colorBuffer.getWidth();
}
//---------------------------------------------------------------------------
int CGFrameBuffer::getHeight()
{
  return colorBuffer.getHeight();
}
//---------------------------------------------------------------------------
// CGFRAMEBUFFER::CGBUFFER4UB
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer4UB::CGBuffer4UB()
{
  pBufferData = 0;
}
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer4UB::~CGBuffer4UB()
{
  free(pBufferData);
}
//---------------------------------------------------------------------------
bool CGFrameBuffer::CGBuffer4UB::reserve(int width, int height)
{
  this->width = width;
  this->height = height;

  pBufferData = (unsigned char*) realloc(pBufferData,width*height*4);
  if(!pBufferData) return false;

  return true;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::get(int x, int y, float *rgba)
{
  unsigned char * pData = pBufferData + 4*(y*width + x);
  rgba[0] = float(pData[0])/255.0f;
  rgba[1] = float(pData[1])/255.0f;
  rgba[2] = float(pData[2])/255.0f;
  rgba[3] = float(pData[3])/255.0f;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::set(int x, int y, const float *rgba)
{
  unsigned char * pData = pBufferData + 4*(y*width + x);
  pData[0] = (unsigned char) (rgba[0]*255.0f);
  pData[1] = (unsigned char) (rgba[1]*255.0f);
  pData[2] = (unsigned char) (rgba[2]*255.0f);
  pData[3] = (unsigned char) (rgba[3]*255.0f);
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::clear(const float *rgba)
{
  unsigned char cColor[4] = {0,};
  cColor[0] = (unsigned char) (rgba[0]*255.0f);
  cColor[1] = (unsigned char) (rgba[1]*255.0f);
  cColor[2] = (unsigned char) (rgba[2]*255.0f);
  cColor[3] = (unsigned char) (rgba[3]*255.0f);
  for (int i = 0; i < width*height; i++)
    memcpy(pBufferData+i*4,cColor,4*sizeof(unsigned char));
}
//---------------------------------------------------------------------------
unsigned char* CGFrameBuffer::CGBuffer4UB::getDataPointer()
{
  return pBufferData;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer4UB::getWidth()
{
  return width;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer4UB::getHeight()
{
  return height;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::setWhite(int x, int y)
{
  unsigned char * pData = pBufferData + 4*(y*width + x);
  pData[0]=255;
  pData[1]=255;
  pData[2]=255;
  pData[3]=255;
}
//---------------------------------------------------------------------------
// CGFRAMEBUFFER::CGBUFFER1F
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer1f::CGBuffer1f()
{
  pBufferData = 0;
}
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer1f::~CGBuffer1f()
{
  free(pBufferData);
}
//---------------------------------------------------------------------------
bool CGFrameBuffer::CGBuffer1f::reserve(int width, int height)
{
  this->width = width;
  this->height = height;
  pBufferData = (float*) realloc(pBufferData,width*height*sizeof(float));
  if(!pBufferData) return false;

  return true;
}
//---------------------------------------------------------------------------
float CGFrameBuffer::CGBuffer1f::get(int x, int y)
{
  return pBufferData[(y*width + x)];
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer1f::set(int x, int y, const float z)
{
  pBufferData[(y*width + x)] = z;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer1f::clear()
{
  for (int i = 0; i < width*height; i++)
    pBufferData[i]=1.0f;
}
//---------------------------------------------------------------------------
float* CGFrameBuffer::CGBuffer1f::getDataPointer()
{
  return pBufferData;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer1f::getWidth()
{
  return width;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer1f::getHeight()
{
  return height;
}
//---------------------------------------------------------------------------
