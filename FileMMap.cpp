#include "FileMMap.h"

FileMMap::FileMMap(const std::string& pathToFile)
{
	m_File.open(pathToFile, m_FileSize = std::filesystem::file_size(pathToFile));
}

FileMMap::~FileMMap()
{
	m_File.close();
}
	
const uintmax_t FileMMap::getSize()
{
	return m_FileSize;
}

const char& FileMMap::AT(const uintmax_t index)
{
	if (index >= m_FileSize)
		throw std::out_of_range("out of range");
	return m_File.data()[index];
}

