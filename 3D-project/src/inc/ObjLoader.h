#include "defines.h"
#include <fstream>

void loadObj(std::string filename) 
{
	std::istringstream inputString;
	std::ifstream file(OBJ_FOLDER_PATH + filename);
	std::string tempLine;
	while (std::getline(file, tempLine))
	{
		inputString
	}

}