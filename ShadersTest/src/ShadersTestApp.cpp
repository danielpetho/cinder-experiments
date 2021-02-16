#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ShadersTestApp : public App {
  public:
	  void	setup() override;
	  void	draw() override;

	  CameraPersp			mCam;
	  gl::BatchRef		mRect;
	  gl::GlslProgRef		mGlsl;
};

void ShadersTestApp::setup()
{
	mCam.lookAt(vec3(3, 2, 3), vec3(0));

	// setup shader
	try {
		mGlsl = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("basic.vert"))
			.fragment(loadAsset("basic.frag")));
	}
	catch (gl::GlslProgCompileExc ex) {
		cout << ex.what() << endl;
		quit();
	}

	auto plane = geom::Plane().subdivisions(ivec2(30));
	mRect = gl::Batch::create(plane, mGlsl);

	gl::enableDepthWrite();
	gl::enableDepthRead();
}



void ShadersTestApp::draw()
{
	gl::clear();
	gl::setMatrices(mCam);

	mGlsl->uniform("uCheckSize", 30.0f);

	gl::ScopedGlslProg glslProg(mGlsl);
	console() << getElapsedFrames() << std::endl;

	uint32_t d = getElapsedFrames();
	float f = *(float*)&d;
	mGlsl->uniform("u_time", f);
	mRect->draw();
}

CINDER_APP( ShadersTestApp, RendererGl )
