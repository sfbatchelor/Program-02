#pragma once
#include "ofxThreadedImageLoader.h"
#include "ofxGui.h"

class ImageDirViewer
{
public:

	ImageDirViewer();
	ImageDirViewer(string dir);
	~ImageDirViewer();

	void exit();

	void setDirectory(string dir);
	string getDirectory() const;
	size_t getNumImages() const;

	void nextImage();
	void prevImage();
	void selectImage(int index);
	ofImage& getImage();
	ofTexture& getImageTexture();
	string getImageName() const;

private:

	ofxThreadedImageLoader m_imageLoader;
	ofDirectory m_dataDir;
	vector<ofImage> m_images;
	vector<string>  m_imageNames;
	int m_currentImageIndex;


};
