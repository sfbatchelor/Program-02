#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);

	//m_imageDir.setDirectory("");
	m_shader.load("vert.glsl", "frag.glsl");
	
	m_gui.setup("INFO"); 
	m_gui.add(m_screenSize.set("Screen Size", ""));
	m_gui.add(m_currentImageLabel.set("Image:", ""));
	m_gui.add(m_currentShaderLabel.set("Shader", ""));
	//m_currentImageLabel = m_imageDir.getImageName();

	m_shaderUniforms.setName("shader params");
	m_shaderUniforms.add(m_minSpeed.set("min_speed", 5, 1, 20));
	m_shaderUniforms.add(m_maxSpeed.set("max_speed", 10, 1, 20));
	m_shaderUniforms.add(m_minStrength.set("min_strength", 1, 1, 20));
	m_shaderUniforms.add(m_maxStrength.set("max_strength", 15, 1, 20));
	m_shaderUniforms.add(m_minLifetime.set("min_lifetime", 1, 1, 20));
	m_shaderUniforms.add(m_maxLifetime.set("max_lifetime", 3, 1, 20));
	m_shaderUniforms.add(m_colourA.set("colour_a", ofColor(221, 60, 48, 255)));
	m_shaderUniforms.add(m_numParticles.set("num_particles", 50, 1, 100));
	m_gui.add(m_shaderUniforms);

	m_gui.add(m_fps.set("fps", 60, 0, 60));

	m_pauseAndPlaySim.addListener(this, &ofApp::pauseAndPlayChanged);
	m_resetSim.addListener(this, &ofApp::resetSim);
	m_resetCam.addListener(this, &ofApp::resetCam);
	m_gui.add(m_pauseAndPlaySim.setup("Pause/Play"));
	m_gui.add(m_resetSim.setup("Reset Sim"));
	m_gui.add(m_resetCam.setup("Reset Cam"));
	m_isPlaying = true;
	m_hideGUI = false;

	m_cam.setFarClip(ofGetWidth() * 100);
	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	resetSim();

	int work_grp_cnt[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n",
		work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);


	int work_grp_size[3];

	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

	printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n",
		work_grp_size[0], work_grp_size[1], work_grp_size[2]);


	int work_grp_inv;
	glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &work_grp_inv);
	printf("max local work group invocations %i\n", work_grp_inv);


}

//--------------------------------------------------------------
void ofApp::update() {

	m_fps = ofGetFrameRate();
	if (m_isPlaying)
	{
		m_compute.begin();
		m_compute.setUniforms(m_shaderUniforms);
		m_compute.setUniform1f("timeLastFrame", ofGetLastFrameTime());
		m_compute.setUniform1f("elapsedTime", ofGetElapsedTimef());
		m_inputImage.getTexture().bind(0);
		m_outputImage.getTexture().bind(1);
		m_outputImage.getTexture().bindAsImage(1, GL_READ_WRITE);
		m_compute.dispatchCompute(m_numParticles.get()  / 10, 1, 1);
		m_compute.end();
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);//make sure compute has done all its writing to the image
		m_inputImage.getTexture().unbind(0);
	}


}

//--------------------------------------------------------------
void ofApp::draw() {


	// draw selected image
	m_outputImage.getTexture().bind();
	m_shader.begin();
	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	m_plane.draw();
	ofPopMatrix();
	m_shader.end();
	m_outputImage.getTexture().unbind();


	m_cam.begin();
	ofSetColor(255,70);
	glPointSize(5);
	m_vbo.draw(GL_POINTS,0,m_particles.size());
	ofSetColor(255);
	glPointSize(2);
	m_vbo.draw(GL_POINTS,0,m_particles.size());
	m_cam.end();

	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	ofSetColor(255);

	if (!m_hideGUI)
		m_gui.draw();

}

//--------------------------------------------------------------
void ofApp::exit() {
	m_imageDir.stop();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'f') {
		ofToggleFullscreen();
	}
	else if (key == 'a' || key == 's')
		m_imageDir.prevImage();
	else if (key == 'd' || key == 'w')
		m_imageDir.nextImage();
	else if (key == 'h')
		m_hideGUI = !m_hideGUI;
	else if (key == ' ')
		pauseAndPlayChanged();
	m_currentImageLabel = m_imageDir.getImageName();

}



void ofApp::pauseAndPlayChanged()
{
	m_isPlaying = !m_isPlaying;
}

void ofApp::resetSim()
{

	m_compute.setupShaderFromFile(GL_COMPUTE_SHADER, "compute1.glsl");
	m_compute.linkProgram();


	m_particles.resize(m_numParticles);
	int i = 0;
	for (auto & p : m_particles) {
		p.endTime = 0.0f; // this resets the attributes in the compute shader
		p.col = ofVec4f(1, 1, 0, 1);
		i++;
	}
	m_particleBuffer.allocate(m_particles, GL_DYNAMIC_DRAW);
	m_vbo.setVertexBuffer(m_particleBuffer, 4, sizeof(Particle));
	m_vbo.setColorBuffer(m_particleBuffer, sizeof(Particle), sizeof(ofVec4f));
	m_vbo.enableColors();
	m_particleBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);

	ofBackground(0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);

	m_outputImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
	m_outputImage.setColor(ofColor(0, 0, 0, 255));
	m_plane.mapTexCoordsFromTexture(m_outputImage.getTexture());

	auto files = ofDirectory("").getFiles();
	m_inputImage.load(files[0].getFileName());
	auto format = m_inputImage.getPixels().getPixelFormat();

}

void ofApp::resetCam()
{
	m_cam.reset();
}
 //--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	m_screenSize = ofToString(w) + "x" + ofToString(h);

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
