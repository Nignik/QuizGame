#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <vector>

#include "CsvHandler.h"
#include "QuizUtils.h"
#include "QuizCards.h"

namespace fs = std::filesystem;

inline std::ifstream getCsvFile(const std::string& filePath)
{
	std::ifstream file(filePath);

	if (!file)
	{
		std::cerr << "Incorrect filename" << "\nTry again." << std::endl;
	}

	return file;
}
inline std::ifstream getCsvFile(const fs::path& filePath)
{
	std::ifstream file(filePath);

	if (!file)
	{
		std::cerr << "Incorrect filename" << "\nTry again." << std::endl;
	}

	return file;
}

inline std::vector<QuizCard> extractCards(std::ifstream& file)
{
	std::vector<QuizCard> cards;
	for (auto& row : CSVRange(file))
	{
		std::string s1 = std::string(row[0]), s2 = std::string(row[1]);
		ltrim(s1); ltrim(s2);
		rtrim(s1); rtrim(s2);
		cards.emplace_back(s1, s2);
	}

	return cards;
}

inline std::vector<std::string> getFolderContent(const std::string& folderPath)
{
	std::vector<std::string> files;

	try
	{
		if (fs::is_directory(folderPath))
		{
			for (const auto& entry : fs::directory_iterator(folderPath))
			{
				files.push_back(entry.path().filename().string());
			}
		}
		else
		{
			std::cerr << "Error: Specified path is not a directory." << std::endl;
		}
	}
	catch (const fs::filesystem_error& e)
	{
		std::cerr << "Filesystem error: " << e.what() << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "General error: " << e.what() << std::endl;
	}

	return files;
}