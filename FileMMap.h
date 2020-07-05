#pragma once

#include <iostream>
#include <stdexcept>
#include <boost/iostreams/device/mapped_file.hpp>
#include <filesystem>

class FileMMap
{
private:	
	boost::iostreams::mapped_file_source m_File;
	uintmax_t m_FileSize;
public:
	FileMMap(const std::string& pathToFile);
	~FileMMap();
	const uintmax_t getSize();
	const char& AT(const uintmax_t index);
};