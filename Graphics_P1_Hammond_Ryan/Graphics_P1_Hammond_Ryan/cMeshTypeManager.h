#ifndef _cMeshTypeManager_HG_
#define _cMeshTypeManager_HG_

#include <string>
#include <vector>
#include <map>

struct cPlyVertexXYZ
{
	cPlyVertexXYZ() : x(0.0f), y(0.0f), z(0.0f) {}
	float x;
	float y;
	float z;
};

struct cPlyTriFace
{
	cPlyTriFace() : v1(-1), v2(-1), v3(-1) {}	// can never be -1 in an actual model
	int v1;
	int v2;
	int v3;
};

class cMeshTypeManager
{
public:
	cMeshTypeManager();
	~cMeshTypeManager();

	// Loads file from disk, AND
	// Copies inforamtion into VAO (Vertex Array Object)
	bool LoadPlyFileIntoGLBuffer(unsigned int programID, std::string plyFile);

	bool LookUpMeshInfo(std::string meshName,
		unsigned int &VAO_ID,	    // GLuint by ref
		int &numberOfIndices,
		float &unitScale);		// by ref

private:
	bool m_loadPlyModel(std::string filename,
		std::vector< cPlyVertexXYZ > &vecPlyVerts,
		std::vector< cPlyTriFace > &vecPlyIndices);

	struct VAOInfo
	{
		VAOInfo() : VAO_ID(0), numberOfIndices(0), unitScaleValue(1.0f) {}
		std::string meshName;
		unsigned int VAO_ID;
		unsigned int numberOfIndices;
		float unitScaleValue;		// Multiply by this scale and object 1.0f
	};
	// Map... aka "dictionay" "look up table"
	std::map< std::string, VAOInfo > m_MapMeshNameToVAOInfo;
};

#endif