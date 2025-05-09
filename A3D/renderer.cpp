#include "renderer.h"

namespace A3D {

static std::uintptr_t g_lastRendererID = 0;
static std::map<std::uintptr_t, Renderer*> g_renderers;

std::uintptr_t Renderer::createRendererID(Renderer* renderer) {

	do {
		++g_lastRendererID;
	}
	while(g_renderers.find(g_lastRendererID) != g_renderers.end());

	g_renderers[g_lastRendererID] = renderer;
	return g_lastRendererID;
}

void Renderer::removeRendererID(std::uintptr_t rendererID) {
	g_renderers.erase(rendererID);
}
Renderer* Renderer::getRenderer(std::uintptr_t rendererID) {
	auto it = g_renderers.find(rendererID);
	if(it == g_renderers.end())
		return nullptr;
	return it->second;
}

Renderer::Renderer()
	: m_rendererID(0),
	  m_currentScene(nullptr) {
	log(LC_Debug, u"Constructor: Renderer");
	m_rendererID = Renderer::createRendererID(this);
}

Renderer::~Renderer() {
	removeRendererID(rendererID());
	log(LC_Debug, u"Destructor: Renderer");
}

std::uintptr_t Renderer::rendererID() const {
	return m_rendererID;
}

void Renderer::CleanupRenderCache() {
	cleanupQPointers(m_meshCaches);
	cleanupQPointers(m_materialCaches);
	cleanupQPointers(m_materialCaches);
	cleanupQPointers(m_textureCaches);
	cleanupQPointers(m_cubemapCaches);
	cleanupQPointers(m_lineGroupCaches);
}

bool Renderer::OpaqueSorter(GroupBufferData const& a, GroupBufferData const& b) {
	return a.m_distanceFromCamera < b.m_distanceFromCamera;
}

bool Renderer::TranslucentSorter(GroupBufferData const& a, GroupBufferData const& b) {
	return a.m_distanceFromCamera > b.m_distanceFromCamera;
}

void Renderer::PreLoadEntityTree(Entity* root) {
	this->PreLoadEntity(root);

	std::vector<QPointer<Entity>> const& subEntities = root->childrenEntities();
	for(auto it = subEntities.begin(); it != subEntities.end(); ++it) {
		if(it->isNull())
			continue;
		PreLoadEntityTree(*it);
	}
}

void Renderer::DrawAll(Scene* root, Camera const& camera) {
	m_opaqueGroupBuffer.clear();
	m_translucentGroupBuffer.clear();

	BuildDrawLists(camera, root, root->entityMatrix());

	std::stable_sort(m_opaqueGroupBuffer.begin(), m_opaqueGroupBuffer.end(), Renderer::OpaqueSorter);
	std::stable_sort(m_translucentGroupBuffer.begin(), m_translucentGroupBuffer.end(), Renderer::TranslucentSorter);

	DrawInfo drawInfo;
	drawInfo.m_scene      = root;
	drawInfo.m_projMatrix = camera.getProjection();
	drawInfo.m_viewMatrix = camera.getView();

	this->BeginDrawing(camera, root);

	this->BeginOpaque();
	for(auto it = m_opaqueGroupBuffer.begin(); it != m_opaqueGroupBuffer.end(); ++it) {
		drawInfo.m_modelMatrix   = it->m_transform;
		drawInfo.m_groupPosition = it->m_position;
		this->Draw(it->m_group, drawInfo);
	}
	this->EndOpaque();

	this->BeginTranslucent();
	for(auto it = m_translucentGroupBuffer.begin(); it != m_translucentGroupBuffer.end(); ++it) {
		drawInfo.m_modelMatrix   = it->m_transform;
		drawInfo.m_groupPosition = it->m_position;
		this->Draw(it->m_group, drawInfo);
	}
	this->EndTranslucent();

	this->EndDrawing(root);
}

void Renderer::BuildDrawLists(Camera const& camera, Entity* e, QMatrix4x4 const& cascadeMatrix) {
	if(!e || e->renderOptions() & Entity::Hidden)
		return;

	Model* m = e->model();
	if(m && !(m->renderOptions() & Model::Hidden)) {
		QMatrix4x4 baseMatrix                            = cascadeMatrix * m->modelMatrix();
		std::map<QString, QPointer<Group>> const& groups = m->groups();
		for(auto it = groups.begin(); it != groups.end(); ++it) {
			Group* g = it->second;
			if(!g || g->renderOptions() & Group::Hidden)
				continue;

			Mesh* mesh                  = g->mesh();
			Material* mat               = g->material();
			MaterialProperties* matProp = g->materialProperties();
			LineGroup* lg               = g->lineGroup();

			if(mesh && mat && matProp) {
				GroupBufferData* gbd = nullptr;
				if(mat->renderOptions() & Material::Translucent || matProp->isTranslucent() || lg != nullptr) {
					m_translucentGroupBuffer.emplace_back();
					gbd = &m_translucentGroupBuffer.back();
				}
				else {
					m_opaqueGroupBuffer.emplace_back();
					gbd = &m_opaqueGroupBuffer.back();
				}

				gbd->m_group              = g;
				gbd->m_transform          = baseMatrix * g->groupMatrix();
				gbd->m_position           = gbd->m_transform.map(g->position());
				gbd->m_distanceFromCamera = QVector3D::dotProduct(gbd->m_position - camera.position(), camera.forward());
			}
			else if(lg) {
				m_translucentGroupBuffer.emplace_back();
				GroupBufferData* gbd = &m_translucentGroupBuffer.back();

				gbd->m_group              = g;
				gbd->m_transform          = baseMatrix * g->groupMatrix();
				gbd->m_position           = gbd->m_transform.map(g->position());
				gbd->m_distanceFromCamera = QVector3D::dotProduct(gbd->m_position - camera.position(), camera.forward());
			}
		}
	}

	std::vector<QPointer<Entity>> const& subEntities = e->childrenEntities();
	for(auto it = subEntities.begin(); it != subEntities.end(); ++it) {
		if(it->isNull())
			continue;
		BuildDrawLists(camera, *it, cascadeMatrix * (*it)->entityMatrix());
	}
}

void Renderer::BeginDrawing(Camera const&, Scene const* scene) {
	m_currentScene = scene;
}
void Renderer::EndDrawing(Scene const*) {
	m_currentScene = nullptr;
}
void Renderer::BeginOpaque() {}
void Renderer::EndOpaque() {}
void Renderer::BeginTranslucent() {}
void Renderer::EndTranslucent() {}

void Renderer::runDeleteOnAllResources() {
	for(auto it = m_meshCaches.begin(); it != m_meshCaches.end(); ++it) {
		QPointer<MeshCache>& mc = *it;
		if(mc.isNull())
			continue;
		delete mc;
	}
	m_meshCaches.clear();

	for(auto it = m_materialCaches.begin(); it != m_materialCaches.end(); ++it) {
		QPointer<MaterialCache>& mc = *it;
		if(mc.isNull())
			continue;
		delete mc;
	}
	m_materialCaches.clear();

	for(auto it = m_materialPropertiesCaches.begin(); it != m_materialPropertiesCaches.end(); ++it) {
		QPointer<MaterialPropertiesCache>& mc = *it;
		if(mc.isNull())
			continue;
		delete mc;
	}
	m_materialPropertiesCaches.clear();

	for(auto it = m_textureCaches.begin(); it != m_textureCaches.end(); ++it) {
		QPointer<TextureCache>& tc = *it;
		if(tc.isNull())
			continue;
		delete tc;
	}
	m_textureCaches.clear();

	for(auto it = m_cubemapCaches.begin(); it != m_cubemapCaches.end(); ++it) {
		QPointer<CubemapCache>& cc = *it;
		if(cc.isNull())
			continue;
		delete cc;
	}
	m_cubemapCaches.clear();

	for(auto it = m_lineGroupCaches.begin(); it != m_lineGroupCaches.end(); ++it) {
		QPointer<LineGroupCache>& lgc = *it;
		if(lgc.isNull())
			continue;
		delete lgc;
	}
	m_lineGroupCaches.clear();
}

void Renderer::invalidateCache() {
	for(auto it = m_meshCaches.begin(); it != m_meshCaches.end(); ++it) {
		QPointer<MeshCache>& mc = *it;
		if(mc.isNull())
			continue;
		mc->markDirty();
	}

	for(auto it = m_materialCaches.begin(); it != m_materialCaches.end(); ++it) {
		QPointer<MaterialCache>& mc = *it;
		if(mc.isNull())
			continue;
		mc->markDirty();
	}

	for(auto it = m_materialPropertiesCaches.begin(); it != m_materialPropertiesCaches.end(); ++it) {
		QPointer<MaterialPropertiesCache>& mc = *it;
		if(mc.isNull())
			continue;
		mc->markDirty();
	}

	for(auto it = m_textureCaches.begin(); it != m_textureCaches.end(); ++it) {
		QPointer<TextureCache>& tc = *it;
		if(tc.isNull())
			continue;
		tc->markDirty();
	}

	for(auto it = m_cubemapCaches.begin(); it != m_cubemapCaches.end(); ++it) {
		QPointer<CubemapCache>& cc = *it;
		if(cc.isNull())
			continue;
		cc->markDirty();
	}

	for(auto it = m_lineGroupCaches.begin(); it != m_lineGroupCaches.end(); ++it) {
		QPointer<LineGroupCache>& lgc = *it;
		if(lgc.isNull())
			continue;
		lgc->markDirty();
	}
}

void Renderer::getClosestSceneLights(QVector3D const& pos, size_t desiredLightCount, std::vector<std::pair<std::size_t, PointLightInfo>>& result, Scene const* sceneOverride) {
	if(!sceneOverride)
		sceneOverride = m_currentScene;
	if(!sceneOverride) {
		result.resize(0);
		return;
	}

	std::map<std::size_t, PointLightInfo> const& l = sceneOverride->lights();

	result.resize(std::min<std::size_t>(desiredLightCount, l.size()));

	std::partial_sort_copy(
		l.begin(), l.end(), result.begin(), result.end(),
		[&](std::pair<std::size_t, PointLightInfo> const& l, std::pair<std::size_t, PointLightInfo> const& r) -> bool {
			return l.second.position.distanceToPoint(pos) < r.second.position.distanceToPoint(pos);
		}
	);
}

Scene const* Renderer::currentScene() const {
	return m_currentScene;
}

void Renderer::addToMaterialCaches(QPointer<MaterialCache> material) {
	m_materialCaches.push_back(std::move(material));
}

void Renderer::addToMaterialPropertiesCaches(QPointer<MaterialPropertiesCache> materialProperties) {
	m_materialPropertiesCaches.push_back(std::move(materialProperties));
}

void Renderer::addToMeshCaches(QPointer<MeshCache> mesh) {
	m_meshCaches.push_back(std::move(mesh));
}

void Renderer::addToTextureCaches(QPointer<TextureCache> texture) {
	m_textureCaches.push_back(std::move(texture));
}

void Renderer::addToCubemapCaches(QPointer<CubemapCache> cubemap) {
	m_cubemapCaches.push_back(std::move(cubemap));
}

void Renderer::addToLineGroupCaches(QPointer<LineGroupCache> lineGroup) {
	m_lineGroupCaches.push_back(std::move(lineGroup));
}

}
