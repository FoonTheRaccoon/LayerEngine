#pragma once
 
#include "EngUtils.h"


struct Mesh 
{
	// FRONT_FACE_COUNTER_CLOCKWISE
	T_vector<glm::vec3, MT_GRAPHICS> verts = 
	{
		{0.0f,	0.5f,	0.0f},
		{-0.5f,	-0.5f,	0.0f},
		{0.5f,	-0.5f,	0.0f}
	};
	T_vector<u32, MT_GRAPHICS> indices = {0,1,2};
};
