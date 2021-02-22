#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class NoiseShaderApp : public App {
  public:

	  void setup() override;
	  void mouseDown(MouseEvent event) override;
	  void keyDown(KeyEvent event) override;
	  void update() override;
	  void draw() override;

	  CameraPersp		mCam;

	  gl::FboRef		mOffScreenFbo;
	  unsigned int		mCurrentFrame;

	  // specify the poster number
	  unsigned int	mPosterId = 185;

	  // off screen buffer resolution
	  unsigned int	mWidth = 1440;
	  unsigned int	mHeight = 2160;
	  float			mScale = 1;

	  // screen resolution
	  bool			mShowScreen;
	  static void prepareSettings(Settings *settings) {
		  settings->setWindowSize(1440 / 3, 2160 / 3);
	  };

	  // recording setups
	  unsigned int	mEndFrame = 800;
	  unsigned int	mRenderedFrame = 0;
	  bool			mRender = false;
	  std::string	renderPath = "D:/Projects/creative-coding/nonzeroexitcode/post_processing/" + toString(mPosterId) + "/render/";

	  // ----------------------
	  gl::BatchRef			mPlane;
	  gl::GlslProgRef		mGlsl;
	  mat4					mPlaneRotation;
};

void NoiseShaderApp::keyDown(KeyEvent event)
{
	if (event.getChar() == 'r')
	{
		mRender = !mRender;
		console() << "rendering: " << mRender << std::endl;
	}

	if (event.getChar() == 's')
	{
		writeImage(renderPath + (toString(mCurrentFrame) + ".png"), mOffScreenFbo->getColorTexture()->createSource());
		console() << "saved frame: " << mCurrentFrame << std::endl;
	}

	if (event.getChar() == 'i')
	{
		mShowScreen = !mShowScreen;
		console() << "screen showing: " << mShowScreen << std::endl;
	}

}

void NoiseShaderApp::setup()
{
	mOffScreenFbo = gl::Fbo::create(mWidth * mScale, mHeight * mScale);
	mOffScreenFbo->bindFramebuffer();

	mCurrentFrame = 0;


	mCam.lookAt(vec3(3, 2, 3), vec3(0));

	// setup shader
	try {
		mGlsl = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("shader.vert"))
			.fragment(loadAsset("shader.frag")));
	}
	catch (gl::GlslProgCompileExc ex) {
		console() << ex.what() << endl;
		quit();
	}

	auto plane = geom::Plane().subdivisions(ivec2(500));
	mPlane = gl::Batch::create(plane, mGlsl);

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void NoiseShaderApp::mouseDown( MouseEvent event )
{
}

void NoiseShaderApp::update()
{
	mPlaneRotation *= rotate(toRadians(0.2f), normalize(vec3(0, 1, 0)));

}

void NoiseShaderApp::draw()
{
	gl::clear(Color(0, 0, 0));

	gl::clear();
	gl::ScopedViewport viewport(vec2(0), mOffScreenFbo->getSize());

	gl::setMatrices(mCam);

	mGlsl->uniform("uResolution", vec2(mWidth, mHeight));

	gl::ScopedGlslProg glslProg(mGlsl);
	mGlsl->uniform("uTime", (float)mCurrentFrame);

	gl::ScopedModelMatrix modelScope;
	gl::multModelMatrix(mPlaneRotation);

	
	mPlane->draw();

	if (mRender && mRenderedFrame < mEndFrame)
	{
		writeImage(renderPath + (toString(mRenderedFrame) + ".png"), mOffScreenFbo->getColorTexture()->createSource());
		mRenderedFrame++;

		if (mRenderedFrame % 100 == 0) console() << "rendered: " << mRenderedFrame << std::endl;
	}

	if (mShowScreen)
	{
		mOffScreenFbo->blitToScreen(mOffScreenFbo->getBounds(), getWindowBounds());
	}


	mCurrentFrame++;
}

CINDER_APP( NoiseShaderApp, RendererGl, NoiseShaderApp::prepareSettings)
