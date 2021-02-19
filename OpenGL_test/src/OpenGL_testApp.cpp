#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Easing.h"
#include "cinder/gl/Fbo.h"
#include "cinder/ImageIo.h"


#include "Windows.h"



using namespace ci;
using namespace ci::app;
using namespace std;

class OpenGL_testApp : public App {
public:
	void setup() override;
	void draw() override;
	void mouseDown(MouseEvent event);


	static const int NUM_SLICES = 12;

	CameraPersp		mCam;
	gl::BatchRef	mSlices[NUM_SLICES];

	unsigned int    mCurrentFrame;

	gl::FboRef myFbo = gl::Fbo::create(1440, 2160);
};

void OpenGL_testApp::setup()
{

	myFbo->bindFramebuffer();
	auto lambert = gl::ShaderDef().lambert().color();
	gl::GlslProgRef	shader = gl::getStockShader(lambert);
	
	

	for (int i = 0; i < NUM_SLICES; ++i) {
		float rel = i / (float)NUM_SLICES;
		float sliceHeight = 1.0f / NUM_SLICES;
		auto slice = geom::Cube().size(2, sliceHeight, 2);
		auto trans = geom::Translate(0, rel * 2.0f, 0);
		auto color = geom::Constant(geom::COLOR,
			Color(CM_HSV, 266.0f / 360.0f, 1, rel * 1.0f));
		mSlices[i] = gl::Batch::create(slice >> trans >> color,
			shader);
	}

	mCurrentFrame = 0;
	
	

}

void OpenGL_testApp::mouseDown(MouseEvent event)
{
	// pull down the current window as a surface and pass it to writeImage
	writeImage(getHomeDirectory() / "cinder" / "saveImage_" / (toString(mCurrentFrame) + ".png"), myFbo->getColorTexture()->createSource());
	mCurrentFrame++;
}

void OpenGL_testApp::draw()
{
	
	
	gl::clear();

	mCam.lookAt(vec3(3, 3, 5), vec3(0, 0.65, 0));

	//gl::ScopedMatrices matrices;
	//gl::setMatricesWindowPersp(myFbo->getSize(), 60.0f, 1.0f, 1000.0f, true);
	gl::ScopedViewport viewport(vec2(0), myFbo->getSize());
	

	gl::enableDepthRead();
	gl::enableDepthWrite();
	
	gl::setMatrices(mCam);

	const float delay = 0.1f;
	// time in seconds for one slice to rotate
	const float rotationTime = 3.0f;
	// time in seconds to delay each slice's rotation
	const float rotationOffset = 0.2f; // seconds
									   // total time for entire animation
	const float totalTime = delay + rotationTime +
		NUM_SLICES * rotationOffset;

	// loop every 'totalTime' seconds
	float time = fmod(getElapsedFrames() / 30.0f, totalTime);

	
	for (int i = 0; i < NUM_SLICES; ++i) {
		// animates from 0->1
		float rotation = 0;
		// when does the slice begin rotating
		float startTime = i * rotationOffset;
		// when does it complete
		float endTime = startTime + rotationTime;
		// are we in the middle of our time section?
		if (time > startTime && time < endTime)
			rotation = (time - startTime) / rotationTime;
		// ease fn on rotation, then convert to radians
		float angle = easeInOutQuint(rotation) * M_PI / 2.0f;

		gl::ScopedModelMatrix scpModelMtx;
		gl::rotate(angleAxis(angle, vec3(0, 1, 0)));
		
		mSlices[i]->draw();

	}

	//writeImage(getHomeDirectory() / "cinder" / "saveImage_" / (toString(mCurrentFrame) + ".png"), myFbo->getColorTexture()->createSource());
	//gl::Texture2dRef screenTexture = myFbo->getColorTexture();
	//writeImage(getHomeDirectory() / "cinder" / "saveImage_" / (toString(mCurrentFrame) + ".png"), screenTexture->createSource());

	myFbo->blitToScreen(myFbo->getBounds(), getWindowBounds());

	mCurrentFrame++;
}

CINDER_APP( OpenGL_testApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(1440 / 3, 2160 / 3);
})
