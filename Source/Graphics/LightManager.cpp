﻿#include "LightManager.h"
#include <imgui.h>

LightManager::LightManager()
{
	
}

LightManager::~LightManager()
{
	Clear();
}

void LightManager::Register(Light* light)
{
	lights.emplace_back(light);
}

void LightManager::Remove(Light* light)
{
	std::vector<Light*>::iterator	it = std::find(lights.begin(), lights.end(), light);
	if (it != lights.end())
	{
		lights.erase(it);
		delete	light;
	}
}

void LightManager::Clear()
{
	for (Light* light : lights)
	{
		delete	light;
	}
	lights.clear();
}

void LightManager::PushRenderContext(RenderContext& rc)
{
	// 環境光の情報を追加
	rc.ambientLightColor = ambientColor;


	// 登録されている光源の情報を設定
	for (Light* light : lights)
	{
		light->PushRenderContext(rc);
	}
}

void LightManager::DrawDebugGUI()
{
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::ColorEdit3("AmbientColor", &ambientColor.x);
		int nodeId = 0;
		for (Light* light : lights)
		{
			ImGui::PushID(nodeId++);
			light->DrawDebugGUI();
			ImGui::PopID();
		}
		ImGui::TreePop();
	}
	

}

void LightManager::DrawDebugPrimitive()
{
	for (Light* light : lights)
	{
		light->DrawDebugPrimitive();
	}
}