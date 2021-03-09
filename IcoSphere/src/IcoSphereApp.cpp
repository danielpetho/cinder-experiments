#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Utilities.h"
#include "cinder/CameraUi.h"
#include "cinder/Perlin.h"
#include "MSphere.h";

using namespace ci;
using namespace ci::app;
using namespace std;

const unsigned int FBO_WIDTH = 1440;
const unsigned int FBO_HEIGHT = 2160;
const unsigned int SCREEN_DOWNSCALE = 3;

// specify the poster number
const unsigned int POSTER_ID = 198;

class IcoSphereApp : public App {
public:


	void setup() override;
	void mouseDown(MouseEvent event) override;
	void mouseDrag(MouseEvent event) override;
	void mouseWheel(MouseEvent event) override;
	void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;

	static void prepareSettings(Settings *settings) {
		settings->setWindowSize(FBO_WIDTH / SCREEN_DOWNSCALE, FBO_HEIGHT / SCREEN_DOWNSCALE);
	};

private:
	void createGrid(int size);

	CameraPersp		mCam;
	CameraUi		mCamUi;


	gl::FboRef		mOffScreenFbo;
	unsigned int    mCurrentFrame;

	// off screen buffer resolution
	unsigned int	mWidth = FBO_WIDTH;
	unsigned int	mHeight = FBO_HEIGHT;
	float			mScale = 1;

	// utilities
	bool			mShowScreen = true;
	bool			mShowGrid = true;
	bool			mAttachCameraUi = true;
	bool			mRender = false;

	gl::VertBatchRef	mGrid;
	unsigned int		mGridSize = 30;

	// recording setups
	unsigned int	mEndFrame = 1000;
	unsigned int	mRenderedFrame = 0;
	std::string		renderPath = "D:/Projects/creative-coding/nonzeroexitcode/post_processing/" + toString(POSTER_ID) + "/render/";

	//--------------

	MSphere			mSphere;
	gl::VboRef		mSphereVbo;
	gl::VaoRef		mSphereVao;

	gl::GlslProgRef mRenderProg;


	Perlin			perlin;

};

void IcoSphereApp::setup()
{
	mOffScreenFbo = gl::Fbo::create(mWidth * mScale, mHeight * mScale);
	mOffScreenFbo->bindFramebuffer();
	//createGrid(mGridSize);

	mCam.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 10000.0f);
	mCam.lookAt(vec3(0, 20, 20), vec3(0));
	mCamUi = CameraUi(&mCam);

	try {
		mRenderProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("shader.vert"))
			.fragment(loadAsset("shader.frag")));
	}
	catch (gl::GlslProgCompileExc e) {
		ci::app::console() << e.what() << std::endl;
		quit();
	}

	mSphere = MSphere(100, 50, 50, 8);
	
	std::vector<float> mSphereVertexArray = mSphere.getVertexArray();

	/*for (auto vertex : mSphereVertexArray) {
		ci::app::console() << vertex << std::endl;
	}*/
	const float *mSphereVertices = mSphere.getVertices();
	unsigned int mSpherePointsSize = mSphere.getVertexSize();

	mSphereVao = gl::Vao::create();
	mSphereVbo = gl::Vbo::create(GL_ARRAY_BUFFER, mSphereVertexArray, GL_DYNAMIC_DRAW);

	gl::ScopedVao			vao_(mSphereVao);
	gl::ScopedBuffer		vbo_(mSphereVbo);

	size_t stride = 3 * sizeof(float);
	GLuint positionLocation = 0;

	gl::enableVertexAttribArray(positionLocation);
	gl::vertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, stride, 0);

	mCurrentFrame = 0;

	gl::enableDepthRead();
	gl::enableDepthWrite();
}


void IcoSphereApp::update()
{
	float offset = getElapsedSeconds() * 1.0f;
}

void IcoSphereApp::draw()
{
	gl::clear(Color(0, 0, 0));
	gl::ScopedViewport viewport(vec2(0), mOffScreenFbo->getSize());
	gl::setMatrices(mCam);

	gl::ScopedGlslProg glslScope(mRenderProg);
	gl::ScopedBuffer ScopedSphereBuffer(mSphereVbo);
	gl::ScopedVao vao(mSphereVao);
	gl::pointSize(10);

	gl::drawArrays(GL_POINTS, 0, mSphere.getVertexCount());

	//gl::drawCoordinateFrame(2);

	// SHOW UTILITY GRID
	if (mShowGrid && mGrid)
	{
		//gl::ScopedColor colorScope(Color(1, 1, 1));
		//gl::ScopedGlslProg scopedGlslProg(gl::context()->getStockShader(gl::ShaderDef().color()));

		//mGrid->draw();

		//gl::drawCoordinateFrame(2);
	}

	// RENDERING
	if (mRender && mRenderedFrame < mEndFrame)
	{
		writeImage(renderPath + (toString(mRenderedFrame) + ".png"), mOffScreenFbo->getColorTexture()->createSource());
		mRenderedFrame++;

		if (mRenderedFrame % 100 == 0) console() << "rendered: " << mRenderedFrame << std::endl;
	}

	// SHOW SCREEN OR NOT
	if (mShowScreen)
	{
		mOffScreenFbo->blitToScreen(mOffScreenFbo->getBounds(), getWindowBounds());
	}

	mCurrentFrame++;
}

void IcoSphereApp::createGrid(int size)
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


void IcoSphereApp::mouseDown(MouseEvent event)
{
	if (mAttachCameraUi)
		mCamUi.mouseDown(event);
}

void IcoSphereApp::mouseDrag(MouseEvent event)
{
	if (mAttachCameraUi)
		mCamUi.mouseDrag(event);
}

void IcoSphereApp::mouseWheel(MouseEvent event)
{
	if (mAttachCameraUi)
		mCamUi.mouseWheel(event);
}

void IcoSphereApp::keyDown(KeyEvent event)
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

	if (event.getChar() == 'm')
	{
		mAttachCameraUi = !mAttachCameraUi;
		console() << "mouse attached: " << mShowGrid << std::endl;
	}

}

CINDER_APP(IcoSphereApp, RendererGl, IcoSphereApp::prepareSettings)

