#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);

	m_dataDir = ofDirectory("");
	auto maxSize = m_dataDir.listDir();
	m_images.resize(maxSize);

	auto files = m_dataDir.getFiles();
	int i = 0;
	for (auto file : files)
	{
		if (file.getExtension() == "jpg")
		{
			m_imageNames.push_back(file.getFileName());
			m_images[i].load(file.getFileName());
			i++;
		}
	}

	m_shader.load("vert.glsl", "frag.glsl");

	// we add this listener before setting up so the initial circle resolution is correct
	m_gui.setup("INFO"); // most of the time you don't need a name but don't forget to call setup
	m_gui.add(m_screenSize.set("Screen Size", ""));
	m_gui.add(m_currentImageLabel.set("Image:", ""));
	m_gui.add(m_currentShaderLabel.set("Shader", ""));
	m_currentImageLabel = m_imageNames[0];
	m_hideGUI = false;

	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	m_plane.mapTexCoordsFromTexture(m_images[m_currentImage].getTexture());
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {

	m_images[m_currentImage].getTexture().bind();
	m_shader.begin();
	m_shader.setUniform1f("uTime", ofGetElapsedTimef());

	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	m_plane.draw();
	ofPopMatrix();
	m_shader.end();
	m_images[m_currentImage].getTexture().unbind();

	if (!m_hideGUI)
		m_gui.draw();

}

//--------------------------------------------------------------
void ofApp::exit() {
	m_imageLoader.stopThread();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'a' || key == 's')
		m_currentImage--;
	else if (key == 'd' || key == 'w')
		m_currentImage++;
	else if (key == 'h')
		m_hideGUI = !m_hideGUI;

	if (m_currentImage >= (int)m_imageNames.size())
		m_currentImage = 0;
	else if (m_currentImage < 0)
		m_currentImage = m_imageNames.size() - 1;

	if (m_currentImage < m_imageNames.size() && m_currentImage >= 0)
		m_currentImageLabel = m_imageNames[m_currentImage];


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
