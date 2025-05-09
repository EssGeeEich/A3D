/// @file
/// @brief Declares the CubemapCache class used to store renderer-specific cache data for a Cubemap.

#ifndef A3DCUBEMAPCACHE_H
#define A3DCUBEMAPCACHE_H

#include "common.h"
#include <QObject>

namespace A3D {

class Cubemap;

/// @brief Base class for storing and managing cached data derived from a Cubemap.
class CubemapCache : public QObject {
	Q_OBJECT
public:
	/// @brief Constructs a CubemapCache.
	/// @param[in] parent The parent Cubemap associated with this cache.
	explicit CubemapCache(Cubemap* parent);

	/// @brief Destructor.
	~CubemapCache();

	/// @brief Gets the parent Cubemap associated with this cache.
	/// @return Pointer to the parent Cubemap.
	Cubemap* cubemap() const;

	/// @brief Marks the cache as dirty, indicating that it needs to be regenerated or updated.
	void markDirty();

	/// @brief Checks whether the cache is marked as dirty.
	/// @return True if the cache is dirty; false otherwise.
	bool isDirty() const;

protected:
	/// @brief Marks the cache as clean, indicating that it is up-to-date.
	void markClean();

private:
	QPointer<Cubemap> m_cubemap; ///< Weak pointer to the associated Cubemap.
	bool m_isDirty;              ///< Flag indicating whether the cache is dirty.
};

}

#endif // A3DCUBEMAPCACHE_H
