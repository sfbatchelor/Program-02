#include "ofApp.h"
#include <boost/lexical_cast.hpp>

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);


	m_imageDir.setDirectory("");

	m_shader.load("vert.glsl", "frag.glsl");

	// we add this listener before setting up so the initial circle resolution is correct
	m_gui.setup("INFO"); // most of the time you don't need a name but don't forget to call setup
	m_gui.add(m_screenSize.set("Screen Size", ""));
	m_gui.add(m_currentImageLabel.set("Image:", ""));
	m_gui.add(m_currentShaderLabel.set("Shader", ""));
	m_currentImageLabel = m_imageDir.getImageName();
	m_hideGUI = false;

	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	m_plane.mapTexCoordsFromTexture(m_imageDir.getImageTexture());
	m_files = ofDirectory("").getFiles();
	ofSetBackgroundColor(0.2,0.2,0.2);

}

//--------------------------------------------------------------
void ofApp::update() {
	m_shader.update();
}

//--------------------------------------------------------------
void ofApp::draw() {

	m_imageDir.getImageTexture().bind();
	m_shader.getShader().begin();
	m_shader.getShader().setUniform1f("uTime", ofGetElapsedTimef());

	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	m_plane.draw();
	ofPopMatrix();
	m_shader.getShader().end();
	m_imageDir.getImageTexture().unbind();

	if (!m_hideGUI)
		m_gui.draw();

}

//--------------------------------------------------------------
void ofApp::exit() {
	m_shader.exit();
	m_imageDir.exit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'a' || key == 's')
		m_imageDir.prevImage();
	else if (key == 'd' || key == 'w')
		m_imageDir.nextImage();
	else if (key == 'h')
		m_hideGUI = !m_hideGUI;

	m_currentImageLabel = m_imageDir.getImageName();
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
