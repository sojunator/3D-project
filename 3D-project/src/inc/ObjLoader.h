#include "defines.h"
#include <fstream>
#include <sstream>

struct VertexPos
{
	float x, y, z;
};

void loadObj(std::string filename) 
{
	std::istringstream inputString;
	std::ifstream file(OBJ_FOLDER_PATH + filename);
	std::string tempLine;
	while (std::getline(file, tempLine))
	{
		inputString.str(tempLine);
		if (tempLine.substr(0, 2) == "v")
		{

		}
	}

}