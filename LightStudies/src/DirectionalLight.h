#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class DirectionalLight {
public:
	DirectionalLight(vec3 _pos = vec3(0.0f), vec3 _amb = vec3(1.0f), vec3 _diff = vec3(1.0f), vec3 _spec = vec3(1.0f));
	DirectionalLight(vec3 _pos, vec3 _amb, vec3 _diff, vec3 _spec, gl::BatchRef _geom);
	~DirectionalLight() {};

	unsigned int _id;

	void addGeometry(gl::BatchRef _geom) { geom = _geom; }
	
	vec3 getPosition() const { return position; }
	vec3 getAmbient() const { return ambient; }
	vec3 getDiffuse() const { return diffuse; }
	vec3 getSpecular() const { return specular; }

	void setPosition(vec3 _pos) { this->position = _pos; }
	void setAmbient(vec3 _amb) { this->ambient = _amb; }
	void setDiffuse(vec3 _diff) { this->diffuse = _diff; }
	void setSpecular(vec3 _spec) { this->specular = _spec; }

	void updatePosition(gl::GlslProgRef shader, vec3 _pos);
	void updateAmbient(gl::GlslProgRef shader, vec3 _amb);
	void updateDiffuse(gl::GlslProgRef shader, vec3 _diff);
	void updateSpecular(gl::GlslProgRef shader, vec3 _spec);

	void passPositionUf(gl::GlslProgRef shader) const;
	void passDiffuseUf(gl::GlslProgRef shader) const;
	void passSpecularUf(gl::GlslProgRef shader) const;
	void passAmbientUf(gl::GlslProgRef shader) const;

	void passUniform(gl::GlslProgRef shader, std::string uniformName, vec3 value) const;
	void draw() const;
	void setupShader(gl::GlslProgRef shader) const;


private:
	vec3			position;

	vec3			ambient;
	vec3			diffuse;
	vec3			specular;

	gl::BatchRef	geom;
};