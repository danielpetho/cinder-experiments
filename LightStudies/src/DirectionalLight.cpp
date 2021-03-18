#include "DirectionalLight.h"

#include <iostream>  
#include <string>  

DirectionalLight::DirectionalLight(vec3 _pos, vec3 _amb, vec3 _diff, vec3 _spec)
{
	this->position = _pos;
	this->ambient = _amb;
	this->diffuse = _diff;
	this->specular = _spec;
}

DirectionalLight::DirectionalLight(vec3 _pos, vec3 _amb, vec3 _diff, vec3 _spec, gl::BatchRef _geom)
{
	this->position = _pos;
	this->ambient = _amb;
	this->diffuse = _diff;
	this->specular = _spec;
	this->geom = _geom;
}

void DirectionalLight::setupShader(gl::GlslProgRef shader) const
{
	shader->uniform("dirLights[" + to_string(_id) + "].direction", this->position);
	shader->uniform("dirLights[" + to_string(_id) + "].ambient", this->ambient);
	shader->uniform("dirLights[" + to_string(_id) + "].diffuse", this->diffuse);
	shader->uniform("dirLights[" + to_string(_id) + "].specular", this->specular);
}

void DirectionalLight::updatePosition(gl::GlslProgRef shader, vec3 _pos)
{
	this->position = _pos;
	shader->uniform("dirLights[" + to_string(this->_id) + "].direction", this->position);
}

void DirectionalLight::updateAmbient(gl::GlslProgRef shader, vec3 _amb)
{
	this->ambient = _amb;
	shader->uniform("dirLights[" + to_string(this->_id) + "].ambient", this->ambient);
}

void DirectionalLight::updateSpecular(gl::GlslProgRef shader, vec3 _spec)
{
	this->specular = _spec;
	shader->uniform("dirLights[" + to_string(this->_id) + "].specular", this->specular);
}

void DirectionalLight::updateDiffuse(gl::GlslProgRef shader, vec3 _diff)
{
	this->diffuse = _diff;
	shader->uniform("dirLights[" + to_string(this->_id) + "].diffuse", this->diffuse);
}

void DirectionalLight::passUniform(gl::GlslProgRef shader, std::string uniformName, vec3 value) const
{
	shader->uniform(uniformName, value);
}

void DirectionalLight::passPositionUf(gl::GlslProgRef shader) const
{
	shader->uniform("dirLights[" + to_string(this->_id) + "].direction", this->position);
}

void DirectionalLight::passAmbientUf(gl::GlslProgRef shader) const
{
	shader->uniform("dirLights[" + to_string(this->_id) + "].ambient", this->ambient);
}

void DirectionalLight::passDiffuseUf(gl::GlslProgRef shader) const
{
	shader->uniform("dirLights[" + to_string(this->_id) + "].diffuse", this->diffuse);
}

void DirectionalLight::passSpecularUf(gl::GlslProgRef shader) const
{
	shader->uniform("dirLights[" + to_string(this->_id) + "].specular", this->specular);
}

void DirectionalLight::draw() const
{
	gl::pushModelMatrix();
	gl::translate(this->position);
	this->geom->draw();
	gl::popModelMatrix();

}