/// @file
/// @brief Declares the MeshCache class used to store renderer-specific cache data for a Mesh.

#ifndef A3DMESHCACHE_H
#define A3DMESHCACHE_H

#include "common.h"
#include <QObject>

namespace A3D {

class Mesh;

/// @brief Base class for storing and managing cached data derived from a Mesh.
class MeshCache : public QObject {
	Q_OBJECT
public:
	/// @brief Constructs a MeshCache.
	/// @param[in] parent The parent Mesh associated with this cache.
	explicit MeshCache(Mesh* parent);

	/// @brief Destructor.
	~MeshCache();

	/// @brief Gets the parent Mesh associated with this cache.
	/// @return Pointer to the parent Mesh.
	Mesh* mesh() const;

	/// @brief Marks the cache as dirty, indicating that it needs to be regenerated or updated.
	void markDirty();

	/// @brief Checks whether the cache is marked as dirty.
	/// @return True if the cache is dirty; false otherwise.
	bool isDirty() const;

protected:
	/// @brief Marks the cache as clean, indicating that it is up-to-date.
	void markClean();

private:
	QPointer<Mesh> m_mesh; ///< Weak pointer to the associated Mesh.
	bool m_isDirty;        ///< Flag indicating whether the cache is dirty.
};

}

#endif // A3DMESHCACHE_H
