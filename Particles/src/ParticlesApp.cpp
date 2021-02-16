#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Vector.h"
#include "cinder/Perlin.h"
#include "cinder/Color.h"
#include "cinder/params/Params.h"
#include "cinder/cairo/Cairo.h"


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
	  

	void		setup() override;
	void		mouseDown( MouseEvent event ) override;
	void		keyDown(KeyEvent event) override;
	void		update() override;
	void		draw() override;

	void drawVectorField();
	params::InterfaceGlRef	mParams;

	static const int	NUM_PARTICLES = 500;

	Perlin				mPerlin;
	vector<Particle>	mParticles;

	float				mSpeed;
	float				mNoiseTime;
	float				mFrequency;
	float				mAnimationSpeed;

	bool				mDrawVectorField;
};

void prepareSettings(ParticlesApp::Settings *settings)
{
	settings->setResizable(false);
	settings->setWindowSize(vec2(1080, 540));
}

void ParticlesApp::setup()
{

	mPerlin.setSeed(clock());
	mParticles.reserve(NUM_PARTICLES);

	for (int i = 0; i < NUM_PARTICLES; i++)
		mParticles.push_back(Particle(vec2(Rand::randFloat(getWindowWidth()), Rand::randFloat(getWindowHeight()))));

	mSpeed = 2.0f;
	mNoiseTime = 0.0f;
	mFrequency = 0.01f;
	mAnimationSpeed = 1.0f;
	mDrawVectorField = true;

	mParams = params::InterfaceGl::create("Parameters", ivec2(175, 100));
	mParams->addParam("Frequency", &mFrequency, "min=0.0001 max=0.1 step=0.0005 keyIncr=f keyDecr=F");
	mParams->addParam("Animation speed", &mAnimationSpeed, "min=0.5 max=10.0 step=0.5 keyIncr=s keyDecr=S");
}

void ParticlesApp::mouseDown( MouseEvent event )
{
}

void ParticlesApp::keyDown(KeyEvent event)
{
	if (event.getChar() == 'l') {
		mDrawVectorField = !mDrawVectorField;
	}
}

void ParticlesApp::drawVectorField()
{

	gl::begin(GL_LINES);
	for (int y = 0; y < getWindowHeight(); y += 20) {
		for (int x = 0; x < getWindowWidth(); x += 20) {
			vec3 deriv = mPerlin.dfBm(vec3(x, y, mNoiseTime) * mFrequency)  * 15.0f;
			vec2 norm = normalize(vec2(deriv.x, deriv.y));
			
			if (length2(vec2(deriv.x, deriv.y)) > 0) {
				gl::vertex(vec2(x, y));
				gl::vertex(vec2(x, y) + vec2(norm.x, norm.y) * 15.0f);
			}	
		}
	}
	gl::end();
}


void ParticlesApp::update()
{
	mNoiseTime += mAnimationSpeed;

	for (auto &particle : mParticles) {

		vec3 deriv = (mPerlin.dfBm(vec3(particle.mPosition.x, particle.mPosition.y, mNoiseTime) * mFrequency));
		particle.mZ = deriv.z;
		vec2 deriv2 = normalize(vec2(deriv.x, deriv.y));

		particle.mVelocity += deriv2 * mSpeed;
		particle.mVelocity *= 0.9;
		
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

	gl::color(Color(0, 0, 255));
	if (mDrawVectorField) drawVectorField();

	mParams->draw();
}

CINDER_APP( ParticlesApp, RendererGl, prepareSettings )
