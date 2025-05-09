/// @file
/// @brief Declares the MaterialPropertiesCacheOGL class used for OpenGL-specific material properties caching, handling uniform buffer updates.

#ifndef A3DMATERIALPROPERTIESCACHEOGL_H
#define A3DMATERIALPROPERTIESCACHEOGL_H

#include "common.h"
#include "materialproperties.h"
#include "materialpropertiescache.h"
#include <QOpenGLBuffer>

namespace A3D {

class RendererOGL;
class MaterialCacheOGL;

/// @brief Manages GPU-side caching of MaterialProperties for OpenGL rendering.
class MaterialPropertiesCacheOGL : public MaterialPropertiesCache {
	Q_OBJECT
public:
	/// @brief Constructs a MaterialPropertiesCacheOGL.
	/// @param[in] parent The parent MaterialProperties associated with this cache.
	explicit MaterialPropertiesCacheOGL(MaterialProperties* parent);

	/// @brief Destructor.
	~MaterialPropertiesCacheOGL();

	/// @brief Updates GPU buffers based on current MaterialProperties state.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	void update(RendererOGL* renderer, CoreGLFunctions* gl);

	/// @brief Installs the material properties into the shader via UBOs.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	/// @param[in] materialCache OpenGL-specific material cache containing shader program.
	void install(RendererOGL* renderer, CoreGLFunctions* gl, MaterialCacheOGL* materialCache);

private:
	/// Data structure matching shader uniform block for material properties.
	struct MaterialUBO_Data {
		QVector4D placeholder; ///< Placeholder field, expand with real properties as needed
	};

	MaterialUBO_Data m_materialUBO_data; ///< CPU-side data for material UBO synchronization
	GLuint m_materialUBO;                ///< OpenGL buffer ID for the material properties UBO
};

}

#endif // A3DMATERIALPROPERTIESCACHEOGL_H
