/// @file
/// @brief OpenGL-specific cache for storing and applying Material properties in shaders.

#ifndef A3DMATERIALCACHEOGL_H
#define A3DMATERIALCACHEOGL_H

#include "common.h"
#include "materialcache.h"
#include "materialproperties.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>

namespace A3D {

class MaterialPropertiesCacheOGL;
class RendererOGL;

/// @brief OpenGL-specific implementation of MaterialCache for managing shader uniforms and buffers.
class MaterialCacheOGL : public MaterialCache {
	Q_OBJECT
public:
	/// @brief Constructs a MaterialCacheOGL.
	/// @param[in] parent The parent Material associated with this cache.
	explicit MaterialCacheOGL(Material* parent);

	/// @brief Destructor.
	~MaterialCacheOGL();

	/// @brief Updates internal GPU resources (UBOs, textures) based on Material state.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	void update(RendererOGL* renderer, CoreGLFunctions* gl);

	/// @brief Installs shader program and binds default uniforms.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	void install(RendererOGL* renderer, CoreGLFunctions* gl);

	/// @brief Sets a single shader uniform to a given value.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	/// @param[in] name Name of the uniform variable.
	/// @param[in] value QVariant wrapping the value to assign.
	void applyUniform(RendererOGL* renderer, CoreGLFunctions* gl, QString const& name, QVariant const& value);

	/// @brief Applies multiple shader uniforms from a map.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	/// @param[in] uniforms Map of uniform names to QVariant values.
	void applyUniforms(RendererOGL* renderer, CoreGLFunctions* gl, std::map<QString, QVariant> const& uniforms);

private:
	std::unique_ptr<QOpenGLShaderProgram> m_program; ///< Shader program for the material

	/// @brief Caches uniform location and last-set value to avoid redundant updates.
	struct UniformCachedInfo {
		/// @brief Default constructor initializing uniform ID to -1.
		inline UniformCachedInfo()
			: m_uniformID(-1) {}

		int m_uniformID;      ///< Cached location ID of the uniform in the shader
		QVariant m_lastValue; ///< Most recently assigned value of the uniform
	};

	std::map<QString, UniformCachedInfo> m_uniformCachedInfo; ///< Map of uniform names to their cached info

	GLuint m_meshUBO_index;    ///< Binding index for mesh transform UBO
	GLuint m_matpropUBO_index; ///< Binding index for material properties UBO
	GLuint m_sceneUBO_index;   ///< Binding index for scene data UBO
	GLuint m_lineUBO_index;    ///< Binding index for line rendering UBO
};

}

#endif // A3DMATERIALCACHEOGL_H
