/// @file
/// @brief Declares the LineGroupCache class used to store renderer-specific cache data for a LineGroup.

#ifndef A3DLINEGROUPCACHE_H
#define A3DLINEGROUPCACHE_H

#include "common.h"
#include <QObject>

namespace A3D {

class LineGroup;

/// @brief Base class for storing and managing cached data derived from a LineGroup.
class LineGroupCache : public QObject {
	Q_OBJECT
public:
	/// @brief Constructs a LineGroupCache.
	/// @param[in] parent The parent LineGroup associated with this cache.
	explicit LineGroupCache(LineGroup* parent);

	/// @brief Destructor.
	~LineGroupCache();

	/// @brief Gets the parent LineGroup associated with this cache.
	/// @return Pointer to the parent LineGroup.
	LineGroup* lineGroup() const;

	/// @brief Marks the cache as dirty, indicating that it needs to be regenerated or updated.
	void markDirty();

	/// @brief Checks whether the cache is marked as dirty.
	/// @return True if the cache is dirty; false otherwise.
	bool isDirty() const;

protected:
	/// @brief Marks the cache as clean, indicating that it is up-to-date.
	void markClean();

private:
	QPointer<LineGroup> m_lineGroup; ///< Weak pointer to the associated LineGroup.
	bool m_isDirty;                  ///< Flag indicating whether the cache is dirty.
};

}

#endif // A3DLINEGROUPCACHE_H
