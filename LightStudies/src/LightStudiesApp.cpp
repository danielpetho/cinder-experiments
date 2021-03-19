#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Utilities.h"
#include "cinder/CameraUi.h"
#include "cinder/Rand.h"
#include "cinder/ObjLoader.h" 

#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const unsigned int FBO_WIDTH = 1440;
const unsigned int FBO_HEIGHT = 1440;
const unsigned int SCREEN_DOWNSCALE = 3;

// specify the poster number
const unsigned int POSTER_ID = 209;

class LightStudiesApp : public App {
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


	//----------------------
	gl::BatchRef					mHandObjRef;
	Material						mHandMaterial;

	gl::GlslProgRef					mPhongShader;
	gl::GlslProgRef					mMPhongShader;

	mat4							mSceneRotation;

	const unsigned int				NR_DIR_LIGHTS = 500;

	const unsigned int				NR_GEOMS = 100;

	const unsigned int				NR_POINT_LIGHTS = 1;

	std::vector<DirectionalLight>				mDirLights;
	std::vector<PointLight>						mPointLights;
	std::vector<pair<gl::BatchRef, vec3>>		mGeoms;

	std::vector<mat4>							rotations;
};

void LightStudiesApp::setup()
{
	mOffScreenFbo = gl::Fbo::create(mWidth * mScale, mHeight * mScale);
	//mOffScreenFbo->bindFramebuffer();
	createGrid(mGridSize);

	mCam.setPerspective(45.0f, getWindowAspectRatio(), 0.1f, 10000.0f);
	mCam.lookAt(vec3(0, 20, 1500), vec3(0));
	mCamUi = CameraUi(&mCam);


	// PhongShader
	try {
		mPhongShader = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("phong.vert"))
			.fragment(loadAsset("phong.frag")));

		mMPhongShader = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("phong_m.vert"))
			.fragment(loadAsset("phong_m.frag")));

		ci::app::console() << "shader loaded" << std::endl;
	}
	catch (gl::GlslProgCompileExc e) {
		ci::app::console() << e.what() << std::endl;
		quit();
	}


	// Load Object
	try {
		//ObjLoader loader(loadAsset("torso.obj"));
		ci::app::console() << "object loaded" << std::endl;

		//auto loader = geom::WireIcosahedron();
		//mHandObjRef = gl::Batch::create(loader, mPhongShader);
	}
	catch (gl::GlslProgCompileExc e) {
		ci::app::console() << e.what() << std::endl;
		quit();
	}

	// Object material

	/*mHandMaterial = {
		vec3(0.6f), vec3(0.6f), vec3(0.3f), 64.0f
	};

	mMPhongShader->uniform("material.ambient", mHandMaterial.ambient);
	mMPhongShader->uniform("material.diffuse", mHandMaterial.diffuse);
	mMPhongShader->uniform("material.specular", mHandMaterial.specular);
	mMPhongShader->uniform("material.shininess", mHandMaterial.shininess);*/

	/*for (int i = 0; i < NR_DIR_LIGHTS; i++)
	{
		auto geom = geom::Sphere().radius(randFloat(3, 20)).subdivisions(100);
		gl::BatchRef geomRef = gl::Batch::create(geom, mPhongShader);
		vec3 position = vec3(randFloat(-300.0f, 300.0f), randFloat(-300.0f, 300.0f), randFloat(-300.0f, 300.0f));
		vec3 ambient = vec3(0.0f);
		vec3 diffuse = vec3(0.01f);
		vec3 specular = vec3(0.6f);
		DirectionalLight dirLight(position, ambient, diffuse, specular, geomRef);
		dirLight._id = i;
		//dirLight.setupShader(mMPhongShader);
		mDirLights.push_back(dirLight);
	}
	ci::app::console() << "dir lights done" << std::endl;
	*/

	for (unsigned int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		auto geom = geom::Sphere().radius(randFloat(20)).subdivisions(100);
		gl::ScopedColor color(Color(1.0f, 1.0f, 1.0f));
		gl::BatchRef geomRef = gl::Batch::create(geom, gl::getStockShader(gl::ShaderDef().color()));
		vec3 position = vec3(0.0f, 0.0f, 0.01f);
		vec3 ambient = vec3(1.0f);
		vec3 diffuse = vec3(1.0f);
		vec3 specular = vec3(1.0f);
		float constant = 1.0;
		float linear = 0.19f;
		float quadratic = 0.092f;

		PointLight pointLight(position, ambient, diffuse, specular, constant, linear, quadratic, geomRef);
		pointLight._id = i;
		pointLight.setupShader(mMPhongShader);
		mPointLights.push_back(pointLight);
	}

	ci::app::console() << "point lights done" << std::endl;

	for (unsigned int i = 0; i < NR_GEOMS; i++)
	{
		auto geom = geom::Sphere().radius(randFloat(3, 30)).subdivisions(100);
		//auto geom = geom::Cube().size(vec3(randFloat(3, 20))).subdivisions(30);
		vec3 position = vec3(randFloat(-300.0f, 300.0f), randFloat(-300.0f, 300.0f), randFloat(-300.0f, 300.0f));
		vec3 ambient = vec3(0.2f);
		vec3 diffuse = vec3(0.1f);
		vec3 specular = vec3(0.2f);
		float shininess = 16.0f;

		Material material(ambient, diffuse, specular, shininess);

		gl::BatchRef geomRef = gl::Batch::create(geom, mMPhongShader);

		material.setupShader(mMPhongShader);

		mGeoms.push_back(make_pair(geomRef, position));
	}

	ci::app::console() << "geoms done" << std::endl;

	for (unsigned int i = 0; i < NR_GEOMS; i++)
	{
		rotations.push_back(rotate(toRadians(randFloat(0.5f, 1.5f)), vec3(randFloat(1.0f), randFloat(1.0f), randFloat(1.0f))));
	}

	ci::app::console() << "rotations done" << std::endl;

	mCurrentFrame = 0;
	gl::enableDepth();
}


void LightStudiesApp::update()
{
	unsigned int i = 0;
	/*for (auto &dl : mDirLights) {
		mat4 rotation = rotations.at(i);
		vec3 translatedPos = rotation * vec4(dl.getPosition(), 1.0f);
		//dl.updatePosition(mMPhongShader, translatedPos);
		dl.setPosition(translatedPos);
		++i;
	}*/
	
	i = 0;
	for (auto &geom : mGeoms) {
		mat4 rotation = rotations.at(i);
		vec3 translatedPos = rotation * vec4(geom.second, 1.0f);
		geom.second = translatedPos;
		++i;
	}
	mSceneRotation *= rotate(toRadians(0.1f), normalize(vec3(0.0f, 1.0f, 0.3f)));
}

void LightStudiesApp::draw()
{
	{
		gl::ScopedFramebuffer scpFbo(mOffScreenFbo);

		gl::clear(Color(0.0f, 0.0f, 0.0f));
		gl::ScopedViewport viewport(vec2(0), mOffScreenFbo->getSize());
		//mCam.setEyePoint(vec3(mCam.getEyePoint().x, mCam.getEyePoint().y, mCam.getEyePoint().z + mCurrentFrame * 0.01));

		gl::setMatrices(mCam);

		//gl::multModelMatrix(mSceneRotation);

		mMPhongShader->uniform("uViewPos", mCam.getEyePoint());
		/*mPhongShader->uniform("uLightPos", vec3(0.1f));
		  mPhongShader->uniform("uLightColor", vec3(1.0f));
		  mPhongShader->uniform("uAmbientStrength", 0.9f);
		  mPhongShader->uniform("uSpecularStrength", 0.3f);
		  mPhongShader->uniform("uMaterialShininess", 32.0f);*/

		/*for (auto &dl : mDirLights) {
		  dl.draw();
		  }*/


		for (auto &geom : mGeoms) {
			gl::pushModelMatrix();
			gl::translate(geom.second);
			geom.first->draw();
			gl::popModelMatrix();
		}

		gl::ScopedColor color(Color(1.0f, 1.0f, 1.0f));
		for (auto &pl : mPointLights) {
			//pl.passPositionUf(mMPhongShader);
			pl.draw();
		}


		//gl::ScopedFaceCulling cullScope(true, GL_BACK);
		//mHandObjRef->draw();

		//gl::cullFace(GL_BACK);

		// SHOW UTILITY GRID
		if (mShowGrid && mGrid)
		{
			gl::ScopedColor colorScope(Color(1, 1, 1));
			gl::ScopedGlslProg scopedGlslProg(gl::context()->getStockShader(gl::ShaderDef().color()));

			mGrid->draw();

			gl::drawCoordinateFrame(2);
		}
	}

	gl::ScopedViewport scpViewport(getWindowSize());
	gl::setMatricesWindow(getWindowSize());
	gl::clear();

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
		//mOffScreenFbo->blitToScreen(mOffScreenFbo->getBounds(), getWindowBounds());
		gl::draw(mOffScreenFbo->getColorTexture(), getWindowBounds());
	}

	mCurrentFrame++;
}


///-------------------------------------------------------------------------------------------------
void LightStudiesApp::mouseDown(MouseEvent event)
{
	if (mAttachCameraUi)
		mCamUi.mouseDown(event);
}

void LightStudiesApp::mouseDrag(MouseEvent event)
{
	if (mAttachCameraUi)
		mCamUi.mouseDrag(event);
}

void LightStudiesApp::mouseWheel(MouseEvent event)
{
	if (mAttachCameraUi)
		mCamUi.mouseWheel(event);
}

void LightStudiesApp::keyDown(KeyEvent event)
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

void LightStudiesApp::createGrid(int size)
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

CINDER_APP(LightStudiesApp, RendererGl, LightStudiesApp::prepareSettings)

