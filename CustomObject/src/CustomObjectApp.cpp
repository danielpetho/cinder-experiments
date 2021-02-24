#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Utilities.h"
#include "cinder/ObjLoader.h" 
#include "cinder/CameraUi.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Batch.h"




using namespace ci;
using namespace ci::app;
using namespace std;

class CustomObjectApp : public App {
  public:

	  void setup() override;
	  void mouseDown(MouseEvent event) override;
	  void mouseDrag(MouseEvent event) override;
	  void mouseWheel(MouseEvent event) override;
	  void keyDown(KeyEvent event) override;
	  void update() override;
	  void draw() override;

	  gl::FboRef		mOffScreenFbo;
	  unsigned int    mCurrentFrame;

	  // specify the poster number
	  unsigned int	mPosterId = 0000;

	  // off screen buffer resolution
	  unsigned int	mWidth = 1920;
	  unsigned int	mHeight = 1080;
	  float			mScale = 1;

	  // screen resolution
	  bool			mShowScreen;
	  static void prepareSettings(Settings *settings) {
		  settings->setWindowSize(1920 / 3, 1080 / 3);
	  };

	  // recording setups
	  unsigned int	mEndFrame = 900;
	  unsigned int	mRenderedFrame = 0;
	  bool			mRender = false;
	  std::string	renderPath = "D:/Projects/creative-coding/nonzeroexitcode/post_processing/" + toString(mPosterId) + "/render/";

	  // --------------------------------

private:
	void createGrid(int size);
	void createWireShader();
	void createWireframeShader();
	void loadGeomSource(const geom::Source &source);


	CameraUi					mCamUi;
	CameraPersp					mCam;
	
	gl::BatchRef				mHandObjRef;
	gl::BatchRef				mPrimitiveWireframe;
	gl::VertBatchRef			mGrid;

	gl::GlslProgRef				mPhongShader;
	gl::GlslProgRef				mWireShader;
	gl::GlslProgRef				mWireframeShader;

	bool						mShowGrid = true;


};

void CustomObjectApp::setup()
{
	mOffScreenFbo = gl::Fbo::create(mWidth * mScale, mHeight * mScale);
	mOffScreenFbo->bindFramebuffer();

	mCurrentFrame = 0;
	
	mCam.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 10000.0f);
	mCam.lookAt(vec3(0, 20, 20), vec3(0));
	mCamUi = CameraUi(&mCam);

	createGrid(30);
	createWireframeShader();
	createWireShader();

	ObjLoader loader(loadFile("C:/Users/Daniel/cinder-experiments/hand.obj"));
	mHandObjRef = gl::Batch::create(loader, gl::getStockShader(gl::ShaderDef().lambert().color()));

	// --------- ????
	TriMesh::Format fmt = TriMesh::Format().positions().normals().texCoords().tangents();
	fmt.colors();
	TriMesh mesh(loader, fmt);

	mPrimitiveWireframe = gl::Batch::create(mesh, mWireframeShader);

	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void CustomObjectApp::loadGeomSource(const geom::Source &source)
{

	
}

void CustomObjectApp::createGrid(int size)
{
	mGrid = gl::VertBatch::create(GL_LINES);
	mGrid->begin(GL_LINES);
	for (int i = -size; i <= size; ++i) {
		mGrid->color(Color(0.25f, 0.25f, 0.25f));
		mGrid->color(Color(0.25f, 0.25f, 0.25f));
		mGrid->color(Color(0.25f, 0.25f, 0.25f));
		mGrid->color(Color(0.25f, 0.25f, 0.25f));

		mGrid->vertex(float(i), 0.0f, -size);
		mGrid->vertex(float(i), 0.0f, size);
		mGrid->vertex(-size, 0.0f, float(i));
		mGrid->vertex(+size, 0.0f, float(i));
	}
	mGrid->end();
}

void CustomObjectApp::mouseDown( MouseEvent event )
{
	mCamUi.mouseDown(event);
}

void CustomObjectApp::mouseDrag(MouseEvent event)
{
	mCamUi.mouseDrag(event);
}

void CustomObjectApp::mouseWheel(MouseEvent event)
{
	mCamUi.mouseWheel(event);
}

void CustomObjectApp::createWireShader()
{
	try {
		mWireShader = gl::context()->getStockShader(gl::ShaderDef().color());
	}
	catch (Exception &exc) {
		console() << "error loading wire shader: " << exc.what() << endl;;
	}
}

void CustomObjectApp::createWireframeShader()
{
#if ! defined( CINDER_GL_ES )
	try {
		auto format = gl::GlslProg::Format()
			.vertex(loadAsset("wireframe.vert"))
			.geometry(loadAsset("wireframe.geom"))
			.fragment(loadAsset("wireframe.frag"));

		mWireframeShader = gl::GlslProg::create(format);
	}
	catch (Exception &exc) {
		console() << "error loading wire shader: " << exc.what() << endl;;
	}
#endif // ! defined( CINDER_GL_ES )
}

void CustomObjectApp::keyDown(KeyEvent event)
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

	if (event.getChar() == 'g')
	{
		mShowGrid = !mShowGrid;
		console() << "grid showing: " << mShowGrid << std::endl;
	}
}
void CustomObjectApp::update()
{
	
}

void CustomObjectApp::draw()
{
	gl::clear(Color(0, 0, 0));
	gl::ScopedViewport viewport(vec2(0), mOffScreenFbo->getSize());
	gl::setMatrices(mCam);

	gl::ScopedColor colorScope(Color(1, 1, 1));
	gl::ScopedBlendAlpha blendScope;
	gl::ScopedFaceCulling cullScope(true, GL_FRONT);

	mWireframeShader->uniform("uBrightness", 0.5f);
	mPrimitiveWireframe->draw();

	// Now render the front side.
	gl::cullFace(GL_BACK);

	mWireframeShader->uniform("uBrightness", 1.0f);
	//mPrimitiveWireframe->draw();

	// Draw the grid.
	if (mShowGrid && mGrid) {
		gl::ScopedGlslProg scopedGlslProg(gl::context()->getStockShader(gl::ShaderDef().color()));

		mGrid->draw();

		// draw the coordinate frame with length 2.
		gl::drawCoordinateFrame(2);
	}

	mHandObjRef->draw();


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

CINDER_APP( CustomObjectApp, RendererGl, CustomObjectApp::prepareSettings)
