#include "CGContext.h"
#include <iostream>
#include "CGMath.h"

//---------------------------------------------------------------------------
// CGCONTEXT (VERTEX OPERATIONS)
//---------------------------------------------------------------------------
void CGContext::m_cgPullVertex(int vertexIndex)
{
    if (m_pipelineVerticesCount >= CG_MAX_VERTICES_IN_PIPELINE) {
        std::cerr << "vertex per primitive limit reached" << std::endl;  
        return;
    }
    // We copy all attributes from the attribute pointers to the vertex entry v.
    // If no pointer is given, the default value is used.
    CGVertexData& v=m_pipelineVertices[m_pipelineVerticesCount++];

    if(m_pVertexAttributePointer[CG_POSITION_ATTRIBUTE]) {
        const float *pos=m_pVertexAttributePointer[CG_POSITION_ATTRIBUTE]+3*vertexIndex;
        v.vertexAttributes[CG_POSITION_ATTRIBUTE].set(pos[0], pos[1], pos[2], 1.0f);
    } else {
        v.vertexAttributes[CG_POSITION_ATTRIBUTE].set(0.0f, 0.0f, 0.0f, 1.0f);
    }

    if(m_pVertexAttributePointer[CG_NORMAL_ATTRIBUTE]) {
        const float *nrm=m_pVertexAttributePointer[CG_NORMAL_ATTRIBUTE]+3*vertexIndex;
        v.vertexAttributes[CG_NORMAL_ATTRIBUTE].set(nrm[0], nrm[1], nrm[2], 0.0f);
    } else {
        v.vertexAttributes[CG_NORMAL_ATTRIBUTE].set(0.0f, 0.0f, 1.0f, 0.0f);
    }

    if(m_pVertexAttributePointer[CG_COLOR_ATTRIBUTE]) {
        const float *col=m_pVertexAttributePointer[CG_COLOR_ATTRIBUTE]+4*vertexIndex;
        v.vertexAttributes[CG_COLOR_ATTRIBUTE].set(col[0], col[1], col[2], col[3]);
    } else {
        v.vertexAttributes[CG_COLOR_ATTRIBUTE].set(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // If color material is enabled, we use the vertex color to *overwrite*
    // the uniforms for material ambient and diffuse (similar to OpenGL).
    if(m_capabilities.colorMaterial) {
        m_uniforms.materialAmbient = v.vertexAttributes[CG_COLOR_ATTRIBUTE];
        m_uniforms.materialDiffuse = v.vertexAttributes[CG_COLOR_ATTRIBUTE];
    }

    if(m_pVertexAttributePointer[CG_TEXCOORD_ATTRIBUTE]) {
        const float *tex=m_pVertexAttributePointer[CG_TEXCOORD_ATTRIBUTE]+2*vertexIndex;
        v.vertexAttributes[CG_TEXCOORD_ATTRIBUTE].set(tex[0], tex[1], 0.0f, 1.0f);
    } else {
        v.vertexAttributes[CG_TEXCOORD_ATTRIBUTE].set(0.0f, 0.0f, 0.0f, 1.0f);
    }
}
//---------------------------------------------------------------------------
void CGContext::m_cgVertexPipeline()
{
    // Do first part of vertex processing right here.
    m_cgVertexProgram();

    // Process this vertex for the current primitive.
    m_cgPrimitiveProcessing();

    // Note: rest of the 'original' vertex pipeline is finished after
    // the primitive processing because new vertices might have been
    // created after clipping.
}
//---------------------------------------------------------------------------
/*void CGContext::m_cgVertexProgram()
{
// This function is similar to the 'vertex shader' of OpenGL.

// We are interested in the last vertex (as always if no vertex index available).
CGVertexData& v=m_pipelineVertices[m_pipelineVerticesCount-1];

// Get hold of all vertex attributes.
CGVec4 &aPos = v.vertexAttributes[CG_POSITION_ATTRIBUTE];
CGVec4 &aNrm = v.vertexAttributes[CG_NORMAL_ATTRIBUTE];
CGVec4 &aClr = v.vertexAttributes[CG_COLOR_ATTRIBUTE];
CGVec4 &aTex = v.vertexAttributes[CG_TEXCOORD_ATTRIBUTE];

// Get hold of all vertex varyings.
CGVec4 &vPos = v.vertexVaryings[CG_POSITION_VARYING];
CGVec4 &vNrm = v.vertexVaryings[CG_NORMAL_VARYING];
CGVec4 &vClr = v.vertexVaryings[CG_COLOR_VARYING];
CGVec4 &vTex = v.vertexVaryings[CG_TEXCOORD_VARYING];

// Default program copies all attributes into all varyings used:
vPos = aPos;
vNrm = aNrm;
vClr = aClr;
vTex = aTex;

// Insert any additional vertex program code here.

// Transform from Object Space into Eye Space.
// U8 Aufgabe 2a
vPos = m_uniforms.modelviewMatrix * vPos;

// Lighting
// (later)

// Transform from Eye Space into Clip Space.
vPos = m_uniforms.projectionMatrix * vPos;
}*/
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void CGContext::m_cgVertexProgram()
{
    // This function is similar to the ’vertex shader’ of OpenGL.
    // We are interested in the last vertex (as always if no vertex index available).
    CGVertexData& v=m_pipelineVertices[m_pipelineVerticesCount-1];
    // Get hold of all vertex attributes.
    CGVec4 &aPos = v.vertexAttributes[CG_POSITION_ATTRIBUTE];
    CGVec4 &aNrm = v.vertexAttributes[CG_NORMAL_ATTRIBUTE];
    CGVec4 &aClr = v.vertexAttributes[CG_COLOR_ATTRIBUTE];
    CGVec4 &aTex = v.vertexAttributes[CG_TEXCOORD_ATTRIBUTE];
    // Get hold of all vertex varyings.
    CGVec4 &vPos = v.vertexVaryings[CG_POSITION_VARYING];
    CGVec4 &vNrm = v.vertexVaryings[CG_NORMAL_VARYING];
    CGVec4 &vClr = v.vertexVaryings[CG_COLOR_VARYING];
    CGVec4 &vTex = v.vertexVaryings[CG_TEXCOORD_VARYING];
    CGVec4 &vPosEye = v.vertexVaryings[CG_POSITION_EYESPACE_VARYING];
    // Default program copies all attributes into all varyings used:
    vPos = aPos;
    vNrm = aNrm;
    vClr = aClr;
    vTex = aTex;
    vPos = m_uniforms.modelviewMatrix * vPos;
    vPosEye = vPos;
    // Transform from Object Space into Eye Space.
    vNrm = m_uniforms.normalMatrix * vNrm;
    vNrm = CGMath::normalize(vNrm);

    // If lighting is enabled and phong shading is disabled, recompute vertex colors according to lighting equation.
    if(m_capabilities.lighting && !m_capabilities.phongShading)
    {
        CGVec4 emis, ambi, diff, spec, scene_ambi;
        // TODO: for now, set them all to 0
        emis.set(0.0f, 0.0f, 0.0f, 0.0f); ambi.set(0.0f, 0.0f, 0.0f, 0.0f);
        diff.set(0.0f, 0.0f, 0.0f, 0.0f); spec.set(0.0f, 0.0f, 0.0f, 0.0f);
        scene_ambi.set(0.0f, 0.0f, 0.0f, 0.0f);

        // init attenuation 
        float attenuation = 1.0f;

        // init spot factor
        float spot = 1.0f;

        // emission
        emis = m_uniforms.materialEmission;

        // scene ambient light
        scene_ambi = CGMath::mul(m_uniforms.materialAmbient,m_uniforms.sceneAmbient);

        // ambient
        ambi = CGMath::mul(m_uniforms.materialAmbient,m_uniforms.light0Ambient);

        // L is vector direction from current point (vPos) to the light source (m_uniforms.light0Position)
        // check for sun light (directional light)
        CGVec4 L;
        if (m_uniforms.light0Position[W] == 0)
            L = m_uniforms.light0Position;
        else
            L = CGMath::sub(m_uniforms.light0Position, vPos);

        L = CGMath::normalize(L);

        // calculate attenuation if necessary
        if (m_uniforms.light0Position[W] == 1)
        {
            CGVec4 LminV = CGMath::sub(m_uniforms.light0Position, vPos);

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

        // calculate dot product of nrm and L
        float NdotL = CGMath::dot(vNrm, L); 
        // diffuse
        if(NdotL>0){
            diff = CGMath::mul(m_uniforms.materialDiffuse, m_uniforms.light0Diffuse);
            diff = CGMath::scale(diff, NdotL);
        }
        // E is direction from current point (vPos) to eye position
        CGVec4 p_e;
        p_e.set(0.0,0.0,0.0,1.0);
        CGVec4 E= CGMath::sub(p_e, vPos);
        E= CGMath::normalize(E);
        // H is half vector between L and E
        CGVec4 H= CGMath::add(L, E);
        H=CGMath::normalize(H);
        // specular
        float NdotH = CGMath::dot(vNrm, H);
        if((NdotL>0)&&(NdotH>0)){
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
    // Transform from Eye Space into Clip Space.
    
    vPos = m_uniforms.projectionMatrix * vPos;
    // (This is not reasonable for the normal vector.)
}
//---------------------------------------------------------------------------
void CGContext::m_cgPrimitiveProcessing() {

    // Primitive assembly stage: collect vertices in m_pipelineVertices
    // until enough entries for the current primitive are present.
    int verticesNeeded = 0;
    switch(m_primitiveMode) {
    case CG_POINTS:     verticesNeeded = 1; break;
    case CG_LINES:      verticesNeeded = 2; break;
    case CG_TRIANGLES:  verticesNeeded = 3; break;
        // Insert other primitive types here.
    }
    if (m_pipelineVerticesCount < verticesNeeded) {
        return;
    }
    // else m_pipelineVerticesCount == verticesNeeded and we can process this primitive

    // This might split our one primitive and create new ones.
    int pipelinePrimitiveCount = m_cgClipPrimitive();

    // Finish the 'original' vertex pipeline for all pipeline vertices.
    for(int i=0; i<m_pipelineVerticesCount; i++) {
        m_cgVertexPerspectiveDivide(i);
        m_cgVertexViewportTransform(i);
    }

    // We might have to rasterize more than one primitive.
    for(int i=0; i<pipelinePrimitiveCount; i++) {

        m_cgRasterize(i);
    }

    // Purge vertex list.
    m_pipelineVerticesCount = 0;
}
//---------------------------------------------------------------------------
void CGContext::m_cgVertexPerspectiveDivide(int pipelineVertex)
{
    // We are interested in the last vertex (as always if no vertex index available).
    CGVertexData& v=m_pipelineVertices[pipelineVertex];

    CGVec4 &pos = v.vertexVaryings[CG_POSITION_VARYING];

    // Transform from Clip Space To Normalized Device Coordinates

    // store 1/w_clip for later use by the rasterizer
    // NOTE: 1/w can be linearily interpolated in window space, while w cannot
    pos[W]=1.0f/pos[W];

    // X,Y,Z are euclidean coordinates from now on...
    pos[X]*=pos[W];
    pos[Y]*=pos[W];
    pos[Z]*=pos[W];
}
//---------------------------------------------------------------------------
void CGContext::m_cgVertexViewportTransform(int pipelineVertex)
{
    // We are interested in the last vertex (as always if no vertex index available).
    CGVertexData& v=m_pipelineVertices[pipelineVertex];

    CGVec4 &pos = v.vertexVaryings[CG_POSITION_VARYING];

    // Transform from Normalized Device Coordinates to Window Coordinates
    pos[X]=(pos[X]+1.0f)*0.5f*(float)(m_viewport[2])+(float)m_viewport[0];
    pos[Y]=(pos[Y]+1.0f)*0.5f*(float)(m_viewport[3])+(float)m_viewport[1];
}
