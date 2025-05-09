/// @file
/// @brief Declares the TextureCache class used to store renderer-specific cache data for a Texture.

#ifndef A3DTEXTURECACHE_H
#define A3DTEXTURECACHE_H

#include "common.h"
#include <QObject>

namespace A3D {

class Texture;

/// @brief Base class for storing and managing cached data derived from a Texture.
class TextureCache : public QObject {
	Q_OBJECT
public:
	/// @brief Constructs a TextureCache.
	/// @param[in] parent The parent Texture associated with this cache.
	explicit TextureCache(Texture* parent);

	/// @brief Destructor.
	~TextureCache();

	/// @brief Gets the parent Texture associated with this cache.
	/// @return Pointer to the parent Texture.
	Texture* texture() const;

	/// @brief Marks the cache as dirty, indicating that it needs to be regenerated or updated.
	void markDirty();

	/// @brief Checks whether the cache is marked as dirty.
	/// @return True if the cache is dirty; false otherwise.
	bool isDirty() const;

protected:
	/// @brief Marks the cache as clean, indicating that it is up-to-date.
	void markClean();

private:
	QPointer<Texture> m_texture; ///< Weak pointer to the associated Texture.
	bool m_isDirty;              ///< Flag indicating whether the cache is dirty.
};

}

#endif // A3DTEXTURECACHE_H
