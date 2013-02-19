#include "CGContext.h"
#include <iostream>
#include "CGMath.h"


//---------------------------------------------------------------------------
// CGCONTEXT (FRAGMENT OPERATIONS)
//---------------------------------------------------------------------------
void CGContext::m_cgSetFragment(CGFragmentData& fragment, CGVertexData& vertex)
{
  fragment.set(vertex);
}
//---------------------------------------------------------------------------
void CGContext::m_cgPushFragment(CGFragmentData& fragment)
{
  // Wait for enough fragments to process.
  m_pipelineFragments[m_pipelineFragmentsCount++] = fragment;
  if(m_pipelineFragmentsCount == CG_MAX_FRAGMENTS_IN_PIPELINE)
    m_cgFlushFragments();
}
//---------------------------------------------------------------------------
void CGContext::m_cgFlushFragments()
{
  // Run fragment pipeline components for each fragment.
  for(int i=0; i<m_pipelineFragmentsCount; ++i)
    m_cgFragmentPipeline(m_pipelineFragments[i]);

  // All fragments processed, clear pipeline.
  m_pipelineFragmentsCount = 0;
}
//---------------------------------------------------------------------------
void CGContext::m_cgFragmentPipeline(CGFragmentData& fragment)
{
  if(!m_cgFragmentClipping(fragment))
    return;

  if(!m_cgFragmentProgram(fragment))
    return;

  if(m_capabilities.depthTest)
    if(!m_cgFragmentZTest(fragment))
      return;

  if(m_capabilities.blend)
    if(!m_cgFragmentBlending(fragment)) 
      return;

  m_cgFragmentWriteBuffer(fragment);
}
//---------------------------------------------------------------------------
bool CGContext::m_cgFragmentClipping(CGFragmentData& fragment)
{
  if((fragment.fragmentCoordinates[0]<m_viewport[0] )
    || (fragment.fragmentCoordinates[0]>=m_viewport[0]+m_viewport[2])
    || (fragment.fragmentCoordinates[1]<m_viewport[1])
    || (fragment.fragmentCoordinates[1]>=m_viewport[1]+m_viewport[3])
    || (fragment.fragmentVaryings[CG_POSITION_VARYING][Z]>1.0f)
    || (fragment.fragmentVaryings[CG_POSITION_VARYING][Z]<-1.0f))
    return false;
  return true;
}
//---------------------------------------------------------------------------
bool CGContext::m_cgFragmentZTest(CGFragmentData& fragment)
{
  //returns true if the fragment is visible
  
  float ZatXY = m_frameBuffer.depthBuffer.get(fragment.fragmentCoordinates[X],
                                              fragment.fragmentCoordinates[Y]);
  static const float depthTolerance = 1e-6f;
  return fragment.fragmentVaryings[CG_POSITION_VARYING][Z]-depthTolerance <= ZatXY;
}
//---------------------------------------------------------------------------
bool CGContext::m_cgFragmentBlending(CGFragmentData& fragment)
{
  if(m_capabilities.blend) {
    float* rgbaSrc = fragment.fragmentVaryings[CG_COLOR_VARYING].elements;
    float rgbaDst[4];
    m_frameBuffer.colorBuffer.get(fragment.fragmentCoordinates[X],
                                  fragment.fragmentCoordinates[Y],
                                  rgbaDst);
    float alphaSrc = rgbaSrc[3];
    for(int c=0; c<4; c++) rgbaSrc[c] = alphaSrc*rgbaSrc[c]+(1.0f-alphaSrc)*rgbaDst[c];
  }
  return true;
}
//---------------------------------------------------------------------------
void CGContext::m_cgFragmentWriteBuffer(CGFragmentData& fragment)
{
  // Write the current fragment into the framebuffer.
  m_frameBuffer.colorBuffer.set(fragment.fragmentCoordinates[X],
                                fragment.fragmentCoordinates[Y],
                                fragment.fragmentVaryings[CG_COLOR_VARYING].elements);
  m_frameBuffer.depthBuffer.set(fragment.fragmentCoordinates[X],
                                fragment.fragmentCoordinates[Y],
                                fragment.fragmentVaryings[CG_POSITION_VARYING][Z]);
}
//---------------------------------------------------------------------------
bool CGContext::m_cgFragmentProgram(CGFragmentData& fragment)
{
    CGVec4 &vClr = fragment.fragmentVaryings[CG_COLOR_VARYING];
    CGVec4 &vNrm = fragment.fragmentVaryings[CG_NORMAL_VARYING];
    CGVec4 &txc = fragment.fragmentVaryings[CG_TEXCOORD_VARYING];
    CGVec4 &vPos_Eye = fragment.fragmentVaryings[CG_POSITION_EYESPACE_VARYING];

    if(m_capabilities.lighting && m_capabilities.phongShading)
    {
        // renormalize the normal
        vNrm = CGMath::normalize(vNrm);
        // Compute Blinn-Phong reflection model.
        CGVec4 emis, ambi, diff, spec, scene_ambi;
        emis.set(0.0f, 0.0f, 0.0f, 0.0f); ambi.set(0.0f, 0.0f, 0.0f, 0.0f);
        diff.set(0.0f, 0.0f, 0.0f, 0.0f); spec.set(0.0f, 0.0f, 0.0f, 0.0f);

        scene_ambi.set(0.0f, 0.0f, 0.0f, 0.0f);

        // init attenuation 
        float attenuation = 1.0f;

        // init spot factor
        float spot = 1.0f;

        // emission
        emis = m_uniforms.materialEmission;
        
        // ambient
        ambi = CGMath::mul(m_uniforms.materialAmbient,m_uniforms.light0Ambient);
        
        // L is vector direction from current point (vPos) to the light source (m_uniforms.light0Position)
        // check for sun light (directional light)
        CGVec4 L;
        if (m_uniforms.light0Position[W] == 0)
            L = m_uniforms.light0Position;
        else
            L = CGMath::sub(m_uniforms.light0Position, vPos_Eye);

        L = CGMath::normalize(L);

        // calculate attenuation if necessary
        if (m_uniforms.light0Position[W] == 1)
        {
            CGVec4 LminV = CGMath::sub(m_uniforms.light0Position, vPos_Eye);

            float ac = m_uniforms.light0ConstantAttenuation;
            float al = m_uniforms.light0LinearAttenuation;
            float aq = m_uniforms.light0QuadraticAttenuation;
            
            attenuation = ac + (al * CGMath::length(LminV)) + (aq * (CGMath::length(LminV) * CGMath::length(LminV) ));
            attenuation = 1 / attenuation;
        }
            
        // spot calculations, remember the cosine of the cutoff is saved
        if (m_uniforms.light0SpotCutoff != -1)
        {
            // calculate normalized spot direction
            CGVec4 S = CGMath::normalize(m_uniforms.light0SpotDirection);
            CGVec4 negL = CGMath::scale(L, -1);

            float dot_s_neg_l = CGMath::dot(S, negL);
            
            float max = 0;
            if (dot_s_neg_l > max)
                max = dot_s_neg_l;

            if (max >= m_uniforms.light0SpotCutoff)
                spot = pow(max, m_uniforms.light0SpotExponent);
            else
                spot = 0;
        }

        // calculate dot product of vNrm and L
        float NdotL = CGMath::dot(vNrm, L); 
        // diffuse
        if(NdotL>0)
        {
            diff = CGMath::mul(m_uniforms.materialDiffuse, m_uniforms.light0Diffuse);
            diff = CGMath::scale(diff, NdotL);
        }
        // E is direction from current point (vPos) to eye position
        CGVec4 p_e;
        p_e.set(0.0,0.0,0.0,1.0);
        CGVec4 E= CGMath::sub(p_e, vPos_Eye);
        E= CGMath::normalize(E);
        // H is half vector between L and E
        CGVec4 H= CGMath::add(L, E);
        H=CGMath::normalize(H);
        // specular
        float NdotH = CGMath::dot(vNrm, H);
        if((NdotL>0)&&(NdotH>0))
        {
            spec=CGMath::mul(m_uniforms.materialSpecular, m_uniforms.light0Specular);
            spec=CGMath::scale(spec,NdotL);
            NdotH= pow(NdotH, m_uniforms.materialShininess);
            spec= CGMath::scale(spec, NdotH);
        }

        // sum up the final output color
        // for convinience we need a inner sum
        CGVec4 inner_sum_vector =CGMath::add( CGMath::add(ambi, diff), spec);

        // apply attenuation
        inner_sum_vector = CGMath::scale(inner_sum_vector, attenuation);
        inner_sum_vector = CGMath::scale(inner_sum_vector, spot);

        // sum up the ther parts
        vClr = CGMath::add( CGMath::add(scene_ambi, inner_sum_vector), emis);

        // Explicitly set alpha of the color
        vClr[A] = m_uniforms.materialDiffuse[A];

        // clamp color values to range [0,1]
        vClr = CGMath::clamp(vClr, 0.0f, 1.0f);
    }
    return true;
}
