/// @file
/// @brief Declares the LineGroupCacheOGL class used for OpenGL-specific LineGroup caching.

#ifndef A3DLINEGROUPCACHEOGL_H
#define A3DLINEGROUPCACHEOGL_H

#include "common.h"
#include "linegroupcache.h"
#include "linegroup.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

namespace A3D {

class RendererOGL;

/// @brief OpenGL-specific implementation of LineGroupCache for storing and binding LineGroup meshes in the rendering pipeline.
class LineGroupCacheOGL : public LineGroupCache {
	Q_OBJECT
public:
	/// @brief OpenGL attribute indices used for vertex attributes.
	enum {
		Position3DAttribute = 0, ///< Index for 3D vertex position attribute
		Position2DAttribute = 1, ///< Index for 2D vertex position attribute
		Color3DAttribute    = 2, ///< Index for 3D color attribute
		Color4DAttribute    = 3  ///< Index for 4D color (with alpha) attribute
	};

	/// @brief Constructs a LineGroupCacheOGL.
	/// @param[in] parent The parent LineGroup associated with this cache.
	explicit LineGroupCacheOGL(LineGroup* parent);

	/// @brief Destructor.
	~LineGroupCacheOGL();

	/// @brief Updates the OpenGL linegroup buffers based on the current LineGroup data.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	void update(RendererOGL* renderer, CoreGLFunctions* gl);

	/// @brief Binds and renders the LineGroup using the cached OpenGL data.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	/// @param[in] modelMatrix Model transformation matrix.
	/// @param[in] viewMatrix View transformation matrix.
	/// @param[in] projMatrix Projection transformation matrix.
	void render(RendererOGL* renderer, CoreGLFunctions* gl, QMatrix4x4 const& modelMatrix, QMatrix4x4 const& viewMatrix, QMatrix4x4 const& projMatrix);

private:
	LineGroup::DrawMode m_drawMode; ///< Draw mode (Lines, LineStrips, etc.)
	QOpenGLVertexArrayObject m_vao; ///< OpenGL VAO for managing state
	QOpenGLBuffer m_vbo;            ///< Vertex buffer object
	QOpenGLBuffer m_ibo;            ///< Index buffer object
	std::size_t m_elementCount;     ///< Number of elements to render
	GLenum m_iboFormat;             ///< Format of the index buffer (e.g., GL_UNSIGNED_INT)

	/// @brief Uniform buffer data for mesh transforms.
	struct MeshUBO_Data {
		RawMatrix4x4 pMatrix;         ///< Projection matrix
		RawMatrix4x4 vMatrix;         ///< View matrix
		RawMatrix4x4 mMatrix;         ///< Model matrix
		RawMatrix4x4 mvMatrix;        ///< Model-View matrix
		RawMatrix4x4 mvpMatrix;       ///< Model-View-Projection matrix
		RawMatrix4x4 mNormalMatrix;   ///< Normal matrix from Model matrix
		RawMatrix4x4 mvNormalMatrix;  ///< Normal matrix from Model-View matrix
		RawMatrix4x4 mvpNormalMatrix; ///< Normal matrix from Model-View-Projection matrix
	};

	/// @brief Uniform buffer data for line-specific settings.
	struct LineUBO_Data {
		float LineThickness; ///< Line width in world units
		float FeatherSize;   ///< Anti-aliasing feather size
	};

	MeshUBO_Data m_meshUBO_data; ///< Uniform buffer data for mesh transform data.
	LineUBO_Data m_lineUBO_data; ///< Uniform buffer data for linegroup data.
	GLuint m_meshUBO;            ///< OpenGL buffer ID for mesh uniform data
	GLuint m_lineUBO;            ///< OpenGL buffer ID for line uniform data
};

}

#endif // A3DLINEGROUPCACHEOGL_H
