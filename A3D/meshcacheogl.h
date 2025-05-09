/// @file
/// @brief Declares the MeshCacheOGL class used for OpenGL-specific Mesh caching.

#ifndef A3DMESHCACHEOGL_H
#define A3DMESHCACHEOGL_H

#include "common.h"
#include "meshcache.h"
#include "mesh.h"
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>

namespace A3D {

class RendererOGL;

/// @brief OpenGL implementation of MeshCache that manages GPU meshes.
class MeshCacheOGL : public MeshCache {
	Q_OBJECT
public:
	/// OpenGL attribute indices mapping for vertex attributes.
	enum {
		Position3DAttribute     = 0, ///< 3D position attribute index
		Position2DAttribute     = 1, ///< 2D position attribute index
		TextureCoord2DAttribute = 2, ///< 2D texture coordinate attribute index
		Normal3DAttribute       = 3, ///< Normal vector attribute index
		Color3DAttribute        = 4, ///< RGB color attribute index
		Color4DAttribute        = 5, ///< RGBA color attribute index
		BoneIDAttribute         = 6, ///< Bone ID array attribute index for skinning
		BoneWeightsAttribute    = 7, ///< Bone weights array attribute index for skinning
		SmoothingGroupAttribute = 8  ///< Smoothing group attribute index
	};

	/// @brief Constructs a MeshCacheOGL.
	/// @param[in] parent The parent Mesh associated with this cache.
	explicit MeshCacheOGL(Mesh* parent);

	/// @brief Destructor.
	~MeshCacheOGL();

	/// @brief Updates the OpenGL buffer contents from the current Mesh data.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	void update(RendererOGL* renderer, CoreGLFunctions* gl);

	/// @brief Renders the mesh using cached OpenGL resources.
	/// @param[in] renderer Pointer to the OpenGL renderer.
	/// @param[in] gl Pointer to OpenGL core functions.
	/// @param[in] modelMatrix World transform matrix.
	/// @param[in] viewMatrix View transform matrix.
	/// @param[in] projMatrix Projection transform matrix.
	void render(RendererOGL* renderer, CoreGLFunctions* gl, QMatrix4x4 const& modelMatrix, QMatrix4x4 const& viewMatrix, QMatrix4x4 const& projMatrix);

private:
	Mesh::DrawMode m_drawMode;      ///< Draw mode topology
	QOpenGLVertexArrayObject m_vao; ///< Vertex Array Object for attribute state
	QOpenGLBuffer m_vbo;            ///< Vertex Buffer Object for vertex data
	QOpenGLBuffer m_ibo;            ///< Index Buffer Object for index data
	std::size_t m_elementCount;     ///< Number of elements to render
	GLenum m_iboFormat;             ///< OpenGL format used for index buffer (e.g., GL_UNSIGNED_INT)

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

	MeshUBO_Data m_meshUBO_data; ///< Uniform buffer data for mesh transform data.
	GLuint m_meshUBO;            ///< OpenGL buffer ID for mesh uniform data
};

}

#endif // A3DMESHCACHEOGL_H
