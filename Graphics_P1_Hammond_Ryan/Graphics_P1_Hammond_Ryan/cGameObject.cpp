#include "cGameObject.h"



cGameObject::cGameObject()
{

	// C'tor to init all values to 0.0f;

	this->pre_Rot_X = this->pre_Rot_Y = this->pre_Rot_Z = 0.0f;
	this->post_Rotation.x = this->post_Rotation.y = this->post_Rotation.z = 0.0f;
	this->scale = 1.0f;

	this->bIsWireframe = false;	// Solid 
	this->solid_B = 1.0f;
	this->solid_G = 1.0f;
	this->solid_R = 1.0f;	// default white

	return;
}
