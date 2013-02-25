#include "CGContext.h"

#include "CG1Helper.h"    // We need to speak with the helper during initialization.
#include <iostream>       // If we need to print something to the console...

//---------------------------------------------------------------------------
// CGCONTEXT
//---------------------------------------------------------------------------
CGContext::CGContext(int width, int height)
{
  m_frameBuffer.allocate(width, height);

  m_scanlines.allocate(width, height);

  // Default all internal states.
  m_pVertexAttributePointer[0] = 0;
  m_pVertexAttributePointer[1] = 0;
  m_pVertexAttributePointer[2] = 0;
  m_pVertexAttributePointer[3] = 0;
  m_clearColor[R] = 0;
  m_clearColor[G] = 0;
  m_clearColor[B] = 0;
  m_clearColor[A] = 1;

  m_pipelineFragmentsCount = 0;

  m_capabilities.blend         = false;
  m_capabilities.lighting      = false;
  m_capabilities.depthTest     = false;
  m_capabilities.cullFace      = false;
  m_capabilities.useBresenham  = false;
  m_capabilities.colorMaterial = false;
  m_capabilities.phongShading  = false;

  cgViewport(0,0,width,height);

  // And make our framebuffer known to the helper.
  CG1Helper::AttachSoftwareFrameBuffer(&m_frameBuffer);
}

//---------------------------------------------------------------------------
CGContext::~CGContext()
{
}
//---------------------------------------------------------------------------
void CGContext::cgClearColor(float red, float green, float blue, float alpha)
{
  m_clearColor.set(red, green, blue, alpha);
}
//---------------------------------------------------------------------------
void CGContext::cgClear(int bitfield)
{
  if (bitfield&CG_COLOR_BUFFER_BIT) m_frameBuffer.colorBuffer.clear(m_clearColor.elements);
  if (bitfield&CG_DEPTH_BUFFER_BIT) m_frameBuffer.depthBuffer.clear();
}
//---------------------------------------------------------------------------
void CGContext::cgVertexAttribPointer(CGenum attribute,
                                      float *data)
{
  m_pVertexAttributePointer[attribute] = data;
}
//---------------------------------------------------------------------------
void CGContext::cgDrawArrays(CGenum mode, 
                             int first, 
                             int count)
{
  // Set primitive mode.
  m_primitiveMode=mode;

  // Reset rasterizer state.
  m_pipelineVerticesCount = 0;
  
  // CG RENDER PIPELINE
  // For all vertices.
  for(int iVertex=0; iVertex<count; iVertex++) {
    
    // Pull next vertex from vertex data.
    m_cgPullVertex(first+iVertex);
    
    // Start Pipeline (including rasterizer with fragment pipeline).
    m_cgVertexPipeline();
  }
}
//---------------------------------------------------------------------------
// STATE MODIFIERS
//---------------------------------------------------------------------------
void CGContext::cgEnable(CGenum cap)
{
  switch(cap) {
    case CG_CULL_FACE:      m_capabilities.cullFace     = true; break;
    case CG_LIGHTING:       m_capabilities.lighting     = true; break;
    case CG_DEPTH_TEST:     m_capabilities.depthTest    = true; break;
    case CG_BLEND:          m_capabilities.blend        = true; break;
    case CG_USE_BRESENHAM:  m_capabilities.useBresenham = true; break;
    case CG_COLOR_MATERIAL: m_capabilities.colorMaterial= true; break;
    case CG_PHONG_SHADING:  m_capabilities.phongShading = true; break;
  }
}
//---------------------------------------------------------------------------
void CGContext::cgDisable(CGenum cap)
{
  switch(cap) {
    case CG_CULL_FACE:      m_capabilities.cullFace     = false; break;
    case CG_LIGHTING:       m_capabilities.lighting     = false; break;
    case CG_DEPTH_TEST:     m_capabilities.depthTest    = false; break;
    case CG_BLEND:          m_capabilities.blend        = false; break;
    case CG_USE_BRESENHAM:  m_capabilities.useBresenham = false; break;
    case CG_COLOR_MATERIAL: m_capabilities.colorMaterial= false; break;
    case CG_PHONG_SHADING:  m_capabilities.phongShading = false; break;
  }
}
//---------------------------------------------------------------------------
// UNIFORM MODIFIERS
//---------------------------------------------------------------------------
void CGContext::cgUniform4fv(int location, int count,
    const float* value)
{
    if(count == 1) {
        CGVec4 vec4; vec4.set(value[0],value[1],value[2],value[3]);
        switch(location) {
        case CG_ULOC_LIGHT0_AMBIENT: m_uniforms.light0Ambient = vec4; break;
        case CG_ULOC_LIGHT0_DIFFUSE: m_uniforms.light0Diffuse = vec4; break;
        case CG_ULOC_LIGHT0_SPECULAR: m_uniforms.light0Specular = vec4; break;
        case CG_ULOC_LIGHT0_POSITION: m_uniforms.light0Position = vec4; break;
        case CG_ULOC_LIGHT0_SPOT_DIRECTION: m_uniforms.light0SpotDirection = vec4; break;
        case CG_ULOC_MATERIAL_AMBIENT: m_uniforms.materialAmbient = vec4; break;
        case CG_ULOC_MATERIAL_DIFFUSE: m_uniforms.materialDiffuse = vec4; break;
        case CG_ULOC_MATERIAL_SPECULAR: m_uniforms.materialSpecular = vec4; break;
        case CG_ULOC_MATERIAL_EMISSION: m_uniforms.materialEmission = vec4; break;
        case CG_ULOC_SCENE_AMBIENT: m_uniforms.sceneAmbient = vec4; break;
            // case ULOC_?
        }
    }
    else {
        // If count > 0 specified, create vec4 with value+4*i for each i<count
        // and assign to m_uniforms.???[i].
    }
}
//---------------------------------------------------------------------------
void CGContext::cgUniformMatrix4fv(int location, int count,
  bool transpose, const float* value)
{
  if(count == 1) {
    CGMatrix4x4 mat; mat.setFloatsFromColMajor(value);
    if(transpose) mat.transpose();
    switch(location) {
    case CG_ULOC_MODELVIEW_MATRIX: m_uniforms.modelviewMatrix = mat; break;
    case CG_ULOC_PROJECTION_MATRIX: m_uniforms.projectionMatrix = mat; break;
    case CG_ULOC_NORMAL_MATRIX: m_uniforms.normalMatrix = mat; break;
    }
  } else {
    // If count > 0 specified, create matrix with value+16*i for each i<count
    // and assign to m_uniforms.???[i].
  }
}
//---------------------------------------------------------------------------
// VIEWPORT MODIFIERS
//---------------------------------------------------------------------------
void CGContext::cgViewport(int x, int y, int w, int h)
{
  m_viewport[0]=x;
  m_viewport[1]=y;
  m_viewport[2]=w;
  m_viewport[3]=h;
}

//---------------------------------------------------------------------------
// PIPELINE COMPONENTS: see separate CGContext_*.cpp files.
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
void CGContext::cgUniform1fv(int location, int count,
    const float* value)
{
    if(count == 1) {
        switch(location) {
        case CG_ULOC_MATERIAL_SHININESS: m_uniforms.materialShininess=value[0]; break;
        case CG_ULOC_LIGHT0_CONSTANT_ATTENUATION: m_uniforms.light0ConstantAttenuation=value[0]; break;
        case CG_ULOC_LIGHT0_LINEAR_ATTENUATION: m_uniforms.light0LinearAttenuation=value[0]; break;
        case CG_ULOC_LIGHT0_QUADRATIC_ATTENUATION: m_uniforms.light0QuadraticAttenuation=value[0]; break;
        case CG_ULOC_LIGHT0_SPOT_EXPONENT: m_uniforms.light0SpotExponent=value[0]; break;
        case CG_ULOC_LIGHT0_SPOT_CUTOFF: m_uniforms.light0SpotCutoff=value[0]; break;
            // case ULOC_?
        }
    }
    else {
        // Loop over all values and assign to m_uniforms.???[i] = value[i]
    }
}
//---------------------------------------------------------------------------
