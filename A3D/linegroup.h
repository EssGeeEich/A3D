/// @file
/// @brief Declares the LineGroup class for rendering collections of lines with customizable vertex data and draw modes.
/// @details LineGroup stores vertices, indices, and rendering parameters, and supports caching per renderer.

#ifndef A3DLINEGROUP_H
#define A3DLINEGROUP_H

#include "common.h"
#include "linegroupcache.h"
#include "resource.h"

namespace A3D {

/// @brief Manages a set of line primitives with configurable vertex attributes and draw modes.
/// @details Supports 2D/3D positions, color attributes, index optimization, and renderer-specific caching.
class LineGroup : public Resource {
	Q_OBJECT
public:
	/// @brief Flags indicating which vertex attributes are present.
	enum Content {
		Position2D = 0x0001, ///< Vertex has 2D position
		Position3D = 0x0002, ///< Vertex has 3D position
		Color3D    = 0x0004, ///< Vertex has RGB color
		Color4D    = 0x0008, ///< Vertex has RGBA color
	};
	Q_DECLARE_FLAGS(Contents, Content)

	/// @brief Primitive topology for rendering the lines.
	enum DrawMode {
		Lines,             ///< Unconnected line segments
		IndexedLines,      ///< Indexed line segments
		LineStrips,        ///< Continuous line strip
		IndexedLineStrips, ///< Indexed line strip
	};

	/// @brief Represents a single line vertex with optional attributes.
	struct Vertex {
		QVector2D Position2D; ///< 2D position attribute
		QVector3D Position3D; ///< 3D position attribute
		QVector3D Color3D;    ///< RGB color attribute
		QVector4D Color4D;    ///< RGBA color attribute

		/// @brief Compares this vertex to another, considering only specified contents.
		/// @param[in] o The other vertex to compare.
		/// @param[in] c Flags of attributes to compare.
		/// @return True if all indicated attributes are equal.
		bool Equals(Vertex const& o, Contents c) const;
	};

	/// @brief Constructs an empty LineGroup resource.
	/// @param[in] manager Optional ResourceManager owning this resource.
	explicit LineGroup(ResourceManager* manager = nullptr);

	/// @brief Destructor.
	~LineGroup();

	/// @brief Creates a deep copy of this LineGroup.
	/// @return Pointer to the cloned LineGroup.
	LineGroup* clone() const;

	/// @brief Sets line thickness for rendering.
	/// @param[in] value Thickness in world units.
	void setThickness(float value);

	/// @brief Retrieves current line thickness.
	/// @return Thickness value.
	float thickness() const;

	/// @brief Specifies which vertex attributes to include.
	/// @param[in] contents Bitmask of Contents flags.
	void setContents(Contents contents);

	/// @brief Gets the active vertex attribute flags.
	/// @return Bitmask of Contents.
	Contents contents() const;

	/// @brief Sets the primitive draw mode.
	/// @param[in] mode The DrawMode to use.
	void setDrawMode(DrawMode mode);

	/// @brief Retrieves the current draw mode.
	/// @return The active DrawMode.
	DrawMode drawMode() const;

	/// @brief Optimizes index array for rendering performance.
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

	/// @brief Invalidates renderer-specific cache entries.
	/// @param[in] rendererID Renderer identifier to invalidate (default all).
	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	/// @brief Retrieves a typed cache object for the given renderer.
	/// @tparam T Cache subclass type.
	/// @param[in] rendererID Renderer identifier key.
	/// @return Pointer to cache object or nullptr.
	template <typename T>
	T* getLineGroupCacheT(std::uintptr_t rendererID) const {
		auto it = m_lineGroupCache.find(rendererID);
		if(it == m_lineGroupCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}

	/// @brief Retrieves or creates a cache object for the given renderer.
	/// @tparam T Cache subclass type.
	/// @param[in] rendererID Renderer identifier key.
	/// @return Pair of cache pointer and creation flag.
	template <typename T>
	std::pair<T*, bool> getOrEmplaceLineGroupCache(std::uintptr_t rendererID) {
		auto it = m_lineGroupCache.find(rendererID);
		if(it == m_lineGroupCache.end() || it->second.isNull()) {
			T* c                         = new T(this);
			m_lineGroupCache[rendererID] = QPointer<LineGroupCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for LineGroup.");

		return std::make_pair(c, false);
	}

	/// @brief Computes the byte size of a single packed vertex based on contents.
	/// @param[in] contents Bitmask of Contents flags.
	/// @return Size in bytes of one packed vertex.
	static std::size_t packedVertexSize(Contents contents);

private:
	DrawMode m_drawMode;                  ///< Current primitive draw mode
	std::vector<Vertex> m_vertices;       ///< Vertex list
	std::vector<std::uint32_t> m_indices; ///< Index list
	float m_thickness;                    ///< Line thickness

	Contents m_contents;                                                 ///< Active vertex attribute flags
	mutable std::vector<std::uint8_t> m_packedData;                      ///< Cached packed vertex data
	std::map<std::uintptr_t, QPointer<LineGroupCache>> m_lineGroupCache; ///< Renderer-specific cache
};

Q_DECLARE_OPERATORS_FOR_FLAGS(LineGroup::Contents)

}

#endif // A3DLINEGROUP_H
