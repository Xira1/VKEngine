#pragma once

#include "../../Util/Utils.hpp"



class VulkanBase : public Util::Singleton<VulkanBase> {
public:
	bool Init();

	void DrawFrame();

	void CleanUp();

private:
	bool framebufferResized = false;
};

