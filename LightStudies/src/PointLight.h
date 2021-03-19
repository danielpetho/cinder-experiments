#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PointLight {
public:
	PointLight(
		vec3 _pos = vec3(0.0f), 
		vec3 _amb = vec3(1.0f), 
		vec3 _diff = vec3(1.0f), 
		vec3 _spec = vec3(1.0f), 
		float _const = 1.0f, 
		float _linear = 0.09f, 
		float _quad = 0.032f);

	PointLight(
		vec3 _pos, vec3 _amb, vec3 _diff, vec3 _spec, 
		float _const, float _linear, float _quad, 
		gl::BatchRef _geom);

	~PointLight() {};

	unsigned int _id;

	void addGeometry(gl::BatchRef _geom) { geom = _geom; }
	
	vec3 getPosition() const { return position; }
	vec3 getAmbient() const { return ambient; }
	vec3 getDiffuse() const { return diffuse; }
	vec3 getSpecular() const { return specular; }
	float getConstant() const { return constant; }
	float getLinear() const { return linear; }
	float getQuadratic() const { return quadratic; }


	void setPosition(vec3 _pos) { this->position = _pos; }
	void setAmbient(vec3 _amb) { this->ambient = _amb; }
	void setDiffuse(vec3 _diff) { this->diffuse = _diff; }
	void setSpecular(vec3 _spec) { this->specular = _spec; }
	void setConstant(float _const) { this->constant = _const; }
	void setLinear(float _linear) { this->linear = _linear; }
	void setQuadratic(float _quad) { this->quadratic = _quad; }

	void updatePosition(gl::GlslProgRef shader, vec3 _pos);
	void updateAmbient(gl::GlslProgRef shader, vec3 _amb);
	void updateDiffuse(gl::GlslProgRef shader, vec3 _diff);
	void updateSpecular(gl::GlslProgRef shader, vec3 _spec);
	void updateLinear(gl::GlslProgRef shader, float _lin);
	void updateConstant(gl::GlslProgRef shader, float _const);
	void updateQuadratic(gl::GlslProgRef shader, float _quad);

	void passPositionUf(gl::GlslProgRef shader) const;
	void passDiffuseUf(gl::GlslProgRef shader) const;
	void passSpecularUf(gl::GlslProgRef shader) const;
	void passAmbientUf(gl::GlslProgRef shader) const;
	void passConstantUf(gl::GlslProgRef shader) const;
	void passLinearUf(gl::GlslProgRef shader) const;
	void passQuadraticUf(gl::GlslProgRef shader) const;

	void passUniform(gl::GlslProgRef shader, std::string uniformName, vec3 value) const;
	void draw() const;
	void setupShader(gl::GlslProgRef shader) const;


private:
	vec3			position;

	vec3			ambient;
	vec3			diffuse;
	vec3			specular;

	float			constant;
	float			linear;
	float			quadratic;

	gl::BatchRef	geom;
};
