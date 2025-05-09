#include "rendererogl.h"
#include <cstddef>

namespace A3D {

RendererOGL::RendererOGL(QOpenGLContext* ctx, CoreGLFunctions* gl)
	: Renderer(),
	  m_context(ctx),
	  m_gl(gl),
	  m_lineMaterial(nullptr),
	  m_skyboxMaterial(nullptr),
	  m_skyboxMesh(nullptr),
	  m_sceneUBO(0),
	  m_brdfCalculated(false),
	  m_brdfLUT(0) {
	log(LC_Debug, u"Constructor: RendererOGL");
}

RendererOGL::~RendererOGL() {
	log(LC_Debug, u"Destructor: RendererOGL (start)");
	DeleteAllResources();
	log(LC_Debug, u"Destructor: RendererOGL (end)");
}

void RendererOGL::pushState(bool withFramebuffer) {
	auto glErrorCheck = checkGlErrors("RendererOGL::pushState");
	Q_UNUSED(glErrorCheck)

	if(m_stateStorage.size() > 24) {
		log(LC_Critical, u"RendererOGL::pushState: GL State stack is too big.");
		return;
	}

	StateStorage newStateStorage;

	m_gl->glGetIntegerv(GL_VIEWPORT, newStateStorage.m_viewport);
	m_gl->glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &newStateStorage.m_drawFramebuffer);
	m_gl->glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &newStateStorage.m_readFramebuffer);
	m_gl->glGetBooleanv(GL_DEPTH_WRITEMASK, &newStateStorage.m_depthMask);
	m_gl->glGetIntegerv(GL_CURRENT_PROGRAM, &newStateStorage.m_program);
	for(GLenum e: { GL_DEPTH_TEST, GL_CULL_FACE, GL_BLEND }) {
		GLboolean feature;
		m_gl->glGetBooleanv(e, &feature);
		newStateStorage.m_features[e] = feature;
	}

	for(std::pair<GLenum, GLenum> e: {
			std::pair<GLenum, GLenum>{                  GL_TEXTURE_BINDING_1D,                   GL_TEXTURE_1D},
             std::pair<GLenum, GLenum>{                  GL_TEXTURE_BINDING_2D,                   GL_TEXTURE_2D},
			std::pair<GLenum, GLenum>{                  GL_TEXTURE_BINDING_3D,                   GL_TEXTURE_3D},
             std::pair<GLenum, GLenum>{            GL_TEXTURE_BINDING_1D_ARRAY,             GL_TEXTURE_1D_ARRAY},
			std::pair<GLenum, GLenum>{            GL_TEXTURE_BINDING_2D_ARRAY,             GL_TEXTURE_2D_ARRAY},
             std::pair<GLenum, GLenum>{           GL_TEXTURE_BINDING_RECTANGLE,            GL_TEXTURE_RECTANGLE},
			std::pair<GLenum, GLenum>{            GL_TEXTURE_BINDING_CUBE_MAP,             GL_TEXTURE_CUBE_MAP},
			std::pair<GLenum, GLenum>{      GL_TEXTURE_BINDING_CUBE_MAP_ARRAY,       GL_TEXTURE_CUBE_MAP_ARRAY},
             std::pair<GLenum, GLenum>{              GL_TEXTURE_BINDING_BUFFER,               GL_TEXTURE_BUFFER},
			std::pair<GLenum, GLenum>{      GL_TEXTURE_BINDING_2D_MULTISAMPLE,       GL_TEXTURE_2D_MULTISAMPLE},
			std::pair<GLenum, GLenum>{GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY, GL_TEXTURE_2D_MULTISAMPLE_ARRAY}
    })
	{
		GLint textureBinding = 0;
		m_gl->glGetIntegerv(e.first, &textureBinding);
		newStateStorage.m_textureBindings[e.second] = textureBinding;
	}

	GLint activeTexture = 0;
	m_gl->glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);
	newStateStorage.m_activeTexture = activeTexture;

	newStateStorage.m_newFramebuffer = 0;
	if(withFramebuffer) {
		m_gl->glGenFramebuffers(1, &newStateStorage.m_newFramebuffer);
		if(newStateStorage.m_newFramebuffer)
			m_gl->glBindFramebuffer(GL_FRAMEBUFFER, newStateStorage.m_newFramebuffer);
	}

	m_stateStorage.emplace(std::move(newStateStorage));
}

void RendererOGL::popState() {
	auto glErrorCheck = checkGlErrors("RendererOGL::popState");
	Q_UNUSED(glErrorCheck)

	if(m_stateStorage.empty()) {
		log(LC_Critical, u"RendererOGL::popState: GL State stack is empty.");
		return;
	}

	StateStorage& lastState = m_stateStorage.top();

	for(auto it = lastState.m_features.begin(); it != lastState.m_features.end(); ++it) {
		if(it->second)
			m_gl->glEnable(it->first);
		else
			m_gl->glDisable(it->first);
	}
	m_gl->glViewport(lastState.m_viewport[0], lastState.m_viewport[1], lastState.m_viewport[2], lastState.m_viewport[3]);
	m_gl->glBindFramebuffer(GL_READ_FRAMEBUFFER, lastState.m_readFramebuffer);
	m_gl->glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lastState.m_drawFramebuffer);

	for(auto it = lastState.m_textureBindings.begin(); it != lastState.m_textureBindings.end(); ++it) {
		m_gl->glBindTexture(it->first, it->second);
	}

	m_gl->glActiveTexture(lastState.m_activeTexture);
	m_gl->glDepthMask(lastState.m_depthMask);
	m_gl->glUseProgram(lastState.m_program);

	if(lastState.m_newFramebuffer)
		m_gl->glDeleteFramebuffers(1, &lastState.m_newFramebuffer);

	m_stateStorage.pop();
}

void RendererOGL::Draw(Group* g, DrawInfo const& drawInfo) {
	auto glErrorCheck = checkGlErrors("RendererOGL::Draw");
	Q_UNUSED(glErrorCheck)

	if(!g || g->renderOptions() & Group::Hidden)
		return;

	Mesh* mesh                  = g->mesh();
	Material* mat               = g->material();
	MaterialProperties* matProp = g->materialProperties();
	LineGroup* lineGroup        = g->lineGroup();

	bool wasFaceCullingDisabled = false;

	if(mesh && mat && matProp) {
		auto glErrorCheck = checkGlErrors("Mesh Rendering");
		Q_UNUSED(glErrorCheck)

		{
			SceneUBO_Data newSceneData = m_sceneData;
			if(m_closestSceneLightsBuffer.capacity() < LightCount)
				m_closestSceneLightsBuffer.reserve(LightCount);

			getClosestSceneLights(drawInfo.m_groupPosition, LightCount, m_closestSceneLightsBuffer);

			std::size_t lightCount = std::min<std::size_t>(LightCount, m_closestSceneLightsBuffer.size());

			for(std::size_t i = 0; i < lightCount; ++i) {
				newSceneData.m_lightPos[i]   = QVector4D(m_closestSceneLightsBuffer[i].second.position, 0.f);
				newSceneData.m_lightColor[i] = m_closestSceneLightsBuffer[i].second.color;
			}
			for(std::size_t i = lightCount; i < LightCount; ++i) {
				newSceneData.m_lightColor[i] = QVector4D(0.f, 0.f, 0.f, 0.f);
				newSceneData.m_lightPos[i]   = QVector4D(0.f, 0.f, 0.f, -1.f);
			}

			if(std::memcmp(&m_sceneData, &newSceneData, sizeof(m_sceneData))) {
				std::memcpy(&m_sceneData, &newSceneData, sizeof(m_sceneData));
				RefreshSceneUBO();
			}
		}

		MeshCacheOGL* meshCache                  = buildMeshCache(mesh);
		MaterialCacheOGL* matCache               = buildMaterialCache(mat);
		MaterialPropertiesCacheOGL* matPropCache = buildMaterialPropertiesCache(matProp);

		Mesh::RenderOptions meshRenderOptions    = mesh->renderOptions();
		Material::RenderOptions matRenderOptions = mat->renderOptions();

		if(meshRenderOptions & Mesh::DisableCulling || matRenderOptions & Material::Translucent || matProp->isTranslucent()) {
			wasFaceCullingDisabled = true;
			m_gl->glDisable(GL_CULL_FACE);
		}

		matCache->install(this, m_gl);
		matPropCache->install(this, m_gl, matCache);
		for(std::size_t i = 0; i < MaterialProperties::MaxTextures; ++i) {
			Texture* t = matProp->texture(static_cast<MaterialProperties::TextureSlot>(i));
			if(t) {
				TextureCacheOGL* tCache = buildTextureCache(t);
				tCache->applyToSlot(this, m_gl, static_cast<GLuint>(i));
			}
			else if(i == MaterialProperties::BrdfTextureSlot) {
				m_gl->glActiveTexture(GL_TEXTURE0 + MaterialProperties::BrdfTextureSlot);
				m_gl->glBindTexture(GL_TEXTURE_2D, getBrdfLUT());
			}
		}

		meshCache->render(this, m_gl, drawInfo.m_modelMatrix, drawInfo.m_viewMatrix, drawInfo.m_projMatrix);
	}

	if(lineGroup) {
		auto glErrorCheck = checkGlErrors("LineGroup Rendering");
		Q_UNUSED(glErrorCheck)

		LineGroupCacheOGL* lineGroupCache                       = buildLineGroupCache(lineGroup);
		MaterialCacheOGL* lineMaterialCache                     = nullptr;
		MaterialPropertiesCacheOGL* lineMaterialPropertiesCache = nullptr;

		if(!mesh && mat)
			lineMaterialCache = buildMaterialCache(mat);
		else
			lineMaterialCache = buildMaterialCache(m_lineMaterial);

		if(matProp)
			lineMaterialPropertiesCache = buildMaterialPropertiesCache(matProp);

		if(!wasFaceCullingDisabled) {
			wasFaceCullingDisabled = true;
			m_gl->glDisable(GL_CULL_FACE);
		}

		lineMaterialCache->install(this, m_gl);
		if(lineMaterialPropertiesCache)
			lineMaterialPropertiesCache->install(this, m_gl, lineMaterialCache);
		lineGroupCache->render(this, m_gl, drawInfo.m_modelMatrix, drawInfo.m_viewMatrix, drawInfo.m_projMatrix);
	}

	if(wasFaceCullingDisabled)
		m_gl->glEnable(GL_CULL_FACE);
}

void RendererOGL::RefreshSceneUBO() {
	auto glErrorCheck = checkGlErrors("RendererOGL::RefreshSceneUBO");
	Q_UNUSED(glErrorCheck)

	if(!m_sceneUBO)
		return;
	m_gl->glBindBuffer(GL_UNIFORM_BUFFER, m_sceneUBO);
	m_gl->glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_sceneData), &m_sceneData);
	m_gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void RendererOGL::BeginDrawing(Camera const& cam, Scene const* scene) {
	auto glErrorCheck = checkGlErrors("RendererOGL::BeginDrawing");
	Q_UNUSED(glErrorCheck)

	Renderer::BeginDrawing(cam, scene);

	genBrdfLUT();

	m_gl->glDisable(GL_BLEND);
	m_gl->glEnable(GL_MULTISAMPLE);
	m_gl->glEnable(GL_CULL_FACE);
	m_gl->glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	m_gl->glClearColor(0.f, 0.f, 0.f, 0.f);
	m_gl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_skyboxView = cam.getView();
	m_skyboxProj = cam.getProjection();

	if(!m_sceneUBO) {
		auto glErrorCheck = checkGlErrors("RendererOGL SceneUBO Creation");
		Q_UNUSED(glErrorCheck)
		m_gl->glGenBuffers(1, &m_sceneUBO);

		if(!m_sceneUBO)
			return;

		m_gl->glBindBuffer(GL_UNIFORM_BUFFER, m_sceneUBO);
		m_gl->glBufferData(GL_UNIFORM_BUFFER, sizeof(m_sceneData), nullptr, GL_DYNAMIC_DRAW);
		m_gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	if(!m_lineMaterial) {
		m_lineMaterial = Material::standardMaterial(Material::LineMaterial);
		buildMaterialCache(m_lineMaterial);
	}

	QVector4D const newCamPos = QVector4D(cam.position());
	if(m_sceneData.m_cameraPos != newCamPos) {
		m_sceneData.m_cameraPos = newCamPos;
		m_gl->glBindBuffer(GL_UNIFORM_BUFFER, m_sceneUBO);
		m_gl->glBufferSubData(GL_UNIFORM_BUFFER, offsetof(SceneUBO_Data, m_cameraPos), sizeof(m_sceneData.m_cameraPos), &m_sceneData.m_cameraPos);
		m_gl->glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	m_gl->glBindBufferBase(GL_UNIFORM_BUFFER, RendererOGL::UBO_SceneBinding, m_sceneUBO);
}

void RendererOGL::EndDrawing(Scene const* scene) {
	Renderer::EndDrawing(scene);
}

std::shared_ptr<RendererOGL::DeferredCaller> RendererOGL::checkGlErrors(QString const& context) {
	return std::make_shared<RendererOGL::DeferredCaller>([this, context]() {
		GLenum err = GL_NO_ERROR;
		while((err = m_gl->glGetError()) != GL_NO_ERROR) {
			log(LC_Warning, QStringLiteral(u"OpenGL Error in context %1: 0x%2").arg(context, QString::number(err, 16)));
		}
	});
}

void RendererOGL::BeginOpaque() {

	if(currentScene() && currentScene()->skybox()) {
		Cubemap* c = currentScene()->skybox();

		// Draw skybox
		if(!m_skyboxMesh)
			m_skyboxMesh = Mesh::standardMesh(Mesh::CubeIndexedMesh);
		if(!m_skyboxMaterial)
			m_skyboxMaterial = Material::standardMaterial(Material::SkyboxMaterial);

		MeshCacheOGL* meshCache    = buildMeshCache(m_skyboxMesh);
		MaterialCacheOGL* matCache = buildMaterialCache(m_skyboxMaterial);
		CubemapCacheOGL* ccCache   = buildCubemapCache(c);

		m_gl->glDepthMask(GL_FALSE);
		m_gl->glDisable(GL_CULL_FACE);
		matCache->install(this, m_gl);
		ccCache->applyToSlot(this, m_gl, static_cast<GLuint>(MaterialProperties::EnvironmentTextureSlot), -1, -1);
		meshCache->render(this, m_gl, QMatrix4x4(), m_skyboxView, m_skyboxProj);
		m_gl->glEnable(GL_CULL_FACE);
		m_gl->glDepthMask(GL_TRUE);

		ccCache->applyToSlot(this, m_gl, -1, static_cast<GLuint>(MaterialProperties::EnvironmentTextureSlot), static_cast<GLuint>(MaterialProperties::PrefilterTextureSlot));
	}

	m_gl->glEnable(GL_DEPTH_TEST);
	m_gl->glDepthFunc(GL_LESS);
}
void RendererOGL::EndOpaque() {}

void RendererOGL::BeginTranslucent() {
	m_gl->glDepthMask(GL_FALSE);
	m_gl->glEnable(GL_BLEND);
	m_gl->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void RendererOGL::EndTranslucent() {
	m_gl->glDepthMask(GL_TRUE);
	m_gl->glDisable(GL_BLEND);
}

class ContextSwitcher : public NonCopyable {
public:
	ContextSwitcher(QOpenGLContext* newContext)
		: m_oldContext(QOpenGLContext::currentContext()),
		  m_newContext(newContext) {
		if(m_newContext && m_newContext != m_oldContext)
			m_newContext->makeCurrent(m_newContext->surface());
	}

	~ContextSwitcher() {
		if(m_newContext != m_oldContext) {
			if(m_oldContext)
				m_oldContext->makeCurrent(m_oldContext->surface());
			else if(m_newContext)
				m_newContext->doneCurrent();
		}
	}

private:
	QOpenGLContext* m_oldContext;
	QOpenGLContext* m_newContext;
};

void RendererOGL::Delete(MeshCache* meshCache) {
	if(m_context.isNull()) {
		log(LC_Debug, u"Couldn't delete MeshCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete meshCache;
}

void RendererOGL::Delete(MaterialCache* matCache) {
	if(m_context.isNull()) {
		log(LC_Debug, u"Couldn't delete MaterialCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete matCache;
}

void RendererOGL::Delete(MaterialPropertiesCache* matPropCache) {
	if(m_context.isNull()) {
		log(LC_Debug, u"Couldn't delete MaterialPropertiesCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete matPropCache;
}

void RendererOGL::Delete(TextureCache* texCache) {
	if(m_context.isNull()) {
		log(LC_Debug, u"Couldn't delete TextureCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete texCache;
}

void RendererOGL::Delete(CubemapCache* cubemapCache) {
	if(m_context.isNull()) {
		log(LC_Debug, u"Couldn't delete CubemapCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete cubemapCache;
}

void RendererOGL::Delete(LineGroupCache* lineGroupCache) {
	if(m_context.isNull()) {
		log(LC_Debug, u"Couldn't delete LineGroupCache: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	delete lineGroupCache;
}

void RendererOGL::DeleteAllResources() {
	if(m_context.isNull()) {
		log(LC_Debug, u"Couldn't delete resources: OpenGL context is unavailable. A memory leak might have happened.");
		return;
	}

	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);

	Renderer::runDeleteOnAllResources();

	if(m_sceneUBO) {
		m_gl->glDeleteBuffers(1, &m_sceneUBO);
		m_sceneUBO = 0;
	}

	if(m_brdfLUT) {
		m_gl->glDeleteTextures(1, &m_brdfLUT);
		m_brdfLUT = 0;
	}

	m_brdfCalculated = false;
}

void RendererOGL::PreLoadEntity(Entity* e) {
	if(!e)
		return;

	Model* model = e->model();
	if(!model || model->renderOptions() & Model::Hidden)
		return;

	std::map<QString, QPointer<Group>> const& groups = model->groups();
	for(auto it = groups.begin(); it != groups.end(); ++it) {
		Group* g = it->second;
		if(!g)
			continue;

		LineGroup* lineGroup        = g->lineGroup();
		Mesh* mesh                  = g->mesh();
		Material* mat               = g->material();
		MaterialProperties* matProp = g->materialProperties();

		if(lineGroup)
			buildLineGroupCache(lineGroup);

		if(mesh)
			buildMeshCache(mesh);

		if(mat)
			buildMaterialCache(mat);

		if(matProp) {
			buildMaterialPropertiesCache(matProp);

			for(std::size_t i = 0; i < MaterialProperties::MaxTextures; ++i) {
				if(Texture* t = matProp->texture(static_cast<MaterialProperties::TextureSlot>(i)))
					buildTextureCache(t);
			}
		}
	}
}

void RendererOGL::genBrdfLUT() {
	if(m_brdfCalculated)
		return;

	GLuint texSlot = getBrdfLUT();
	if(!texSlot)
		return;

	pushState(true);
	Mesh* brdfMesh    = Mesh::standardMesh(Mesh::ScreenQuadMesh);
	Material* brdfMat = Material::standardMaterial(Material::BRDFMaterial);

	MeshCacheOGL* meshCache    = buildMeshCache(brdfMesh);
	MaterialCacheOGL* matCache = buildMaterialCache(brdfMat);

	static QSize const brdfLutSize(512, 512);
	m_gl->glBindTexture(GL_TEXTURE_2D, texSlot);
	m_gl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, brdfLutSize.width(), brdfLutSize.height(), 0, GL_RG, GL_FLOAT, 0);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	m_gl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	m_gl->glViewport(0, 0, brdfLutSize.width(), brdfLutSize.height());
	m_gl->glDisable(GL_DEPTH_TEST);
	m_gl->glDisable(GL_CULL_FACE);

	m_gl->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfLUT, 0);
	m_gl->glClear(GL_COLOR_BUFFER_BIT);

	matCache->install(this, m_gl);
	meshCache->render(this, m_gl, QMatrix4x4(), QMatrix4x4(), QMatrix4x4());
	popState();

	m_brdfCalculated = true;
}

GLuint RendererOGL::getBrdfLUT() {
	if(!m_brdfLUT)
		m_gl->glGenTextures(1, &m_brdfLUT);

	return m_brdfLUT;
}

MeshCacheOGL* RendererOGL::buildMeshCache(Mesh* mesh) {
	std::pair<MeshCacheOGL*, bool> mc = mesh->getOrEmplaceMeshCache<MeshCacheOGL>(rendererID());

	if(mc.first->isDirty())
		mc.first->update(this, m_gl);

	if(mc.second)
		addToMeshCaches(mc.first);

	return mc.first;
}

MaterialCacheOGL* RendererOGL::buildMaterialCache(Material* material) {
	std::pair<MaterialCacheOGL*, bool> mc = material->getOrEmplaceMaterialCache<MaterialCacheOGL>(rendererID());

	if(mc.first->isDirty())
		mc.first->update(this, m_gl);

	if(mc.second)
		addToMaterialCaches(mc.first);

	return mc.first;
}
MaterialPropertiesCacheOGL* RendererOGL::buildMaterialPropertiesCache(MaterialProperties* materialProperties) {
	std::pair<MaterialPropertiesCacheOGL*, bool> mc = materialProperties->getOrEmplaceMaterialPropertiesCache<MaterialPropertiesCacheOGL>(rendererID());

	if(mc.first->isDirty())
		mc.first->update(this, m_gl);

	if(mc.second)
		addToMaterialPropertiesCaches(mc.first);

	return mc.first;
}

TextureCacheOGL* RendererOGL::buildTextureCache(Texture* texture) {
	std::pair<TextureCacheOGL*, bool> tc = texture->getOrEmplaceTextureCache<TextureCacheOGL>(rendererID());

	if(tc.first->isDirty())
		tc.first->update(this, m_gl);

	if(tc.second)
		addToTextureCaches(tc.first);

	return tc.first;
}

CubemapCacheOGL* RendererOGL::buildCubemapCache(Cubemap* cubemap) {
	std::pair<CubemapCacheOGL*, bool> cc = cubemap->getOrEmplaceCubemapCache<CubemapCacheOGL>(rendererID());

	if(cc.first->isDirty())
		cc.first->update(this, m_gl);

	if(cc.second)
		addToCubemapCaches(cc.first);

	return cc.first;
}

LineGroupCacheOGL* RendererOGL::buildLineGroupCache(LineGroup* cubemap) {
	std::pair<LineGroupCacheOGL*, bool> cc = cubemap->getOrEmplaceLineGroupCache<LineGroupCacheOGL>(rendererID());

	if(cc.first->isDirty())
		cc.first->update(this, m_gl);

	if(cc.second)
		addToLineGroupCaches(cc.first);

	return cc.first;
}

}
