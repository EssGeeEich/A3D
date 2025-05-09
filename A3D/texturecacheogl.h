/// @file
/// @brief Declares the TextureCacheOGL class used for OpenGL-specific Texture caching.

#ifndef A3DTEXTURECACHEOGL_H
#define A3DTEXTURECACHEOGL_H

#include "common.h"
#include "texturecache.h"
#include <QOpenGLTexture>

namespace A3D {

class RendererOGL;

/// @brief OpenGL implementation of TextureCache that manages GPU texture resources.
///
/// This class wraps a QOpenGLTexture and synchronizes it with the CPU‑side Texture
/// data. It can update the underlying GL texture and bind it to a specified slot.
class TextureCacheOGL : public TextureCache {
	Q_OBJECT
public:
	/// @brief Constructs a TextureCacheOGL.
	/// @param[in] parent The parent Texture associated with this cache.
	explicit TextureCacheOGL(Texture* parent);

	/// @brief Destructor.
	~TextureCacheOGL();

	/// @brief Updates the OpenGL texture contents from the current Texture data.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	void update(RendererOGL* renderer, CoreGLFunctions* gl);

	/// @brief Binds the cached texture to the specified texture unit.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	/// @param[in] slot The texture unit index (e.g., 0 for GL_TEXTURE0).
	void applyToSlot(RendererOGL* renderer, CoreGLFunctions* gl, GLuint slot);

private:
	std::unique_ptr<QOpenGLTexture> m_texture; ///< The Qt wrapper for the OpenGL texture object.
};

}

#endif // A3DTEXTURECACHEOGL_H
