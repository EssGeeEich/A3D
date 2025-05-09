/// @file
/// @brief Declares the CubemapCacheOGL class used for OpenGL-specific cubemap caching and operations.

#ifndef A3DCUBEMAPCACHEOGL_H
#define A3DCUBEMAPCACHEOGL_H

#include "common.h"
#include "cubemapcache.h"
#include <QOpenGLTexture>

namespace A3D {

class RendererOGL;

/// @brief OpenGL-specific implementation of CubemapCache for storing and binding cubemap textures in the rendering pipeline.
/// @details Also handles the creation and binding of environment, irradiance, and prefiltered cubemap textures for use in OpenGL rendering pipelines.
class CubemapCacheOGL : public CubemapCache {
	Q_OBJECT
public:
	/// @brief Constructs a CubemapCacheOGL.
	/// @param[in] parent The parent Cubemap associated with this cache.
	explicit CubemapCacheOGL(Cubemap* parent);

	/// @brief Destructor.
	~CubemapCacheOGL();

	/// @brief Updates the OpenGL cubemap textures based on the current Cubemap data.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	void update(RendererOGL* renderer, CoreGLFunctions* gl);

	/// @brief Binds the cubemap and associated maps to the provided texture slots.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	/// @param[in] environmentSlot Texture unit slot for the environment cubemap.
	/// @param[in] irradianceSlot Texture unit slot for the irradiance cubemap.
	/// @param[in] prefilterSlot Texture unit slot for the prefiltered cubemap.
	void applyToSlot(RendererOGL* renderer, CoreGLFunctions* gl, GLint environmentSlot, GLint irradianceSlot, GLint prefilterSlot);

private:
	/// @brief Calculates the irradiance map from the environment cubemap.
	/// @param[in] format OpenGL internal format for the texture.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	void calcIrradiance(GLenum format, RendererOGL* renderer, CoreGLFunctions* gl);

	/// @brief Calculates the prefiltered environment map from the environment cubemap.
	/// @param[in] format OpenGL internal format for the texture.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	void calcPrefilter(GLenum format, RendererOGL* renderer, CoreGLFunctions* gl);

	GLuint m_cubemap;           ///< OpenGL texture ID for the environment cubemap.
	GLuint m_cubemapIrradiance; ///< OpenGL texture ID for the irradiance cubemap.
	GLuint m_cubemapPrefilter;  ///< OpenGL texture ID for the prefiltered cubemap.
};

}

#endif // A3DCUBEMAPCACHEOGL_H
