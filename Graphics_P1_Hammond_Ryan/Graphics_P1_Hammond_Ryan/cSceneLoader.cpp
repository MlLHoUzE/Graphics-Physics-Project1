#include "cSceneLoader.h"
#include <fstream>

cSceneLoader::cSceneLoader()
{

}

cSceneLoader::~cSceneLoader()
{

}

bool cSceneLoader::loadScene(std::string fileName, cMeshTypeManager &meshManager, unsigned int program, std::vector<cGameObject*> &vec_pGOs, glm::mat4 &targetTransform)
{
	std::ifstream theFile(fileName.c_str());
	if (!theFile.is_open())
	{
		return -1;
	}
	std::string test;

	while (theFile >> test)
	{
		if (test == "Begin")
			break;
	}

	bool bKeepReading = true;
	do
	{
		std::string meshName;
		theFile >> meshName;	//read in first element in line

		if (meshName == "Camera")
		{
			float tempFloat;
			theFile >> tempFloat;
			targetTransform[3].x = tempFloat;
			theFile >> tempFloat;
			targetTransform[3].y = tempFloat;
			theFile >> tempFloat;
			targetTransform[3].z = tempFloat;

			theFile >> meshName;
		}
		if (meshName == "eof")	//if read in "EOF" then return
		{
			return true;
		}

		if (!meshManager.LoadPlyFileIntoGLBuffer(program, meshName))//load the mesh by name from text file
		{
			//if mesh isnt loaded 
			return -1;
		}

		cGameObject* pGameObject = new cGameObject();	//create new gameobject

		int temp;
		theFile >> temp;	//read in each value and assign them to the appropriate variable in cGameObject
		pGameObject->bIsWireframe = temp;

		float tempFloat;
		theFile >> tempFloat;
		pGameObject->x = tempFloat;
		theFile >> tempFloat;
		pGameObject->y = tempFloat;
		theFile >> tempFloat;
		pGameObject->z = tempFloat;

		theFile >> tempFloat;
		pGameObject->post_Rotation.x = tempFloat;
		theFile >> tempFloat;
		pGameObject->post_Rotation.y = tempFloat;
		theFile >> tempFloat;
		pGameObject->post_Rotation.z = tempFloat;

		theFile >> tempFloat;
		pGameObject->scale = tempFloat;

		theFile >> tempFloat;
		pGameObject->solid_R = tempFloat;
		theFile >> tempFloat;
		pGameObject->solid_G = tempFloat;
		theFile >> tempFloat;
		pGameObject->solid_B = tempFloat;

		pGameObject->meshName = meshName;
		vec_pGOs.push_back(pGameObject);	//add the gameobject to the global variable from the main


	} while (bKeepReading);


}