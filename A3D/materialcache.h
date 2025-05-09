/// @file
/// @brief Declares the MaterialCache class used to store renderer-specific cache data for a Material.

#ifndef A3DMATERIALCACHE_H
#define A3DMATERIALCACHE_H

#include "common.h"
#include <QObject>
#include <QPointer>

namespace A3D {

class Material;

/// @brief Base class for storing and managing cached data derived from a Material.
class MaterialCache : public QObject {
	Q_OBJECT
public:
	/// @brief Constructs a MaterialCache.
	/// @param[in] parent The parent Material associated with this cache.
	explicit MaterialCache(Material* parent);

	/// @brief Destructor.
	~MaterialCache();

	/// @brief Gets the parent Material associated with this cache.
	/// @return Pointer to the parent Material.
	Material* material() const;

	/// @brief Marks the cache as dirty, indicating that it needs to be regenerated or updated.
	void markDirty();

	/// @brief Checks whether the cache is marked as dirty.
	/// @return True if the cache is dirty; false otherwise.
	bool isDirty() const;

protected:
	/// @brief Marks the cache as clean, indicating that it is up-to-date.
	void markClean();

private:
	QPointer<Material> m_material; ///< Weak pointer to the associated Material.
	bool m_isDirty;                ///< Flag indicating whether the cache is dirty.
};

}
#endif // A3dMATERIALCACHE_H
