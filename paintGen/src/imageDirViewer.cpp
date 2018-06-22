#include "imageDirViewer.h"

ImageDirViewer::ImageDirViewer()
{
	m_dataDir = ofDirectory("");
}

ImageDirViewer::ImageDirViewer(string dir)
{
	setDirectory(dir);
}


ImageDirViewer::~ImageDirViewer()
{
}

void ImageDirViewer::stop()
{
	m_imageLoader.stopThread();

}

void ImageDirViewer::start()
{
	m_imageLoader.startThread();
}

void ImageDirViewer::setDirectory(string dir)
{
	m_dataDir = ofDirectory(dir);
	m_imageNames.clear();
	m_images.clear();
	m_currentImageIndex = 0;

	auto maxSize = m_dataDir.listDir();
	m_images.resize(maxSize);

	auto files = m_dataDir.getFiles();
	int i = 0;
	for (auto file : files)
	{
		if (file.getExtension() == "jpg")
		{
			m_imageNames.push_back(file.getFileName());
			m_images[i].allocate(ofGetWidth()*2.5, ofGetHeight()*2.5, OF_IMAGE_COLOR); // ballpark the image res as we could only determine this by doing a byte read of the image, which this threaded approach doesn't do
			m_imageLoader.loadFromDisk(m_images[i], file.getFileName());
			i++;
		}
	}

}

string ImageDirViewer::getDirectory() const
{
	return m_dataDir.getOriginalDirectory();
}

size_t ImageDirViewer::getNumImages() const
{
	return m_imageNames.size();
}

void ImageDirViewer::nextImage()
{
	if (m_imageNames.empty())
		return;

	m_currentImageIndex++;
	if (m_currentImageIndex >= (int)m_imageNames.size())
		m_currentImageIndex = 0;
}

void ImageDirViewer::prevImage()
{
	if (m_imageNames.empty())
		return;

	m_currentImageIndex--;
	if (m_currentImageIndex < 0)
		m_currentImageIndex = m_imageNames.size() - 1;
}

void ImageDirViewer::selectImage(int index)
{
	if (m_imageNames.empty())
		return;

	if (index < 0)
		index = 0;
	if (index >= (int)m_imageNames.size())
		index = m_imageNames.size() - 1;

	m_currentImageIndex = index;
}

string ImageDirViewer::getImageName() const
{
	if (m_imageNames.empty())
		return "";

	return m_imageNames[m_currentImageIndex];
}


ofImage & ImageDirViewer::getImage() 
{
	if (m_images.empty())
		return ofImage();

	return m_images[m_currentImageIndex];
}

ofTexture & ImageDirViewer::getImageTexture()
{
	return getImage().getTexture();
}
