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
	m_shaderUniforms.add(m_attractionCoeff.set("attraction", 0.18, 0, 1));
	m_shaderUniforms.add(m_cohesionCoeff.set("cohesion", 0.05, 0, 1));
	m_shaderUniforms.add(m_repulsionCoeff.set("repulsion", 0.7, 0, 1));
	m_shaderUniforms.add(m_maxSpeed.set("max_speed", 2500, 0, 5000));
	m_shaderUniforms.add(m_attractor1Force.set("attr1_force", 800, 0, 5000));
	m_shaderUniforms.add(m_attractor2Force.set("attr2_force", 800, 0, 5000));
	m_shaderUniforms.add(m_attractor3Force.set("attr3_force", 1200, 0, 5000));
	m_gui.add(m_shaderUniforms);
	m_gui.add(m_fps.set("fps", 60, 0, 60));
	m_gui.add(m_dirAsColor.set("Colour", false));
	m_dirAsColor.addListener(this, &ofApp::dirAsColorChanged);
	m_dirAsColor = true;

	m_pauseAndPlaySim.addListener(this, &ofApp::pauseAndPlayChanged);
	m_resetSim.addListener(this, &ofApp::resetSim);
	m_resetCam.addListener(this, &ofApp::resetCam);
	m_gui.add(m_pauseAndPlaySim.setup("Pause/Play"));
	m_gui.add(m_resetSim.setup("Reset Sim"));
	m_gui.add(m_resetCam.setup("Reset Cam"));
	m_isPlaying = true;

	m_hideGUI = false;

	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	//m_plane.mapTexCoordsFromTexture(m_imageDir.getImageTexture());

	m_compute.setupShaderFromFile(GL_COMPUTE_SHADER, "compute1.glsl");
	m_compute.linkProgram();
	m_cam.setFarClip(ofGetWidth() * 10);
	m_particles.resize(ofGetWidth() * ofGetHeight());
	int i = 0;
	for (auto & p : m_particles) {
		p.pos.x = ofRandom(-ofGetWidth()*0.5, ofGetWidth()*0.5);
		p.pos.y = ofRandom(-ofGetHeight()*0.5, ofGetHeight()*0.5);
		p.pos.z = ofRandom(-50., -200);
		p.pos.w = 1;
		p.vel.set(0, 0, 0, 0);
		i++;
	}
	m_particleBuffer.allocate(m_particles, GL_DYNAMIC_DRAW);
	m_particleBufferOld.allocate(m_particles, GL_DYNAMIC_DRAW);

	m_vbo.setVertexBuffer(m_particleBuffer, 4, sizeof(Particle));
	m_vbo.setColorBuffer(m_particleBuffer, sizeof(Particle), sizeof(ofVec4f) * 2);

	ofBackground(0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	m_particleBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	m_particleBufferOld.bindBase(GL_SHADER_STORAGE_BUFFER, 1);

	m_outputImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);
	m_plane.mapTexCoordsFromTexture(m_outputImage.getTexture());


	auto files = ofDirectory("").getFiles();
	//m_inputImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR);
	m_inputImage.load(files[0].getFileName());
	auto format = m_inputImage.getPixels().getPixelFormat();

}

//--------------------------------------------------------------
void ofApp::update() {

	m_fps = ofGetFrameRate();
	if (m_isPlaying)
	{
		//m_imageDir.getImageTexture().bind(0);
		//m_outputImage.getTexture().bind(1);

		m_compute.begin();
		m_compute.setUniforms(m_shaderUniforms);
		m_compute.setUniform1f("timeLastFrame", ofGetLastFrameTime());
		m_compute.setUniform1f("elapsedTime", ofGetElapsedTimef());
		m_inputImage.getTexture().bindAsImage(0, GL_READ_WRITE);
		m_inputImage.getTexture().bind(0);
		m_outputImage.getTexture().bindAsImage(1, GL_WRITE_ONLY);

		//m_compute.setUniformTexture(string("inputImage"), m_imageDir.getImage().getTextureReference(), 0);

		float size = 4;
		m_attractor1.set(ofMap(ofNoise(ofGetElapsedTimef()*0.3), 0, 1, -ofGetWidth()*size, ofGetWidth()*size),
			ofMap(ofNoise(ofGetElapsedTimef()*0.3 + 0.2), 0, 1, -ofGetHeight()*size, ofGetHeight()*size),
			ofMap(ofNoise(ofGetElapsedTimef()*0.3 + 0.5), 0, 1, 0, -ofGetHeight()*size));
		m_attractor2.set(ofMap(ofNoise(ofGetElapsedTimef()*0.5 + 0.3), 0, 1, -ofGetWidth()*size, ofGetWidth()*size),
			ofMap(ofNoise(ofGetElapsedTimef()*0.5 + 0.2), 0, 1, -ofGetHeight()*size, ofGetHeight()*size),
			ofMap(ofNoise(ofGetElapsedTimef()*0.5 + 0.1), 0, 1, 0, -ofGetHeight()*size));
		m_attractor3.set(ofMap(ofNoise(ofGetElapsedTimef()*0.9 + 0.1), 0, 1, -ofGetWidth()*size, ofGetWidth()*size),
			ofMap(ofNoise(ofGetElapsedTimef()*0.9 + 0.5), 0, 1, -ofGetHeight()*size, ofGetHeight()*size),
			ofMap(ofNoise(ofGetElapsedTimef()*0.9 + 0.7), 0, 1, 0, -ofGetHeight()*size));

		m_compute.setUniform3f("attractor1", m_attractor1.x, m_attractor1.y, m_attractor1.z);
		m_compute.setUniform3f("attractor2", m_attractor2.x, m_attractor2.y, m_attractor2.z);
		m_compute.setUniform3f("attractor3", m_attractor3.x, m_attractor3.y, m_attractor3.z);

		// since each work group has a local_size of 1024 (this is defined in the shader)
		// we only have to issue 1 / 1024 workgroups to cover the full workload.
		// note how we add 1024 and subtract one, this is a fast way to do the equivalent
		// of std::ceil() in the float domain, i.e. to round up, so that we're also issueing
		// a work group should the total size of particles be < 1024
		m_compute.dispatchCompute(ofGetWidth()/10, ofGetHeight()/10, 1);
		m_compute.end();
		m_inputImage.getTexture().bind(0);
		//m_imageDir.getImageTexture().unbind(0);
		//m_outputImage.getTexture().unbind(1);
		m_particleBuffer.copyTo(m_particleBufferOld);
	}


}

//--------------------------------------------------------------
void ofApp::draw() {

	//ofBackground(40,40,40);

	// draw selected image
	//m_imageDir.getImageTexture().bind();
	m_outputImage.getTexture().bind();
	m_shader.begin();
	m_shader.setUniform1f("uTime", ofGetElapsedTimef());
	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	m_plane.draw();
	ofPopMatrix();
	m_shader.end();
	//m_imageDir.getImageTexture().unbind();
	m_outputImage.getTexture().unbind();


	// draw sim particles
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	m_cam.begin();
	ofSetColor(ofColor::red);
	ofDrawRectangle(m_attractor1,10,10);
	ofDrawRectangle(m_attractor2,10,10);
	ofDrawRectangle(m_attractor3,10,10);

	ofSetColor(255,70);
	glPointSize(5);
	m_vbo.draw(GL_POINTS,0,m_particles.size());
	ofSetColor(255);
	glPointSize(2);
	m_vbo.draw(GL_POINTS,0,m_particles.size());

	ofNoFill();

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


//--------------------------------------------------------------
void ofApp::dirAsColorChanged(bool & dirAsColor){
	if(dirAsColor){
		m_vbo.enableColors();
	}else{
		m_vbo.disableColors();
	}
}

void ofApp::pauseAndPlayChanged()
{
	m_isPlaying = !m_isPlaying;
}

void ofApp::resetSim()
{
	m_particles.resize(ofGetWidth() * ofGetHeight());
	int i = 0;
	for (auto & p : m_particles) {
		p.pos.x = ofRandom(-ofGetWidth()*0.5, ofGetWidth()*0.5);
		p.pos.y = ofRandom(-ofGetHeight()*0.5, ofGetHeight()*0.5);
		p.pos.z = ofRandom(-50., -200);
		p.pos.w = 1;
		p.vel.set(0, 0, 0, 0);
		i++;
	}
	m_particleBuffer.allocate(m_particles, GL_DYNAMIC_DRAW);
	m_particleBufferOld.allocate(m_particles, GL_DYNAMIC_DRAW);

	m_vbo.setVertexBuffer(m_particleBuffer, 4, sizeof(Particle));
	m_vbo.setColorBuffer(m_particleBuffer, sizeof(Particle), sizeof(ofVec4f) * 2);
	m_dirAsColor = true;

	ofBackground(0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	
	m_particleBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	m_particleBufferOld.bindBase(GL_SHADER_STORAGE_BUFFER, 1);

	m_outputImage.clear();
	m_outputImage.allocate(ofGetWidth(), ofGetHeight(), OF_IMAGE_COLOR_ALPHA);


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
