
/* CGLightSource.cpp<++>
 * @Author:      Sascha Ebert
 * @License:     GPL
 * @Created:     2013-01-27.
 * @Editor:      Vim
 */

#include "CGLightSource.h"


//---------------------------------------------------------------------------
/// Constructor (should default all values.
CGLightSource::CGLightSource(void)
{
    this->setDefaultParameters();
}
//---------------------------------------------------------------------------
/// Destructor.
CGLightSource::~CGLightSource(void){}
//---------------------------------------------------------------------------
/// Reset all values to the default (GL) light parameters.
void CGLightSource::setDefaultParameters()
{
    this->setColorAmbient(0, 0, 0, 1);
    this->setColorDiffuse(1, 1, 1, 1);
    this->setColorSpecular(1, 1, 1, 1);

    float def_pos[4] = {0, 0, 1, 0};
    this->setPosition(def_pos);

    float def_spot_dir[4] = {0, 0, -1, 0};
    this->setSpotDirection(def_spot_dir);

    this->setSpotExponent(0);
    this->setSpotCutoff(180);
    this->setConstantAttenuation(1);
    this->setLinearAttenuation(0);
    this->setQuadraticAttenuation(0);
}
//---------------------------------------------------------------------------

/// Set the ambient component color.
void CGLightSource::setColorAmbient (float r, float g, float b, float a)
{
    this->ambientColor[R] = r;
    this->ambientColor[G] = g;
    this->ambientColor[B] = b;
    this->ambientColor[A] = a;
}
//---------------------------------------------------------------------------
/// Set the diffuse component color.
void CGLightSource::setColorDiffuse (float r, float g, float b, float a)
{
    this->diffuseColor[R] = r;
    this->diffuseColor[G] = g;
    this->diffuseColor[B] = b;
    this->diffuseColor[A] = a;
}
//---------------------------------------------------------------------------
/// Set the specular component color.
void CGLightSource::setColorSpecular(float r, float g, float b, float a)
{
    this->specularColor[R] = r;
    this->specularColor[G] = g;
    this->specularColor[B] = b;
    this->specularColor[A] = a;
}
//---------------------------------------------------------------------------
/// Set the light position (float[4]).
void CGLightSource::setPosition(const float * pos)
{
    this->position[X] = pos[X];
    this->position[Y] = pos[Y];
    this->position[Z] = pos[Z];
    this->position[W] = pos[W];
}
//---------------------------------------------------------------------------
/// Set the spot direction (float[4]).
void CGLightSource::setSpotDirection(const float * spotDir)
{
    this->spotDirection[X] = spotDir[X];
    this->spotDirection[Y] = spotDir[Y];
    this->spotDirection[Z] = spotDir[Z];
    this->spotDirection[W] = spotDir[W];
}
//---------------------------------------------------------------------------
/// Set the spot exponent.
void CGLightSource::setSpotExponent (float exponent)
{
    this->spotExponent = exponent;
}
//---------------------------------------------------------------------------
/// Set the spot cutoff.
/// Hint: store the cosine of the cutoff to avoid calculation in the vertex program
void CGLightSource::setSpotCutoff(float cutoff)
{
    this->spotCutoff = cos(cutoff);
}
//---------------------------------------------------------------------------
/// Set the constant attenuation part.
void CGLightSource::setConstantAttenuation   (float attenuation)
{
    this->constantAttenuation = attenuation;
}
//---------------------------------------------------------------------------
/// Set the linear attenuation part.
void CGLightSource::setLinearAttenuation(float attenuation)
{
    this->linearAttenuation = attenuation;
}
//---------------------------------------------------------------------------
/// Set the quadratic attenuation part.
void CGLightSource::setQuadraticAttenuation (float attenuation)
{
    this->quadraticAttenuation = attenuation;
}
//---------------------------------------------------------------------------
/// Set all attenuation parts as float[3] array [constant,linear,quadratic].
void CGLightSource::setAttenuation   (const float * attenuation)
{
    this->constantAttenuation = attenuation[0];
    this->linearAttenuation = attenuation[1];
    this->quadraticAttenuation = attenuation[2];
}
//---------------------------------------------------------------------------
/// Set the uniforms in the given context to the values of the light source.
void CGLightSource::setupUniforms(CGContext* context)
{
    context->cgUniform4fv(CG_ULOC_LIGHT0_AMBIENT, 1, this->ambientColor);
    context->cgUniform4fv(CG_ULOC_LIGHT0_DIFFUSE, 1, this->diffuseColor);
    context->cgUniform4fv(CG_ULOC_LIGHT0_SPECULAR, 1, this->specularColor);
    context->cgUniform4fv(CG_ULOC_LIGHT0_POSITION, 1, this->position);
    context->cgUniform4fv(CG_ULOC_LIGHT0_SPOT_DIRECTION, 1, this->spotDirection);

    context->cgUniform1fv(CG_ULOC_LIGHT0_CONSTANT_ATTENUATION, 1, &(this->constantAttenuation));
    context->cgUniform1fv(CG_ULOC_LIGHT0_LINEAR_ATTENUATION, 1, &(this->linearAttenuation));
    context->cgUniform1fv(CG_ULOC_LIGHT0_QUADRATIC_ATTENUATION, 1, &(this->quadraticAttenuation));

    context->cgUniform1fv(CG_ULOC_LIGHT0_SPOT_EXPONENT, 1, &(this->spotExponent));
    context->cgUniform1fv(CG_ULOC_LIGHT0_SPOT_CUTOFF, 1, &(this->spotCutoff));
}
