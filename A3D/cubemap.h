/// @file
/// @brief Declares the Cubemap class for managing cube map textures used in rendering.

#ifndef A3DCUBEMAP_H
#define A3DCUBEMAP_H

#include "common.h"
#include <QObject>
#include "texture.h"
#include "cubemapcache.h"
#include "resource.h"
#include "image.h"

namespace A3D {

/// @brief Represents a cube map texture resource used for environment mapping.
class Cubemap : public Resource {
	Q_OBJECT
public:
	/// @brief Identifies the six faces of a cube map.
	enum CubemapFace {
		CF_NEG_X = 0, ///< Negative X face
		CF_NEG_Y,     ///< Negative Y face
		CF_NEG_Z,     ///< Negative Z face
		CF_POS_X,     ///< Positive X face
		CF_POS_Y,     ///< Positive Y face
		CF_POS_Z,     ///< Positive Z face
		CF_COUNT,     ///< Total number of cube map faces
	};

	/// @brief Constructs an empty Cubemap resource.
	/// @param[in] manager The resource manager that owns this cubemap.
	explicit Cubemap(ResourceManager* manager = nullptr);

	/// @brief Destroys the Cubemap.
	~Cubemap();

	/// @brief Creates a deep copy of this cubemap.
	/// @return A new Cubemap object that is a clone of this one.
	Cubemap* clone() const;

	/// @brief Sets the image for a specific face of the cube map.
	/// @param[in] face The face of the cube to set.
	/// @param[in] image The image to set on the specified face.
	void setCubemapFace(CubemapFace face, Image const& image);

	/// @brief Gets the image of a specific face of the cube map.
	/// @param[in] face The face to retrieve.
	/// @return The image assigned to the specified face.
	Image const& cubemapFace(CubemapFace face) const;

	/// @brief Checks if all cube map faces are valid.
	/// @return True if all faces are initialized and valid; false otherwise.
	bool isValid() const;

	/// @brief Invalidates renderer-specific cache entries.
	/// @param[in] rendererID Renderer identifier to invalidate (default all).
	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	/// @brief Retrieves a cached CubemapCache object for a specific renderer ID.
	/// @tparam T The type of CubemapCache.
	/// @param[in] rendererID The renderer ID used to retrieve the cache.
	/// @return A pointer to the casted CubemapCache object, or nullptr if not found or invalid.
	template <typename T>
	T* getCubemapCacheT(std::uintptr_t rendererID) const {
		auto it = m_cubemapCache.find(rendererID);
		if(it == m_cubemapCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}

	/// @brief Retrieves or creates and caches a CubemapCache object for the given renderer ID.
	/// @tparam T The type of CubemapCache to create.
	/// @param[in] rendererID The renderer ID for which to retrieve or create the cache.
	/// @return A pair consisting of the pointer to the CubemapCache and a boolean indicating whether it was newly created.
	template <typename T>
	std::pair<T*, bool> getOrEmplaceCubemapCache(std::uintptr_t rendererID) {
		auto it = m_cubemapCache.find(rendererID);
		if(it == m_cubemapCache.end() || it->second.isNull()) {
			T* c                       = new T(this);
			m_cubemapCache[rendererID] = QPointer<CubemapCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for Cubemap.");

		return std::make_pair(c, false);
	}

private:
	Image m_faces[CF_COUNT]; ///< Array of images representing the 6 faces of the cube map.

	std::map<std::uintptr_t, QPointer<CubemapCache>> m_cubemapCache; ///< Map of renderer-specific cached cube map resources.
};

}

#endif // A3DCUBEMAP_H
