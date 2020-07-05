#pragma once

#include "FileMMap.h"
#include <string>
#include <optional>


enum class Direction : int
{
	NOT_DEFINE = 0,
	POSITIVE = 1,
	NEGATIVE = -1
};

struct Item
{
	Direction direction;
	double speed;
	double time;
};

struct Result
{
	double pathPositive; 
	double pathNegative;
};

enum class Parameters : size_t
{
	FIRST_PARAMETER = 0,
	SECOND_PARAMETER = 1,
	THIRD_PARAMETER = 2,
	FORTH_PARAMETER = 3
};

class Parser
{
	FileMMap& m_FileMap;
	
	uintmax_t m_Cursor{};
	uintmax_t m_End{};
	Item m_Item{};
	size_t m_ParameterCount{};
	Result m_Result{};

public:

	Parser(FileMMap& fileMap);
    static std::optional<intmax_t> GetBlockBegin(FileMMap& fileMap, const uintmax_t index);
	static std::optional<intmax_t> GetBlockEnd(FileMMap& fileMap, const uintmax_t index);
	
	// {"timestamp":"1583020800577000000", "direction" : "-1", "speed" : "23.28", "time" : "0.0123"}
	Result Parse(const uintmax_t begin, const uintmax_t end);
	
private:
	void braces();
	void colon();
	void quotes();
	void value();
};

