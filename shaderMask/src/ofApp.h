#pragma once

#include "ofMain.h"
#include "ofxThreadedImageLoader.h"
#include "ofxGui.h"
#include "utils\ofFileUtils.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofxThreadedImageLoader m_imageLoader;
	vector<ofImage> m_images;
	vector<string> m_imageNames;
	ofDirectory m_dataDir;
	int m_currentImageIndex;

	ofxPanel m_gui;
	ofParameter<string> m_currentImageLabel;
	ofParameter<string> m_screenSize;
	ofParameter<string> m_currentShaderLabel;
	bool m_hideGUI;

	ofShader m_shader;
	ofPlanePrimitive m_plane;
};
