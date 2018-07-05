#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ShaderWatcher.h"

class ofApp : public ofBaseApp{

public:
		void setup();
		void update();
		void draw();
		void drawInteractionArea();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

private:


	// Snapshot
	ofImage m_screenGrab;
	string m_screenGrabFilename;
	bool m_snapshot;

	// Cam and Hud
	ofEasyCam m_cam;
	bool m_hideOverlays;
	ofxPanel m_gui;
	ofParameter<string> m_screenSize;

	// Geom & Shader
	ShaderWatcher m_shader;
	ofPlanePrimitive m_plane;


};
