#include "Scene.h"

void Scene::Add(Object& object)
{
	m_Objects.reserve(m_Objects.size() + 1);
	m_Objects.push_back(&object);
}

void Scene::Add(PointLight& light)
{
	m_PointLights.reserve(m_PointLights.size() + 1);
	m_PointLights.push_back(&light);
}

void Scene::Add(DirLight& light)
{
	m_DirLights.reserve(m_DirLights.size() + 1);
	m_DirLights.push_back(&light);
}
