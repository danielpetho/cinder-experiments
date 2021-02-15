#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Perlin.h"
#include "cinder/Color.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Particle {
public:
	Particle(const vec2 &position)
		: mPosition(position), mVelocity(vec2(0)), mZ(0), mRadius(Rand::randFloat(1.0f, 2.0f)) {}

	vec2 mPosition, mVelocity;
	float mZ, mRadius;

	void checkEdges()
	{
		if (mPosition.x < -10)
			mPosition.x = getWindowWidth();

		if (mPosition.x > getWindowWidth() + 10)
			mPosition.x = -10;

		if (mPosition.y < -10)
			mPosition.y = getWindowHeight();

		if (mPosition.y > getWindowHeight() + 10)
			mPosition.y = -10;
	}

	void draw() 
	{
		gl::drawSolidCircle(mPosition, mRadius);
	}
};	

class ParticlesApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	static const int	NUM_PARTICLES = 500;

	Perlin				mPerlin;
	vector<Particle>	mParticles;

	float				mSpeed;
	float				mNoiseTime;
	float				mMaxSpeed;
};



void ParticlesApp::setup()
{
	mPerlin.setSeed(clock());
	mParticles.reserve(NUM_PARTICLES);

	for (int i = 0; i < NUM_PARTICLES; i++)
		mParticles.push_back(Particle(vec2(Rand::randFloat(getWindowWidth()), Rand::randFloat(getWindowHeight()))));

	mMaxSpeed = 5.0f;
	mSpeed = 1.0f;
	mNoiseTime = 0.0f;
}

void ParticlesApp::mouseDown( MouseEvent event )
{
}

void ParticlesApp::update()
{
	mNoiseTime += 1.0f;

	for (auto &particle : mParticles) {

		// Add some perlin noise to the velocity.
		vec3 deriv = mPerlin.dfBm(vec3(particle.mPosition.x, particle.mPosition.y, mNoiseTime));
		particle.mZ = deriv.z;
		vec2 deriv2 = normalize(vec2(deriv.x, deriv.y));
		if (mSpeed > mMaxSpeed) mSpeed = mMaxSpeed;

		particle.mVelocity += deriv2 * mSpeed;
		particle.mVelocity *= 0.9;
		// Move the particles according to their velocities.
		particle.mPosition += particle.mVelocity;
		particle.checkEdges();
	}
}

void ParticlesApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 

	gl::color(Color(255, 255, 255));
	
	for (auto &particle : mParticles)
	{
		particle.draw();
	}
}

CINDER_APP( ParticlesApp, RendererGl )
