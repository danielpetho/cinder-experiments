#include "PointLight.h";

#include <iostream>  
#include <string>  

PointLight::PointLight(vec3 _pos, vec3 _amb, vec3 _diff, vec3 _spec, float _const, float _linear, float _quad)
{
	this->position = _pos;
	this->ambient = _amb;
	this->diffuse = _diff;
	this->specular = _spec;
	this->linear = _linear;
	this->constant = _const;
	this->quadratic = _quad;
}

PointLight::PointLight(vec3 _pos, vec3 _amb, vec3 _diff, vec3 _spec,  float _const, float _linear, float _quad, gl::BatchRef _geom)
{
	this->position = _pos;
	this->ambient = _amb;
	this->diffuse = _diff;
	this->specular = _spec;
	this->geom = _geom;
	this->constant = _const;
	this->linear = _linear;
	this->quadratic = _quad;
}

void PointLight::setupShader(gl::GlslProgRef shader) const
{
	shader->uniform("pointLights[" + to_string(_id) + "].position", this->position);
	shader->uniform("pointLights[" + to_string(_id) + "].ambient", this->ambient);
	shader->uniform("pointLights[" + to_string(_id) + "].diffuse", this->diffuse);
	shader->uniform("pointLights[" + to_string(_id) + "].specular", this->specular);
	shader->uniform("pointLights[" + to_string(_id) + "].constant", this->constant);
	shader->uniform("pointLights[" + to_string(_id) + "].linear", this->linear);
	shader->uniform("pointLights[" + to_string(_id) + "].quadratic", this->quadratic);

}

void PointLight::updatePosition(gl::GlslProgRef shader, vec3 _pos)
{
	this->position = _pos;
	shader->uniform("pointLights[" + to_string(this->_id) + "].position", this->position);
}

void PointLight::updateAmbient(gl::GlslProgRef shader, vec3 _amb)
{
	this->ambient = _amb;
	shader->uniform("pointLights[" + to_string(this->_id) + "].ambient", this->ambient);
}

void PointLight::updateSpecular(gl::GlslProgRef shader, vec3 _spec)
{
	this->specular = _spec;
	shader->uniform("pointLights[" + to_string(this->_id) + "].specular", this->specular);
}

void PointLight::updateDiffuse(gl::GlslProgRef shader, vec3 _diff)
{
	this->diffuse = _diff;
	shader->uniform("pointLights[" + to_string(this->_id) + "].diffuse", this->diffuse);
}

void PointLight::updateConstant(gl::GlslProgRef shader, float _const)
{
	this->constant = _const;
	shader->uniform("pointLights[" + to_string(this->_id) + "].constant", this->constant);
}

void PointLight::updateLinear(gl::GlslProgRef shader, float _lin)
{
	this->linear = _lin;
	shader->uniform("pointLights[" + to_string(this->_id) + "].linear", this->linear);
}

void PointLight::updateQuadratic(gl::GlslProgRef shader, float _quad)
{
	this->quadratic = _quad;
	shader->uniform("pointLights[" + to_string(this->_id) + "].quadratic", this->quadratic);
}

void PointLight::passUniform(gl::GlslProgRef shader, std::string uniformName, vec3 value) const
{
	shader->uniform(uniformName, value);
}

void PointLight::passPositionUf(gl::GlslProgRef shader) const
{
	shader->uniform("pointLights[" + to_string(this->_id) + "].position", this->position);
}

void PointLight::passAmbientUf(gl::GlslProgRef shader) const
{
	shader->uniform("pointLights[" + to_string(this->_id) + "].ambient", this->ambient);
}

void PointLight::passDiffuseUf(gl::GlslProgRef shader) const
{
	shader->uniform("pointLights[" + to_string(this->_id) + "].diffuse", this->diffuse);
}

void PointLight::passSpecularUf(gl::GlslProgRef shader) const
{
	shader->uniform("pointLights[" + to_string(this->_id) + "].specular", this->specular);
}

void PointLight::passConstantUf(gl::GlslProgRef shader) const
{
	shader->uniform("pointLights[" + to_string(this->_id) + "].constant", this->constant);
}

void PointLight::passLinearUf(gl::GlslProgRef shader) const
{
	shader->uniform("pointLights[" + to_string(this->_id) + "].linear", this->linear);
}

void PointLight::passQuadraticUf(gl::GlslProgRef shader) const
{
	shader->uniform("pointLights[" + to_string(this->_id) + "].quadratic", this->quadratic);
}

void PointLight::draw() const
{
	gl::pushModelMatrix();
	gl::translate(this->position);
	this->geom->draw();
	gl::popModelMatrix();

}