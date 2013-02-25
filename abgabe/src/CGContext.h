#ifndef CGCONTEXT_H
#define CGCONTEXT_H

#include "CG.h"            // This is the internal global header.
#include "CGFrameBuffer.h" // Our context needs a framebuffer.
#include "CGScanlineSet.h" // Scanlines for triangle rasterization.
#include "CGMatrix.h"

#include <math.h>
#include <string.h>

//---------------------------------------------------------------------------

class CGContext 
{
  // Attributes.
private:

  /// Framebuffer.
  CGFrameBuffer m_frameBuffer;

  /// Rasterizier bounding set.
  CGScanlineSet m_scanlines;

  /// Internal state: vertex attribute pointers.
  float *m_pVertexAttributePointer[4];
  
  /// Internal state: current primitive mode.
  int m_primitiveMode;

  /// Internal state: current program.
  int m_program;

  /// Internal state: clear color.
  CGVec4 m_clearColor;

  /// Internal state: vertex data still in the pipeline (not yet enough for the respective primitive type).
  CGVertexData m_pipelineVertices[CG_MAX_VERTICES_IN_PIPELINE];

  /// Internal state: number of vertices still hanging in the pipeline (for rasterizer).
  int m_pipelineVerticesCount;

  /// Internal state: fragments waiting to be processed.
  CGFragmentData m_pipelineFragments[CG_MAX_FRAGMENTS_IN_PIPELINE];

  /// Internal state: number of fragments waiting to be processed.
  int m_pipelineFragmentsCount;

  /// Internal state: capabilities.
  struct {
    bool blend, lighting, depthTest, cullFace, useBresenham, colorMaterial, phongShading;
  } m_capabilities;

  /// Internal state: uniform variables.
  struct {
      CGMatrix4x4 projectionMatrix, modelviewMatrix;
      CGMatrix4x4 normalMatrix;
      CGVec4 materialAmbient, materialDiffuse, materialSpecular, materialEmission;
      float materialShininess;
      CGVec4 light0Ambient, light0Diffuse, light0Specular, light0Position, light0SpotDirection;
      float light0ConstantAttenuation, light0LinearAttenuation, light0QuadraticAttenuation,
            light0SpotExponent, light0SpotCutoff;
      CGVec4 sceneAmbient;
  } m_uniforms;

  /// Internal state: viewport
  int m_viewport[4];

  // Methods.
public:
  /// Context constructor.
  CGContext(int width, int height);

  /// Context destructor.
  ~CGContext();

  /// Provide specific vertex attribute. Supported: CG_POSITION, CG_NORMAL, CG_COLOR, CG_TEXCOORD.
  void cgVertexAttribPointer(CGenum attribute, float *data);

  /// Clears the bitwise specified buffers by the current clear value (e.g. m_cgClearColor)
  void cgClearColor(float red, float green, float blue, float alpha);

  /// Clears the bitwise specified buffers by the current clear value (e.g. m_cgClearColor)
  void cgClear(int bitfield);

  /// Render primitives with given mode. Note: this calls the pipeline directly (in our implementation).
  void cgDrawArrays(CGenum mode, int first, int count);

  /// Enables capability. Supported: CG_CULL_FACE, CG_LIGHTING, Cg_DEPTH_TEST, CG_BLEND.
  void cgEnable(CGenum cap);

  /// Disables capability. Supported: CG_CULL_FACE, CG_LIGHTING, Cg_DEPTH_TEST, CG_BLEND.
  void cgDisable(CGenum cap);

  /// Sets polygon mode, only supports CG_LINE and CG_FILL for BOTH faces.
  void cgPolygonMode(CGenum mode);

  /// Set uniform 4f variable. Pass count 1 for single 4f variable, >1 for multiple.
  void cgUniform4fv(int location, int count, const float* value);

  /// Set uniform matrix 4x4f variable. Pass count 1 for single matrix.
  /// Transpose when passing row-major.
  void cgUniformMatrix4fv(int location, int count, bool transpose, const float* value);

  /// Set the viewport
  void cgViewport(int x, int y, int width, int height);

  /// Set uniform float variable. Pass count 1 for single float, >1 for multiple.
  void cgUniform1fv(int location, int count, const float* value);

private:
  // Pipeline methods (_VertexOperations.cpp).

  /// Initialize pipeline vertex with given (or default) attributes.
  void m_cgPullVertex(int vertexIndex);

  /// Execute pipeline components on current vertex data.
  void m_cgVertexPipeline();

  /// Transform current vertex (e. g. by projection and modelview matrices)
  /// and calculate per-Vertex Lighting.
  void m_cgVertexProgram();

  /// Process primitive (clipping and finishing vertex pipeline) if enough vertices piped.
  void m_cgPrimitiveProcessing();

  /// Transform Clip Space to Normalized Device Coordinates (NDC)
  void m_cgVertexPerspectiveDivide(int pipelineVertex);

  /// Transform Normalized Device Coordinates to Window Space
  void m_cgVertexViewportTransform(int pipelineVertex);

  // Pipeline methods (_PrimitiveOperations.cpp).

  /// Clip primitive (in clip space) and create new primitives if needed. Returns number of resulting primitives.
  int m_cgClipPrimitive();
  
  /// Test if triangle should be culled. Returns true if culled.
  bool m_cgBFCullTriangle(CGVec4 &vertex0Position, CGVec4 &vertex1Position, CGVec4 &vertex2Position);

  /// Rasterize primitive and execute pipeline fragment part.
  void m_cgRasterize(int primitiveNumber);

  /// Simple point rasterizer.
  void m_cgRasterizePoint(int pipelineVertexOffset);
  
  /// Stupid line rasterizer.
  void m_cgRasterizeStupidLine(int pipelineVertexOffset);

  /// Bresenham rasterizer.
  void m_cgRasterizeLineBresenham(int pipelineVertexOffset);

  /// Wireframe triangle rasterizer.
  void m_cgRasterizeWireTriangle(int pipelineVertexOffset);

  /// Triangle rasterizer.
  void m_cgRasterizeTriangle(int pipelineVertexOffset);


  // Pipeline methods (_FragmentOperations.cpp).

  /// Initialize pipeline fragment data from (pipeline) vertex data
  void m_cgSetFragment(CGFragmentData& fragment, CGVertexData& vertex);

  /// Enqueue fragment for processing
  void m_cgPushFragment(CGFragmentData& fragment);

  /// Process all fragments currently in the pipeline.
  void m_cgFlushFragments();

  /// Execute pipeline components on fragment data.
  void m_cgFragmentPipeline(CGFragmentData& fragment);

  /// Test current fragment coordinates against framebuffer. Returns false if to be clipped.
  bool m_cgFragmentClipping(CGFragmentData& fragment);

  /// Test current fragment against depth buffer and update. Returns false if test fails.
  bool m_cgFragmentZTest(CGFragmentData& fragment);

  /// Blend the fragment color from the underlying framebuffer color. Returns true.
  bool m_cgFragmentBlending(CGFragmentData& fragment);

  /// Write current fragment into fragment buffer.
  void m_cgFragmentWriteBuffer(CGFragmentData& fragment);

  /// Fragment Program
  bool m_cgFragmentProgram(CGFragmentData& fragment);
};

//---------------------------------------------------------------------------

#endif

