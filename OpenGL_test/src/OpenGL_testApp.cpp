#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"
#include "cinder/Easing.h"

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

	int				mCurrentFrame;
};

void OpenGL_testApp::setup()
{
	auto lambert = gl::ShaderDef().lambert().color();
	gl::GlslProgRef	shader = gl::getStockShader(lambert);

	for (int i = 0; i < NUM_SLICES; ++i) {
		float rel = i / (float)NUM_SLICES;
		float sliceHeight = 1.0f / NUM_SLICES;
		auto slice = geom::Cube().size(1, sliceHeight, 1);
		auto trans = geom::Translate(0, rel * 1.5f, 0);
		auto color = geom::Constant(geom::COLOR,
			Color(CM_HSV, 266.0f / 360.0f, 1, rel));
		mSlices[i] = gl::Batch::create(slice >> trans >> color,
			shader);
	}


	mCam.lookAt(vec3(2, 2, 3), vec3(0, 0.5f, 0));
}

void OpenGL_testApp::mouseDown(MouseEvent event)
{
	// pull down the current window as a surface and pass it to writeImage
	writeImage(getHomeDirectory() / "cinder" / "saveImage_" / (toString(mCurrentFrame) + ".png"), copyWindowSurface());
	mCurrentFrame++;
}

void OpenGL_testApp::draw()
{
	gl::clear();
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
}

CINDER_APP( OpenGL_testApp, RendererGl, [](App::Settings *settings) {
	settings->setWindowSize(700, 700);
	settings->setMultiTouchEnabled(false);
})
