/// @file
/// @brief Declares the MaterialPropertiesCache class used to store renderer-specific cache data for Material Properties.

#ifndef A3DMATERIALPROPERTIESCACHE_H
#define A3DMATERIALPROPERTIESCACHE_H

#include "common.h"
#include <QObject>
#include <QPointer>

namespace A3D {

class MaterialProperties;

/// @brief Base class for storing and managing cached data derived from MaterialProperties.
class MaterialPropertiesCache : public QObject {
	Q_OBJECT
public:
	/// @brief Constructs a MaterialPropertiesCache.
	/// @param[in] parent The parent MaterialProperties associated with this cache.
	explicit MaterialPropertiesCache(MaterialProperties* parent);

	/// @brief Destructor.
	~MaterialPropertiesCache();

	/// @brief Gets the parent MaterialProperties associated with this cache.
	/// @return Pointer to the parent MaterialProperties.
	MaterialProperties* materialProperties() const;

	/// @brief Marks the cache as dirty, indicating that it needs to be regenerated or updated.
	void markDirty();

	/// @brief Checks whether the cache is marked as dirty.
	/// @return True if the cache is dirty; false otherwise.
	bool isDirty() const;

protected:
	/// @brief Marks the cache as clean, indicating that it is up-to-date.
	void markClean();

private:
	QPointer<MaterialProperties> m_materialProperties; ///< Weak pointer to the associated MaterialProperties.
	bool m_isDirty;                                    ///< Flag indicating whether the cache is dirty.
};

}

#endif // A3DMATERIALPROPERTIESCACHE_H
