#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Material {
public:
	Material(vec3 _amb = vec3(1.0f), vec3 _diff = vec3(1.0f), vec3 _spec = vec3(0.1f), float _shine = 32.0f);
	~Material() {};

	unsigned int _id;
	
	vec3 getAmbient() const { return ambient; }
	vec3 getDiffuse() const { return diffuse; }
	vec3 getSpecular() const { return specular; }
	float getShininess() const { return shininess; }

	void setAmbient(vec3 _amb) { this->ambient = _amb; }
	void setDiffuse(vec3 _diff) { this->diffuse = _diff; }
	void setSpecular(vec3 _spec) { this->specular = _spec; }
	void setShininess(float _shine) { this->shininess = _shine; }

	void updateAmbient(gl::GlslProgRef shader, vec3 _amb);
	void updateDiffuse(gl::GlslProgRef shader, vec3 _diff);
	void updateSpecular(gl::GlslProgRef shader, vec3 _spec);
	void updateShininess(gl::GlslProgRef shader, float _shine);

	void passDiffuseUf(gl::GlslProgRef shader) const;
	void passSpecularUf(gl::GlslProgRef shader) const;
	void passAmbientUf(gl::GlslProgRef shader) const;
	void passShininessUf(gl::GlslProgRef shader) const;

	void passUniform(gl::GlslProgRef shader, std::string uniformName, vec3 value) const;
	void setupShader(gl::GlslProgRef shader) const;


private:
	vec3			ambient;
	vec3			diffuse;
	vec3			specular;
	float			shininess;

};
