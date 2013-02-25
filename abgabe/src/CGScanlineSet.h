#ifndef CGSCANLINESET_H
#define CGSCANLINESET_H

#include "CG.h"
//---------------------------------------------------------------------------
class CGScanlineSet
{
public:
  /// Constructor
  CGScanlineSet();
  /// Destructor
  ~CGScanlineSet();
  /// Call this function when height or width changes.
  bool allocate(int width, int height);
  /// Reset all scanlines.
  void clearLines();
  /// Retrieve the current minimum fragment of this line.
  CGFragmentData& getLineMin(int line);
  /// Retrieve the current maximum fragment of this line.
  CGFragmentData& getLineMax(int line);
  /// Has this line a valid range (min<=max)?
  bool isLineValid(int line);
  /// Set this value as min/max value if smallest/greatest.
  void addBoundaryFragment(const CGFragmentData &frag);
  /// Get minimum y coordinate
  int getLower() const;
  /// Get maximum y coordinate
  int getUpper() const;

private:
  /// Framebuffer size and fragment data
  int width, height;
  CGFragmentData* pLines;

  /// min and max y
  int lower, upper;
};

//---------------------------------------------------------------------------

#endif