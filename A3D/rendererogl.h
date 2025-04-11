#ifndef A3DRENDEREROGL_H
#define A3DRENDEREROGL_H

#include "common.h"
#include "renderer.h"
#include "meshcacheogl.h"
#include "materialcacheogl.h"
#include "materialpropertiescacheogl.h"
#include "texturecacheogl.h"
#include "cubemapcacheogl.h"
#include "linegroupcacheogl.h"
#include <queue>
#include <stack>

namespace A3D {

class RendererOGL final : public Renderer {
public:
	enum UBOBindings {
		UBO_MeshBinding               = 0,
		UBO_MaterialPropertiesBinding = 1,
		UBO_SceneBinding              = 2,
		UBO_LineBinding               = 3,
	};

	RendererOGL(QOpenGLContext*, CoreGLFunctions*);
	~RendererOGL();

	virtual void Draw(Group*, DrawInfo const&) override;
	virtual void PreLoadEntity(Entity*) override;
	virtual void Delete(MeshCache*) override;
	virtual void Delete(MaterialCache*) override;
	virtual void Delete(MaterialPropertiesCache*) override;
	virtual void Delete(TextureCache*) override;
	virtual void Delete(CubemapCache*) override;
	virtual void Delete(LineGroupCache*) override;
	virtual void DeleteAllResources() override;

	class DeferredCaller {
	public:
		inline DeferredCaller(std::function<void()> f)
			: m_f(std::move(f)) {}
		~DeferredCaller() {
			if(m_f)
				m_f();
		}

	private:
		std::function<void()> m_f;
		DeferredCaller(DeferredCaller const&)            = delete;
		DeferredCaller(DeferredCaller&&)                 = delete;
		DeferredCaller& operator=(DeferredCaller const&) = delete;
		DeferredCaller& operator=(DeferredCaller&&)      = delete;
	};

protected:
	virtual void BeginDrawing(Camera const&, Scene const*) override;
	virtual void EndDrawing(Scene const*) override;

	virtual void BeginOpaque() override;
	virtual void EndOpaque() override;
	virtual void BeginTranslucent() override;
	virtual void EndTranslucent() override;

private:
	friend class MeshCacheOGL;
	friend class MaterialCacheOGL;
	friend class MaterialPropertiesCacheOGL;
	friend class TextureCacheOGL;
	friend class CubemapCacheOGL;
	friend class LineGroupCacheOGL;

	void pushState(bool withFramebuffer);
	void popState();
	std::shared_ptr<DeferredCaller> checkGlErrors(QString const& context);

	struct StateStorage {
		GLint m_viewport[4];
		std::map<GLenum, GLboolean> m_features;
		std::map<GLenum, GLint> m_textureBindings;
		GLenum m_activeTexture;
		GLint m_drawFramebuffer;
		GLint m_readFramebuffer;
		GLint m_program;
		GLboolean m_depthMask;

		GLuint m_newFramebuffer;
	};

	std::stack<StateStorage> m_stateStorage;

	enum { LightCount = 4 };

	void RefreshSceneUBO();
	void DrawOpaque(Entity* root, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix, std::deque<Entity*>* translucentList);
	void DrawTranslucent(std::deque<Entity*>& entities, QMatrix4x4 const& parentMatrix, QMatrix4x4 const& projMatrix, QMatrix4x4 const& viewMatrix);

	MeshCacheOGL* buildMeshCache(Mesh*);
	MaterialCacheOGL* buildMaterialCache(Material*);
	MaterialPropertiesCacheOGL* buildMaterialPropertiesCache(MaterialProperties*);
	TextureCacheOGL* buildTextureCache(Texture*);
	CubemapCacheOGL* buildCubemapCache(Cubemap*);
	LineGroupCacheOGL* buildLineGroupCache(LineGroup*);

	// Will draw on screen for one frame!
	// Should be called when a framebuffer is active, and you have to assume that the framebuffer
	// will be dirty afterwards.
	// Perfect opportunity to call this during Cubemap update().
	void genBrdfLUT();

	GLuint getBrdfLUT();

	// OpenGL Data
	QPointer<QOpenGLContext> m_context;
	CoreGLFunctions* m_gl;
	std::vector<Entity*> m_translucentEntityBuffer;
	std::vector<std::pair<std::size_t, PointLightInfo>> m_closestSceneLightsBuffer;

	// OIT Data
	GLuint m_oitFBO;
	GLuint m_oitAccumTexture;
	GLuint m_oitRevealageTexture;
	GLuint m_oitDepthRBO;

	// Line data
	A3D::Material* m_lineMaterial;

	// Skybox data
	A3D::Material* m_skyboxMaterial;
	A3D::Mesh* m_skyboxMesh;
	QMatrix4x4 m_skyboxView;
	QMatrix4x4 m_skyboxProj;

	// Scene data
	struct SceneUBO_Data {
		QVector4D m_cameraPos;

		QVector4D m_lightPos[4];
		QVector4D m_lightColor[4];
	};
	SceneUBO_Data m_sceneData;
	GLuint m_sceneUBO;

	// BRDF precalculated LUT
	bool m_brdfCalculated;
	GLuint m_brdfLUT;
};

}

#endif // A3DRENDEREROGL_H
