#ifndef _cSceneLoader_HG_
#define _cSceneLoader_HG_

#include <string>
#include <glm/mat4x4.hpp>
#include "cMeshTypeManager.h"
#include "cGameObject.h"

class cSceneLoader
{
public:
	cSceneLoader();
	~cSceneLoader();
	bool loadScene(std::string fileName, cMeshTypeManager &meshManager, unsigned int program, std::vector<cGameObject*> &vec_pGOs, glm::mat4 &targetTransform);
};

#endif