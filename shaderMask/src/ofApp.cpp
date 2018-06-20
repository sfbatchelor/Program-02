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
			//m_images[i].load(file.getFileName());
			m_images[i].allocate(ofGetWidth()*2.5, ofGetHeight()*2.5, OF_IMAGE_COLOR); // ballpark the image res as we could only determine this by doing a byte read of the image, which this threaded approach doesn't do
			m_imageLoader.loadFromDisk(m_images[i], file.getFileName());
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
	m_currentImageIndex = 0;
	m_plane.mapTexCoordsFromTexture(m_images[m_currentImageIndex].getTexture());
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {

	m_images[m_currentImageIndex].getTexture().bind();
	m_shader.begin();
	m_shader.setUniform1f("uTime", ofGetElapsedTimef());

	ofPushMatrix();
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	m_plane.draw();
	ofPopMatrix();
	m_shader.end();
	m_images[m_currentImageIndex].getTexture().unbind();

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
		m_currentImageIndex--;
	else if (key == 'd' || key == 'w')
		m_currentImageIndex++;
	else if (key == 'h')
		m_hideGUI = !m_hideGUI;

	if (m_currentImageIndex >= (int)m_imageNames.size())
		m_currentImageIndex = 0;
	else if (m_currentImageIndex < 0)
		m_currentImageIndex = m_imageNames.size() - 1;

	if (m_currentImageIndex < m_imageNames.size() && m_currentImageIndex >= 0)
	{
		m_currentImageLabel = m_imageNames[m_currentImageIndex];
	}


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
