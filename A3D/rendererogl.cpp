#include "rendererogl.h"

namespace A3D {

RendererOGL::RendererOGL(QOpenGLContext* ctx, CoreGLFunctions* gl)
	: Renderer(),
	  m_context(ctx),
	  m_gl(gl)
{
	dbgConstruct("RendererOGL")
}

RendererOGL::~RendererOGL() {
	dbgDestruct("RendererOGL started")
	DeleteAllResources();
	dbgDestruct("RendererOGL finished")
}

void RendererOGL::Draw(Entity* e, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix) {	
	if(!e)
		return;
	Mesh* mesh = e->mesh();
	Material* mat = e->material();
	if(!mesh || !mat)
		return;
	
	MeshCacheOGL* meshCache = buildMeshCache(mesh);
	MaterialCacheOGL* matCache = buildMaterialCache(mat);
	
	Mesh::RenderOptions meshRenderOptions = mesh->renderOptions();
	Material::RenderOptions materialRenderOptions = mat->renderOptions();
	
	if(meshRenderOptions & Mesh::DisableCulling || materialRenderOptions & Material::Translucent)
		m_gl->glDisable(GL_CULL_FACE);
	
	matCache->render(m_gl, e->materialProperties(), parentMatrix * e->modelMatrix(), viewMatrix, projMatrix);
	meshCache->render(m_gl);
	
	if(meshRenderOptions & Mesh::DisableCulling || materialRenderOptions & Material::Translucent)
		m_gl->glEnable(GL_CULL_FACE);
}

void RendererOGL::BeginOpaque() {
	m_gl->glEnable(GL_DEPTH_TEST);
	m_gl->glDepthFunc(GL_LESS);
}
void RendererOGL::EndOpaque() {
	
}
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
		  m_newContext(newContext)
	{
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
		qDebug() << "Couldn't delete MeshCache: OpenGL context is unavailable. A memory leak might have happened.";
		return;
	}
	
	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);
	
	delete meshCache;
}

void RendererOGL::Delete(MaterialCache* matCache) {
	if(m_context.isNull()) {
		qDebug() << "Couldn't delete MaterialCache: OpenGL context is unavailable. A memory leak might have happened.";
		return;
	}
	
	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);
	
	delete matCache;
}

void RendererOGL::DeleteAllResources() {
	if(m_context.isNull()) {
		qDebug() << "Couldn't delete resources: OpenGL context is unavailable. A memory leak might have happened.";
		return;
	}
	
	ContextSwitcher switcher(m_context);
	Q_UNUSED(switcher);
	
	Renderer::runDeleteOnAllResources();
}

MeshCacheOGL* RendererOGL::buildMeshCache(Mesh* mesh) {
	std::pair<MeshCacheOGL*, bool> mc = mesh->getOrEmplaceMeshCache<MeshCacheOGL>(rendererID());
	
	if(mc.first->isDirty())
		mc.first->update(m_gl);
	
	if(mc.second)
		addToMeshCaches(mc.first);
	
	return mc.first;
}

MaterialCacheOGL* RendererOGL::buildMaterialCache(Material* material) {
	std::pair<MaterialCacheOGL*, bool> mc = material->getOrEmplaceMaterialCache<MaterialCacheOGL>(rendererID());
	
	if(mc.first->isDirty())
		mc.first->update(m_gl);
	
	if(mc.second)
		addToMaterialCaches(mc.first);
	
	return mc.first;
}

}