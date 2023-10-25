#pragma once
 
#include "../Data/Mesh.h"
#include "../Render/Vulkan/GraphicsPipeline.h"

class MeshRenderer
{
public:
	MeshRenderer(const GraphicsPipeline& graphicsPipeline);

private:

	Mesh m_Mesh;
	const GraphicsPipeline& m_GraphicsPipeline;
};

