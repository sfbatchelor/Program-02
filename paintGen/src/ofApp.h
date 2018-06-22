#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "utils\ofFileUtils.h"

#include "imageDirViewer.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();
	void pauseAndPlayChanged();
	void resetSim();
	void resetCam();


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

	ImageDirViewer m_imageDir;

	ofxPanel m_gui;
	ofParameter<string> m_currentImageLabel;
	ofParameter<string> m_screenSize;
	ofParameter<string> m_currentShaderLabel;

	ofParameter<float> m_maxSpeed, m_minSpeed;
	ofParameter<float> m_maxStrength, m_minStrength;
	ofParameter<float> m_maxLifetime, m_minLifetime;
	ofParameter<int> m_numParticles;
	ofParameter<ofColor> m_colourA;
	ofParameterGroup m_shaderUniforms;

	ofParameter<float> m_fps;
	ofxButton m_resetSim;
	ofxButton m_pauseAndPlaySim;
	ofxButton m_resetCam;

	bool m_isPlaying;
	bool m_hideGUI;


	struct Particle {
		ofVec4f pos;
		ofVec4f col;
		float endTime;
		float speed;
		float strength;
	};

	ofShader m_shader;
	ofShader m_compute;
	ofImage m_outputImage;
	ofImage m_inputImage;
	vector<Particle> m_particles;
	ofBufferObject m_particleBuffer;
	GLuint m_vaoID;
	ofEasyCam m_cam;
	ofVbo m_vbo;


	ofPlanePrimitive m_plane;
};
