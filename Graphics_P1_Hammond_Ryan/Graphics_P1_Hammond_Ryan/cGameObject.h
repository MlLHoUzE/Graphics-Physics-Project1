#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/vec3.hpp>
#include <string>

class cGameObject
{
public:
	cGameObject();

	float pre_Rot_X, pre_Rot_Y, pre_Rot_Z;		// Euler angles 
	glm::vec3 post_Rotation;// , post_Rot_Y, post_Rot_Z;	// Euler angles 
	float scale;		// Relative to a unit scale

	float x, y, z;			// Position ("translation")
private:
	int m_uniqueID;

public:
	std::string meshName;		// "bunny.ply" or whatever

	bool bIsWireframe;
	float solid_R, solid_G, solid_B;		// Solid RGB

};

#endif 
