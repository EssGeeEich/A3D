/// @file
/// @brief Defines the Mesh resource for storing vertex and index data, with rendering options and caching.
/// @details Stores geometry data, supports optional vertex attributes, index optimization, and per-renderer caching.

#ifndef A3DMESH_H
#define A3DMESH_H

#include "common.h"
#include "meshcache.h"
#include "resource.h"

namespace A3D {

/// @brief Mesh resource storing geometry data and caching for renderers.
/// @details Supports rendering options, attribute toggles, index optimization, and caching per renderer.
class Mesh : public Resource {
	Q_OBJECT
public:
	/// Flags controlling mesh rendering behavior.
	enum RenderOption {
		NoOptions      = 0x0, ///< Default rendering with back-face culling
		DisableCulling = 0x1  ///< Disable back-face culling for this mesh
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	/// Flags indicating included vertex attributes.
	enum Content {
		Position2D     = 0x0001, ///< Vertex has 2D position
		Position3D     = 0x0002, ///< Vertex has 3D position
		TextureCoord2D = 0x0004, ///< Vertex has 2D texture coordinate
		Normal3D       = 0x0008, ///< Vertex has Normal vector
		Color3D        = 0x0010, ///< Vertex has RGB color
		Color4D        = 0x0020, ///< Vertex has RGBA color
		BoneIDs        = 0x0040, ///< Vertex has Bone ID indices for skinning
		BoneWeights    = 0x0080, ///< Vertex has Bone weight values for skinning
		SmoothingGroup = 0x0100  ///< Vertex has Smoothing group indices for lighting
	};
	Q_DECLARE_FLAGS(Contents, Content)

	/// Standard built-in meshes.
	enum StandardMesh {
		Triangle2DMesh, ///< Simple 2D triangle
		ScreenQuadMesh, ///< Fullscreen quad
		UnitQuadMesh,   ///< Unit square quad
		CubeIndexedMesh ///< Indexed cube
	};

	/// @brief Retrieves a built-in standard mesh.
	/// @param[in] mesh Identifier of the standard mesh.
	/// @return Pointer to the shared Mesh instance.
	static Mesh* standardMesh(StandardMesh mesh);

	/// Primitive topology for mesh drawing.
	enum DrawMode {
		Triangles,            ///< Unindexed triangles
		IndexedTriangles,     ///< Indexed triangles
		TriangleStrips,       ///< Triangle strips
		IndexedTriangleStrips ///< Indexed triangle strips
	};

	/// @brief Vertex definition with optional attributes.
	/// @details Contains position, normals, colors, skinning, and smoothing attributes.
	struct Vertex {
		QVector2D Position2D;        ///< 2D position attribute
		QVector3D Position3D;        ///< 3D position attribute
		QVector2D TextureCoord2D;    ///< 2D texture coordinate attribute
		QVector3D Normal3D;          ///< Normal vector attribute
		QVector3D Color3D;           ///< RGB color attribute
		QVector4D Color4D;           ///< RGBA color attribute
		std::uint8_t BoneIDs[4];     ///< Bone index array for skinning
		QVector4D BoneWeights;       ///< Bone weight array for skinning
		std::uint8_t SmoothingGroup; ///< Smoothing group index for lighting

		/// @brief Compares this vertex to another, considering only specified contents.
		/// @param[in] o The other vertex to compare.
		/// @param[in] c Flags of attributes to compare.
		/// @return True if all indicated attributes are equal.
		bool Equals(Vertex const& o, Contents c) const;
	};

	/// @brief Constructs a Mesh with an optional ResourceManager.
	/// @param[in] manager The ResourceManager that owns this mesh.
	explicit Mesh(ResourceManager* manager = nullptr);

	/// @brief Destructor.
	~Mesh();

	/// @brief Creates a deep copy of this mesh.
	/// @return Pointer to the cloned mesh.
	Mesh* clone() const;

	/// @brief Sets rendering options flags.
	/// @param[in] options Bitmask of RenderOption flags.
	void setRenderOptions(RenderOptions options);

	/// @brief Gets current rendering options.
	/// @return Bitmask of RenderOption flags.
	RenderOptions renderOptions() const;

	/// @brief Sets which vertex attributes are included.
	/// @param[in] contents Bitmask of Content flags.
	void setContents(Contents contents);

	/// @brief Gets active vertex attribute flags.
	/// @return Bitmask of Content flags.
	Contents contents() const;

	/// @brief Sets the primitive draw mode.
	/// @param[in] mode The DrawMode to use.
	void setDrawMode(DrawMode mode);

	/// @brief Retrieves the current draw mode.
	/// @return The active DrawMode.
	DrawMode drawMode() const;

	//void setBoneTransform(QString const& bone, QMatrix4x4 const& matrix);
	//void boneTransform(QString const& bone, QMatrix4x4 const& matrix);
	//std::vector<QMatrix4x4> const& boneTransforms();

	/// @brief Optimizes the index buffer for performance.
	void optimizeIndices();

	/// @brief Accesses the mutable vertex list.
	/// @return Reference to the vector of Vertex.
	std::vector<Vertex>& vertices();

	/// @brief Accesses the vertex list.
	/// @return Const reference to the vector of Vertex.
	std::vector<Vertex> const& vertices() const;

	/// @brief Retrieves packed vertex data for GPU upload.
	/// @return Const reference to packed byte array.
	std::vector<std::uint8_t> const& packedData() const;

	/// @brief Accesses the mutable index list.
	/// @return Reference to the vector of indices.
	std::vector<std::uint32_t>& indices();

	/// @brief Accesses the index list.
	/// @return Const reference to the vector of indices.
	std::vector<std::uint32_t> const& indices() const;

	/// @brief Performs ray intersection test against mesh triangles.
	/// @param[in] origin Ray origin in world coordinates.
	/// @param[in] dir Ray direction vector.
	/// @return Intersection point if hit; std::nullopt otherwise.
	std::optional<QVector3D> intersect(QVector3D origin, QVector3D dir) const;

	/// @brief Invalidates renderer-specific cache entries.
	/// @param[in] rendererID Renderer identifier to invalidate (default all).
	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	/// @brief Retrieves a typed cache object for the given renderer.
	/// @tparam T Cache subclass type.
	/// @param[in] rendererID Renderer identifier key.
	/// @return Pointer to cache object or nullptr.
	template <typename T>
	T* getMeshCacheT(std::uintptr_t rendererID) const {
		auto it = m_meshCache.find(rendererID);
		if(it == m_meshCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}

	/// @brief Retrieves or creates a cache object for the given renderer.
	/// @tparam T Cache subclass type.
	/// @param[in] rendererID Renderer identifier key.
	/// @return Pair of cache pointer and creation flag.
	template <typename T>
	std::pair<T*, bool> getOrEmplaceMeshCache(std::uintptr_t rendererID) {
		auto it = m_meshCache.find(rendererID);
		if(it == m_meshCache.end() || it->second.isNull()) {
			T* c                    = new T(this);
			m_meshCache[rendererID] = QPointer<MeshCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for Mesh.");

		return std::make_pair(c, false);
	}

	/// @brief Computes the byte size of a single packed vertex based on contents.
	/// @param[in] contents Bitmask of Contents flags.
	/// @return Size in bytes of one packed vertex.
	static std::size_t packedVertexSize(Contents contents);

private:
	DrawMode m_drawMode;                  ///< Current draw topology mode
	std::vector<Vertex> m_vertices;       ///< Vertex buffer data
	std::vector<std::uint32_t> m_indices; ///< Index buffer data
	RenderOptions m_renderOptions;        ///< Material rendering flags

	//std::map<QString, std::size_t> m_bones;
	//std::vector<QMatrix4x4> m_boneTransforms;

	Contents m_contents;                                       ///< Active vertex attribute flags
	mutable std::vector<std::uint8_t> m_packedData;            ///< Cached packed vertex array
	std::map<std::uintptr_t, QPointer<MeshCache>> m_meshCache; ///< Per-renderer cache objects
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Mesh::RenderOptions)
Q_DECLARE_OPERATORS_FOR_FLAGS(Mesh::Contents)

}
#endif // A3DMESH_H
