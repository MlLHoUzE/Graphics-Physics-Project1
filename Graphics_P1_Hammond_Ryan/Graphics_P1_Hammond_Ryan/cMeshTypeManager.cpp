#include "cMeshTypeManager.h"

#include <vector>
#include <fstream>

#include <glad/glad.h>			// OpenGL defines...

struct vert_XYZ_RGB
{
	float x, y, z;				// 4 + 4 + 4
	float r, g, b;			// 4 + 4 + 4  = 24 bytes
};



cMeshTypeManager::cMeshTypeManager()
{
	return;
}

cMeshTypeManager::~cMeshTypeManager()
{
	return;
}

// Loads file from disk, AND
// Copies inforamtion into VAO (Vertex Array Object)
bool cMeshTypeManager::LoadPlyFileIntoGLBuffer(unsigned int programID, std::string plyFile)
{
	// ********************************************************************
	// *** START OF: Copying object data into VAO (vertex array object)

	// Here is where the model is loaded into the vertex buffer
	std::vector< cPlyVertexXYZ > vec_PlyVerts;
	std::vector< cPlyTriFace > vec_PlyIndices;

	if (!this->m_loadPlyModel(plyFile, vec_PlyVerts, vec_PlyIndices))
	{
		return false;
	}

	// Calculate the maximum extent (max-min) for x,y,z
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	// Set the initial values to the 1st vertex.
	minX = maxX = vec_PlyVerts[0].x;
	minY = maxY = vec_PlyVerts[0].y;
	minZ = maxZ = vec_PlyVerts[0].z;

	for (int index = 0; index != vec_PlyVerts.size(); index++)
	{
		if (vec_PlyVerts[index].x < minX) { minX = vec_PlyVerts[index].x; }
		if (vec_PlyVerts[index].y < minY) { minY = vec_PlyVerts[index].y; }
		if (vec_PlyVerts[index].z < minZ) { minZ = vec_PlyVerts[index].z; }

		if (vec_PlyVerts[index].x > maxX) { maxX = vec_PlyVerts[index].x; }
		if (vec_PlyVerts[index].y > maxY) { maxY = vec_PlyVerts[index].y; }
		if (vec_PlyVerts[index].z > maxZ) { maxZ = vec_PlyVerts[index].z; }
	}
	// What the max extent
	float extentX = maxX - minX;
	float extentY = maxY - minY;
	float extentZ = maxZ - minZ;

	float maxExtent = extentX;
	if (extentY > maxExtent) { maxExtent = extentY; }
	if (extentZ > maxExtent) { maxExtent = extentZ; }


	// vert_XYZ_RGB for my Model
	int numberofVerts = vec_PlyVerts.size();
	vert_XYZ_RGB* p_vertArray = new vert_XYZ_RGB[numberofVerts];

	for (int index = 0; index != numberofVerts; index++)
	{
		p_vertArray[index].x = vec_PlyVerts[index].x;
		p_vertArray[index].y = vec_PlyVerts[index].y;
		p_vertArray[index].z = vec_PlyVerts[index].z;
	}


	// Copy data in... 

	// Create a vertex array object...
	cMeshTypeManager::VAOInfo tempMeshVAOInfo;
	tempMeshVAOInfo.meshName = plyFile;
	// Save the unit scale value
	tempMeshVAOInfo.unitScaleValue = 1.0f / maxExtent;  //0.01f


	glGenVertexArrays(1, &(tempMeshVAOInfo.VAO_ID));
	// Refer to specific vertex array buffer
	glBindVertexArray(tempMeshVAOInfo.VAO_ID);

	GLuint vertex_buffer = 0;
	GLuint index_buffer = 0;

	// Create a vertex buffer
	glGenBuffers(1, &vertex_buffer);
	// Brings the particular vertex buffer into context
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	int sizeInBytes = sizeof(vert_XYZ_RGB) * numberofVerts;

	glBufferData(GL_ARRAY_BUFFER,
		sizeInBytes,
		p_vertArray,			// vertices
		GL_STATIC_DRAW);

	// Generate an "index" buffer
	glGenBuffers(1, &index_buffer);
	// Brings the particular vertex buffer into context
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

	// Copy the index information into the index buffer
	int numberOfTriangles = vec_PlyIndices.size();
	//	int numberOfIndices = numberOfTriangles * 3;
	tempMeshVAOInfo.numberOfIndices = numberOfTriangles * 3;
	int* pIndexArray = new int[tempMeshVAOInfo.numberOfIndices];


	int indexBufferIndex = 0;
	for (int triIndex = 0;
	triIndex != vec_PlyIndices.size();
		triIndex++, indexBufferIndex += 3)
	{
		pIndexArray[indexBufferIndex + 0] = vec_PlyIndices[triIndex].v1;		// Index 0
		pIndexArray[indexBufferIndex + 1] = vec_PlyIndices[triIndex].v2;		// Index 0
		pIndexArray[indexBufferIndex + 2] = vec_PlyIndices[triIndex].v3;		// Index 0
	}

	int indexBufferArraySizeInBytes = tempMeshVAOInfo.numberOfIndices * sizeof(int);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indexBufferArraySizeInBytes,
		pIndexArray,			// index array
		GL_STATIC_DRAW);



	// Bind the vertex attributes to the shader
	int sizeOfVertInBytes = sizeof(vert_XYZ_RGB);

	// Telling the shader where the vertex info is...
	GLuint vpos_location = glGetAttribLocation(programID, "vPosition");	// 8
	GLuint vcol_location = glGetAttribLocation(programID, "vColour");	// 24

	glEnableVertexAttribArray(vpos_location);
	glVertexAttribPointer(vpos_location, 3,
		GL_FLOAT, GL_FALSE,
		sizeOfVertInBytes,	// sizeof(float) * 6,		// 
		(void*)0);

	int offsetInBytesToColour = offsetof(vert_XYZ_RGB, r);
	glEnableVertexAttribArray(vcol_location);
	glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
		sizeOfVertInBytes,	// sizeof(float) * 5, 
		(void*)(sizeof(float) * 3));


	// Data is copied into the vertex (and index) buffers, so delete array(s)
	delete[] p_vertArray;
	delete[] pIndexArray;

	// Save that VAO info for later

	this->m_MapMeshNameToVAOInfo[plyFile] = tempMeshVAOInfo;


	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// ********************************************************************
	// *** END OF: Copying object data into VAO (vertex array object)
	// ********************************************************************

	return true;
}

bool cMeshTypeManager::LookUpMeshInfo(std::string meshName,
	unsigned int &VAO_ID,	    // GLuint by ref
	int &numberOfIndices,
	float &unitScale)
{
	// Perform a lookup into the map
	std::map< std::string, cMeshTypeManager::VAOInfo >::iterator itVAO
		= this->m_MapMeshNameToVAOInfo.find(meshName);

	// If the iterator ISN'T set to end(), then it found it
	if (itVAO == this->m_MapMeshNameToVAOInfo.end())
	{	// Didn't find
		return false;
	}

	VAO_ID = itVAO->second.VAO_ID;
	numberOfIndices = itVAO->second.numberOfIndices;
	unitScale = itVAO->second.unitScaleValue;

	return true;
}

bool cMeshTypeManager::m_loadPlyModel(std::string filename,
	std::vector< cPlyVertexXYZ > &vecPlyVerts,
	std::vector< cPlyTriFace > &vecPlyIndices)
{
	std::ifstream theFile(filename.c_str());
	if (!theFile.is_open())
	{
		return false;
	}

	// Assume formant is only xyz with no other vertex attribs

	// Read until we get to the text "vertex" 
	std::string temp;
	bool bKeepReading = true;
	while (bKeepReading)
	{
		theFile >> temp;
		// "vertex"?
		if (temp == "vertex")
		{
			bKeepReading = false;
		}
	}
	// Next value is the number of vertices
	int numVertices = 0;
	theFile >> numVertices;

	// Read until we get to the text "face"
	bKeepReading = true;
	while (bKeepReading)
	{
		theFile >> temp;
		// "face"?
		if (temp == "face")
		{
			bKeepReading = false;
		}
	}
	// Next value is the number of faces (aka "triangles")
	int numberOfTriangles = 0;
	theFile >> numberOfTriangles;

	// Read until we get to "end_header"
	bKeepReading = true;
	while (bKeepReading)
	{
		theFile >> temp;
		// "end_header"?
		if (temp == "end_header")
		{
			bKeepReading = false;
		}
	}

	// Now read all the vertices
	for (int index = 0; index != numVertices; index++)
	{
		cPlyVertexXYZ tempVert;
		theFile >> tempVert.x >> tempVert.y >> tempVert.z;
		// add to the temporary vector
		vecPlyVerts.push_back(tempVert);
	}

	// Now the indices...
	for (int index = 0; index != numberOfTriangles; index++)
	{
		cPlyTriFace tempTri;
		int numIndicesDiscard = 0;		// 3
		theFile >> numIndicesDiscard >> tempTri.v1 >> tempTri.v2 >> tempTri.v3;
		vecPlyIndices.push_back(tempTri);
	}

	// All done.

	return true;
}