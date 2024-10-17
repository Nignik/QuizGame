#pragma once

#include <string>

struct QuizBlueprint
{
	std::string path;
	int repeats = 1;

	void SetPath(std::string newPath)
	{
		path = newPath;
	}

	void SetRepeatTimes(int n)
	{
		repeats = n;
	}
};