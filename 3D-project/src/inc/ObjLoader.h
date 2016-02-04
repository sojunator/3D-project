#include "defines.h"
#include <fstream>
#include <sstream>
#include <vector>

struct VertexPos
{
	float x, y, z;
};

struct VertexUv
{
	float u, v;
};

struct VertexNormal
{
	float x, y, z;
};

struct Face
{
	float x, y, z;
};

struct Material
{
	float x, y, z;
};

void loadObj(std::string objfilename, std::string mtrfilename) 
{
	std::istringstream inputString;
	std::ifstream obj_file(OBJ_FOLDER_PATH + objfilename);
	std::ifstream mtr_file(OBJ_FOLDER_PATH + mtrfilename);

	std::string tempLine, special;
	std::vector<VertexPos> vertices;
	std::vector<VertexUv> uvCords;
	std::vector<VertexNormal> normals;
	std::vector<Face> faces;
	Face ctf;
	char slash;
	VertexNormal vtn;
	VertexUv uv;
	VertexPos vtx;

	// Read obj file

	while (std::getline(obj_file, tempLine))
	{
		inputString.str(tempLine);
		if (tempLine.substr(0, 2) == "v ")
		{
			inputString >> special >>
				vtx.x >> vtx.y >> vtx.z;
			vertices.push_back(vtx);
		}

		if (tempLine.substr(0, 2) == "vt")
		{
			inputString >> special >>
				uv.u >> uv.v;
			uvCords.push_back(uv);
		}

		if (tempLine.substr(0, 2) == "vn")
		{
			inputString >> special >>
				vtn.x >> vtn.y >> vtn.z;
			normals.push_back(vtn);
		}

		if (tempLine.substr(0, 2) == "f ")
		{
			for (int i = 0; i < 3; i++)
			{
				inputString >> special >>
					ctf.x >> slash >> ctf.y >> slash >> ctf.z;
				faces.push_back(ctf);
			}
		}

		inputString.clear();
	}
	obj_file.close();

	std::vector<Material> materials;
	Material Kd, Ka, Ks;
	std::string texture;

	while (std::getline(mtr_file, tempLine))
	{
		inputString.str(tempLine);
		if (tempLine.substr(0, 2) == "Kd")
		{
			inputString >> special >>
				Kd.x >> Kd.y >> Kd.z;
			materials.push_back(Kd);
		}

		if (tempLine.substr(0, 2) == "Ka")
		{
			inputString >> special
				>> Ka.x >> Ka.y >> Ka.z;
			materials.push_back(Ka);
		}

		if (tempLine.substr(0, 2) == "Ks")
		{
			inputString >> special
				>> Ks.x >> Ks.y >> Ks.z;
			materials.push_back(Ks);
		}
		
		if (tempLine.substr(0, 3) == "map")
		{
			inputString >> special >>
				texture;
		}

		inputString.clear();
	}
	mtr_file.close();
}