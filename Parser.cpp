#include "Parser.h"

Parser::Parser(FileMMap& fileMap):m_FileMap(fileMap)
{
}
// {"timestamp":"1583020800577000000", "direction" : "-1", "speed" : "23.28", "time" : "0.0123"}
Result Parser::Parse(const uintmax_t begin, const uintmax_t end)
{
	m_Result.pathNegative = 0;
	m_Result.pathPositive = 0;
	m_Cursor = begin;
	m_End = end;
	braces();
	return m_Result;
}

void Parser::braces()
{
	while (m_Cursor < m_End)
	{
		if (m_FileMap.AT(m_Cursor) == '{')
			colon();
			++m_Cursor;
	}
}

void Parser::colon()
{
	while (m_Cursor < m_End)
	{
		if (m_FileMap.AT(m_Cursor) == ':')
			quotes();
			++m_Cursor;
	}
}

void Parser::quotes()
{
	while (m_Cursor < m_End)
	{
		if (m_FileMap.AT(m_Cursor) == '"')
			value();
			++m_Cursor;
	}
}

void Parser::value()
{
	std::string param;
	++m_Cursor;

	while (m_FileMap.AT(m_Cursor) != '"' &&
		m_Cursor < m_End && (size_t)m_ParameterCount)
	{
		param.push_back(m_FileMap.AT(m_Cursor));
		++m_Cursor;
	}

	switch (static_cast<Parameters>(m_ParameterCount))
	{
		case Parameters::FIRST_PARAMETER:
			m_Item = {};
			break;
		case Parameters::SECOND_PARAMETER:
			m_Item.direction = static_cast<Direction>(atoi(param.c_str()));
			break;
		case Parameters::THIRD_PARAMETER:
			m_Item.speed = atof(param.c_str());
			break;
		case Parameters::FORTH_PARAMETER:
			m_Item.time = atof(param.c_str());

			if (m_Item.direction == Direction::NEGATIVE)
				m_Result.pathNegative += m_Item.speed * m_Item.time;
			else if (m_Item.direction == Direction::POSITIVE)
				m_Result.pathPositive += m_Item.speed * m_Item.time;

			break;
	}

	++m_ParameterCount;

	if (m_ParameterCount > static_cast<size_t>(Parameters::FORTH_PARAMETER))
	{
		m_ParameterCount = static_cast<size_t>(Parameters::FIRST_PARAMETER);
		braces();
	}
	colon();
}

std::optional<intmax_t> Parser::GetBlockBegin(FileMMap& fileMap, const uintmax_t index)
{
	const uintmax_t size = fileMap.getSize();

	for (uintmax_t i = index; i < size; ++i)
	{
		if (fileMap.AT(i) == '{')
		{
			return i;
		}
	}
	return false;
}

std::optional<intmax_t> Parser::GetBlockEnd(FileMMap& fileMap, const uintmax_t index)
{
	const uintmax_t size = fileMap.getSize();

	for (uintmax_t i = index; i < size; ++i)
	{
		if (fileMap.AT(i) == '}')
		{
			return i;
		}
	}

	for (uintmax_t i = index - 1; i >= 0; --i)
	{
		if (fileMap.AT(i) == '}')
		{
			return i;
		}
	}

	return false;
}