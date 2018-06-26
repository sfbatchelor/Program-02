#include "ShaderWatcher.h"

ShaderWatcher::ShaderWatcher()
{
}

ShaderWatcher::~ShaderWatcher()
{
	m_vertex.m_fileWatcher.stopThread();
	m_fragment.m_fileWatcher.stopThread();
}

bool ShaderWatcher::load(const std::filesystem::path & vertName, const std::filesystem::path & fragName)
{
	if (m_shader.load(vertName, fragName))
	{

		m_vertex.m_fileWatcher.lock();
		m_fragment.m_fileWatcher.lock();
		m_vertex.m_filePath = vertName;
		m_fragment.m_filePath = fragName;

		m_vertex.m_fileWatcher.setFile(m_vertex.m_filePath, 100);
		m_fragment.m_fileWatcher.setFile(m_fragment.m_filePath, 100);

		m_vertex.m_fileWatcher.registerCallback(std::function<void()>([this]() {onFileWasModified(); }));
		m_fragment.m_fileWatcher.registerCallback(std::function<void()>([this]() {onFileWasModified(); }));

		m_frontShader = m_shader;

		m_vertex.m_fileWatcher.unlock();
		m_fragment.m_fileWatcher.unlock();
		m_vertex.m_fileWatcher.startThread();
		m_fragment.m_fileWatcher.startThread();
		
		return true;
	}

	return false;
}

void  ShaderWatcher::update()
{
	ofScopedLock(m_mutex);
	if (m_needsUpdating)
	{
		load(m_vertex.m_filePath, m_fragment.m_filePath);
		m_needsUpdating = false;
	}

}

const ofShader& ShaderWatcher::getShader() const
{
	ofScopedLock(m_mutex);
	return m_frontShader;
}

void ShaderWatcher::onFileWasModified()
{
	m_mutex.lock();
	m_needsUpdating = true;
	m_mutex.unlock();
}

ComputeWatcher::ComputeWatcher()
{
}

ComputeWatcher::~ComputeWatcher()
{
	m_compute.m_fileWatcher.stopThread();
}

bool ComputeWatcher::loadCompute(const std::filesystem::path & shaderName)
{
	if (m_shader.loadCompute(shaderName))
	{
		m_compute.m_fileWatcher.lock();
		m_compute.m_filePath = shaderName;
		m_compute.m_fileWatcher.setFile(m_compute.m_filePath, 100);
		m_compute.m_fileWatcher.registerCallback(std::function<void()>([this]() {onFileWasModified(); }));
		m_frontShader = m_shader;
		m_compute.m_fileWatcher.unlock();
		return true;
	}
	return false;
}

void ComputeWatcher::update()
{
	ofScopedLock(m_mutex);
	if (m_needsUpdating)
	{
		loadCompute(m_compute.m_filePath);
		m_needsUpdating = false;
	}
}

const ofShader& ComputeWatcher::getShader() const
{
	ofScopedLock(m_mutex);
	return m_frontShader;
}

void ComputeWatcher::onFileWasModified()
{
	m_mutex.lock();
	m_needsUpdating = true;
	m_mutex.unlock();
}
