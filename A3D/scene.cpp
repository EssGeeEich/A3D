#include "scene.h"

namespace A3D {

Scene::Scene(QObject* parent)
	: Entity{ nullptr },
	  m_runTimeMultiplier(1.f) {
	QObject::setParent(parent);
	log(LC_Debug, u"Constructor: Scene");
}
Scene::~Scene() {
	log(LC_Debug, u"Destructor: Scene");
}

ResourceManager* Scene::resourceManager() {
	return &m_resourceManager;
}

ResourceManager const* Scene::resourceManager() const {
	return &m_resourceManager;
}

PointLightInfo& Scene::getOrCreateLight(std::size_t id) {
	return m_lights[id];
}
PointLightInfo const* Scene::getLight(std::size_t id) const {
	auto it = m_lights.find(id);
	if(it == m_lights.end())
		return nullptr;
	return &(it->second);
}
std::map<std::size_t, PointLightInfo> const& Scene::lights() const {
	return m_lights;
}

Cubemap* Scene::skybox() const {
	return m_skybox;
}

void Scene::setSkybox(Cubemap* c) {
	m_skybox = c;
}

void Scene::addController(SceneController* sc) {
	removeController(sc);
	m_sceneControllers.push_back(sc);
}

void Scene::removeController(SceneController* sc) {
	m_sceneControllers.erase(std::remove_if(m_sceneControllers.begin(), m_sceneControllers.end(), [sc](QPointer<SceneController> it) -> bool {
		return (!it || it != sc);
	}));
}

float Scene::runTimeMultiplier() const {
	return m_runTimeMultiplier;
}

void Scene::setRunTimeMultiplier(float rtm) {
	m_runTimeMultiplier = rtm;
}

bool Scene::isRunning() const {
	return m_sceneRunTimer.isValid();
}

void Scene::setRunning(bool running) {
	if(m_sceneRunTimer.isValid() == running)
		return;

	if(running)
		m_sceneRunTimer.restart();
	else
		m_sceneRunTimer.invalidate();
}

void Scene::updateScene() {
	if(!m_sceneRunTimer.isValid() || m_sceneRunTimer.elapsed() <= 0)
		return;

	std::chrono::milliseconds t = std::chrono::milliseconds(m_sceneRunTimer.restart());

	bool hasChanges = this->updateEntity(t);
	for(auto it = m_sceneControllers.begin(); it != m_sceneControllers.end();) {
		if(!*it) {
			it = m_sceneControllers.erase(it);
			continue;
		}

		hasChanges = (*it)->update(t) || hasChanges;
		++it;
	}

	if(hasChanges)
		emit sceneUpdated();
}

}
