#include "CGScanlineSet.h"

#include "stdlib.h"
//---------------------------------------------------------------------------
CGScanlineSet::CGScanlineSet()
{ 
  pLines = 0;   
}
//---------------------------------------------------------------------------
CGScanlineSet::~CGScanlineSet()
{ 
  free(pLines); 
}
//---------------------------------------------------------------------------
bool CGScanlineSet::allocate(int width, int height) 
{
  lower = 0;
  upper = height-1;
  this->width = width; this->height = height;
  pLines = (CGFragmentData*) realloc(pLines, sizeof(CGFragmentData)*2*height); 
  if(!pLines) return false;
  clearLines();
  return true;
}
//---------------------------------------------------------------------------
void CGScanlineSet::clearLines() 
{
  CGFragmentData minF, maxF;
  minF.fragmentCoordinates[0] = width;  // set minimum as rightmost(+1)
  maxF.fragmentCoordinates[0] = -1;     // set maximum as leftmost (-1)
  for(int i=lower; i<=upper; i++ ) {
    pLines[2*i+0] = minF; pLines[2*i+1] = maxF;
  }
  lower = height - 1;
  upper = 0;
}
//---------------------------------------------------------------------------
CGFragmentData& CGScanlineSet::getLineMin(int line) 
{ 
  return pLines[2*line+0]; 
}
//---------------------------------------------------------------------------
CGFragmentData& CGScanlineSet::getLineMax(int line) 
{ 
  return pLines[2*line+1]; 
}
//---------------------------------------------------------------------------
bool CGScanlineSet::isLineValid(int line)
{
  return getLineMin(line).fragmentCoordinates[0]
      <= getLineMax(line).fragmentCoordinates[0]; 
}
//---------------------------------------------------------------------------
void CGScanlineSet::addBoundaryFragment(const CGFragmentData &frag)
{
  int line = frag.fragmentCoordinates[1];
  if(line<0 || line>=height) return;
  if(frag.fragmentCoordinates[0] < getLineMin(line).fragmentCoordinates[0])
    getLineMin(line)=frag;
  if(frag.fragmentCoordinates[0] > getLineMax(line).fragmentCoordinates[0])
    getLineMax(line)=frag;
  lower = (lower>line) ? line : lower;
  upper = (upper<line) ? line : upper;
}
//---------------------------------------------------------------------------
int CGScanlineSet::getLower() const
{
	return lower;
}
//---------------------------------------------------------------------------
int CGScanlineSet::getUpper() const
{
	return upper;
}
//---------------------------------------------------------------------------