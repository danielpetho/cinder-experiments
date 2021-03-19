#include "Material.h"

#include <iostream>  
#include <string>  

Material::Material(vec3 _amb, vec3 _diff, vec3 _spec, float _shine)
{
	this->ambient = _amb;
	this->diffuse = _diff;
	this->specular = _spec;
	this->shininess = _shine;
}


void Material::setupShader(gl::GlslProgRef shader) const
{
	shader->uniform("material.ambient", this->ambient);
	shader->uniform("material.diffuse", this->diffuse);
	shader->uniform("material.specular", this->specular);
	shader->uniform("material.shininess", this->shininess);
}


void Material::updateAmbient(gl::GlslProgRef shader, vec3 _amb)
{
	this->ambient = _amb;
	shader->uniform("material.ambient", this->ambient);
}

void Material::updateSpecular(gl::GlslProgRef shader, vec3 _spec)
{
	this->specular = _spec;
	shader->uniform("material.specular", this->specular);
}

void Material::updateDiffuse(gl::GlslProgRef shader, vec3 _diff)
{
	this->diffuse = _diff;
	shader->uniform("material.diffuse", this->diffuse);
}

void Material::updateShininess(gl::GlslProgRef shader, float _shine)
{
	this->shininess = _shine;
	shader->uniform("material.shininess", this->shininess);
}

void Material::passUniform(gl::GlslProgRef shader, std::string uniformName, vec3 value) const
{
	shader->uniform(uniformName, value);
}

void Material::passAmbientUf(gl::GlslProgRef shader) const
{
	shader->uniform("material.shininess", this->ambient);
}

void Material::passDiffuseUf(gl::GlslProgRef shader) const
{
	shader->uniform("material.diffuse", this->diffuse);
}

void Material::passSpecularUf(gl::GlslProgRef shader) const
{
	shader->uniform("material.specular", this->specular);
}

void Material::passShininessUf(gl::GlslProgRef shader) const
{
	shader->uniform("material.shininess", this->shininess);
}
