#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	// hud and settings
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(true);
	m_snapshot = false;
	m_gui.setup("INFO"); 
	m_gui.add(m_screenSize.set("Screen Size", ""));
	m_hideOverlays = false;

	// webcam 
	m_camWidth = ofGetWidth();  // try to grab at this size.
    m_camHeight = ofGetHeight();
    vector<ofVideoDevice> devices = m_videoGrabber.listDevices();
    for(size_t i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            //log the device
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            //log the device and note it as unavailable
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }
    m_videoGrabber.setDeviceID(0);
    m_videoGrabber.setDesiredFrameRate(60);
    m_videoGrabber.initGrabber(m_camWidth, m_camHeight);





	// geom & shader
	m_shader.load("vert.glsl", "frag.glsl");
	m_plane.set(ofGetWidth(), ofGetHeight(), 10, 10);
	m_plane.mapTexCoordsFromTexture(m_videoGrabber.getTexture());
	ofSetBackgroundColor(0.2,0.2,0.2);




}

//--------------------------------------------------------------
void ofApp::update(){
	m_shader.update();
	m_videoGrabber.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	/// WORLD
	{
		m_cam.begin();
		m_shader.getShader().begin();
		m_videoGrabber.getTexture().bind();
		m_shader.getShader().setUniform1f("uTime", ofGetElapsedTimef());
		ofPushMatrix();
		m_plane.draw();
		ofPopMatrix();
		m_videoGrabber.getTexture().unbind();
		m_shader.getShader().end();

		/// SCREEN GRAB
		if (m_snapshot == true) {
			m_screenGrab.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
			string fileName = "snapshot_" + ofGetTimestampString() + ".png";
			m_screenGrab.save(fileName);
			m_screenGrabFilename = "saved " + fileName;
			m_snapshot = false;
		}

		if (!m_hideOverlays)
			ofDrawGrid(5000, 5, true, true, true, true);
		m_cam.end();
	}


	/// GUI
	if (!m_hideOverlays)
	{

		m_gui.draw();
		stringstream ss;
		ss << "FPS: " << ofToString(ofGetFrameRate(), 0) << endl << endl;
		ss << "MODE: " << (m_cam.getOrtho() ? "ORTHO" : "PERSPECTIVE") << endl;
		ss << "MOUSE INPUT ENABLED: " << (m_cam.getMouseInputEnabled() ? "TRUE" : "FALSE") << endl;
		ss << "INERTIA ENABLED: " << (m_cam.getInertiaEnabled() ? "TRUE" : "FALSE") << endl;
		ss << "ROTATION RELATIVE Y AXIS: " << (m_cam.getRelativeYAxis() ? "TRUE" : "FALSE") << endl;
		if (m_cam.getOrtho()) {
			ss << "    Notice that in ortho mode zoom will be centered at the mouse position." << endl;
		}
		ofDrawBitmapString(ss.str().c_str(), 20, 100);
		// also interaction area
		drawInteractionArea();

	}
}
//--------------------------------------------------------------
void ofApp::drawInteractionArea()
{
	ofRectangle vp = ofGetCurrentViewport();
	float r = std::min<float>(vp.width, vp.height) * 0.5f;
	float x = vp.width * 0.5f;
	float y = vp.height * 0.5f;

	ofPushStyle();
	ofSetLineWidth(3);
	ofSetColor(255, 255, 0);
	ofNoFill();
	glDepthMask(false);
	ofSetCircleResolution(64);

	ofDrawCircle(x, y, r);
	glDepthMask(true);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::exit() {
	m_shader.exit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	switch (key) {
	case 'x':
		m_snapshot = true;
		break;
	case ' ':
		m_cam.getOrtho() ? m_cam.disableOrtho() : m_cam.enableOrtho();
		break;
	case 'C':
	case 'c':
		m_cam.getMouseInputEnabled() ? m_cam.disableMouseInput() : m_cam.enableMouseInput();
		break;
	case 'F':
	case 'f':
		ofToggleFullscreen();
		break;
	case 'H':
	case 'h':
		m_hideOverlays = true;
		break;
	case 'I':
	case 'i':
		m_cam.getInertiaEnabled() ? m_cam.disableInertia() : m_cam.enableInertia();
		break;
	case 'Y':
	case 'y':
		m_cam.setRelativeYAxis(!m_cam.getRelativeYAxis());
		break;
	}

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	m_screenSize = ofToString(w) + "x" + ofToString(h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
