#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Fbo.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/gl/Ssbo.h"

using namespace ci;
using namespace ci::app;
using namespace std;

// Particle type holds information for rendering and simulation.
// Used to buffer initial simulation values.
// Using std140 in ssbo requires we have members on 4 byte alignment. 
#pragma pack( push, 1 )
//__declspec( align( 4 ) )
struct Particle
{
	vec3	pos;
	float   pad1;
	vec3	vel;
	float   pad2;
	vec3	acc;
	float	pad3;
	vec4    color;
	float	damping;
	vec3    pad4;
};
#pragma pack( pop )



class NoiseParticlesApp : public App {
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
	unsigned int	mWidth = 1920 / 2;
	unsigned int	mHeight = 1080 / 2;
	float			mScale = 1;

	// screen resolution
	bool			mShowScreen;
	static void prepareSettings(Settings *settings) {
		settings->setWindowSize(1920 / 2, 1080 / 2);
	};

	// recording setups
	unsigned int	mEndFrame = 800;
	unsigned int	mRenderedFrame = 0;
	bool			mRender = false;
	std::string	renderPath = "D:/Projects/creative-coding/nonzeroexitcode/post_processing/" + toString(mPosterId) + "/render/";

	// ----------------------

private:
	// something related to shared resources/optimatization
	enum { WORK_GROUP_SIZE = 128, };

	gl::GlslProgRef mRenderProg;
	gl::GlslProgRef mUpdateProg;

	// Buffers holding raw particle data on GPU.
	gl::SsboRef mParticleBuffer;
	gl::VboRef mIdsVbo;
	gl::VaoRef mAttributes;

	const int NUM_PARTICLES = static_cast<int>(100e3);
};

void NoiseParticlesApp::setup()
{
	mOffScreenFbo = gl::Fbo::create(mWidth * mScale, mHeight * mScale);
	mOffScreenFbo->bindFramebuffer();

	mCurrentFrame = 0;

	vector<Particle> particles;
	particles.assign(NUM_PARTICLES, Particle());

	vec3 center = vec3(mWidth / 2, mHeight / 2, 0.0);
	for (unsigned int i = 0; i < particles.size(); ++i)
	{
		auto &p = particles.at(i);
		float x = Rand::randFloat(center.x - 500, center.x + 500);
		float y = Rand::randFloat(center.y - 500, center.y + 500);
		float z = Rand::randFloat(center.z - 500, center.z + 500);

		p.pos = vec3(x, y, 0);
		p.vel = vec3(0.0, 0.0, 0.0); // random initial velocity

		p.damping = Rand::randFloat(0.965f, 0.985f);
		Color c(CM_HSV, 0.5f, 1.0f, 1.0f);
		p.color = vec4(c.r, c.g, c.b, 1.0f);
	}

	ivec3 count = gl::getMaxComputeWorkGroupCount();
	CI_ASSERT(count.x >= (NUM_PARTICLES / WORK_GROUP_SIZE));

	// Create particle buffers on GPU and copy data into the first buffer.
	// Mark as static since we only write from the CPU once.
	mParticleBuffer = gl::Ssbo::create(particles.size() * sizeof(Particle), particles.data(), GL_STATIC_DRAW);
	gl::ScopedBuffer scopedParticleSsbo(mParticleBuffer);
	mParticleBuffer->bindBase(0);

	// setup shader
	try {
		mRenderProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(loadAsset("particleRender.vert"))
			.fragment(loadAsset("particleRender.frag"))
			.attribLocation("particleId", 0));
	}
	catch (gl::GlslProgCompileExc ex) {
		console() << ex.what() << endl;
		quit();
	}

	std::vector<GLuint> ids(NUM_PARTICLES);
	GLuint currId = 0;
	std::generate(ids.begin(), ids.end(), [&currId]() -> GLuint { return currId++; });

	mIdsVbo = gl::Vbo::create<GLuint>(GL_ARRAY_BUFFER, ids, GL_STATIC_DRAW);
	mAttributes = gl::Vao::create();
	gl::ScopedVao vao(mAttributes);
	gl::ScopedBuffer scopedIds(mIdsVbo);
	gl::enableVertexAttribArray(0);
	gl::vertexAttribIPointer(0, 1, GL_UNSIGNED_INT, sizeof(GLuint), 0);

	try {
		mUpdateProg = gl::GlslProg::
			create(gl::GlslProg::Format().compute(loadAsset("particleUpdate.comp")));
	}
	catch (gl::GlslProgCompileExc e) {
		ci::app::console() << e.what() << std::endl;
		quit();
	}


	gl::enableDepthWrite();
	gl::enableDepthRead();
}

void NoiseParticlesApp::keyDown(KeyEvent event)
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

void NoiseParticlesApp::mouseDown( MouseEvent event )
{
}

void NoiseParticlesApp::update()
{
	// Update particles on the GPU
	gl::ScopedGlslProg prog(mUpdateProg);

	gl::ScopedBuffer scopedParticleSsbo(mParticleBuffer);

	gl::dispatchCompute(NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1);
	gl::memoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

}

void NoiseParticlesApp::draw()
{

	gl::clear(Color(0, 0, 0));
	gl::ScopedViewport viewport(vec2(0), mOffScreenFbo->getSize());

	//gl::setMatricesWindowPersp(getWindowSize());
	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::ScopedGlslProg render(mRenderProg);
	gl::ScopedBuffer scopedParticleSsbo(mParticleBuffer);
	gl::ScopedVao vao(mAttributes);

	mUpdateProg->uniform("uTime", (float)mCurrentFrame);

	gl::context()->setDefaultShaderVars();

	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

	gl::setMatricesWindow(app::getWindowSize());
	gl::drawString(toString(static_cast<int>(getAverageFps())) + " fps", vec2(32.0f, 52.0f));


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

CINDER_APP( NoiseParticlesApp, RendererGl, NoiseParticlesApp::prepareSettings)
