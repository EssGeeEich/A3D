/// @file
/// @brief Defines the MaterialProperties class used for storing material-specific values and textures.

#ifndef A3DMATERIALPROPERTIES_H
#define A3DMATERIALPROPERTIES_H

#include "common.h"
#include <QObject>
#include "material.h"
#include "resource.h"
#include "materialpropertiescache.h"

namespace A3D {

/// @brief Represents physical material properties, including shader parameters and textures.
class MaterialProperties : public Resource {
	Q_OBJECT
public:
	/// Available texture slots corresponding to different material maps.
	enum TextureSlot {
		AlbedoTextureSlot,    ///< Base color texture
		NormalTextureSlot,    ///< Normal map texture
		MetallicTextureSlot,  ///< Metallic map texture
		RoughnessTextureSlot, ///< Roughness map texture
		AOTextureSlot,        ///< Ambient occlusion texture

		EnvironmentTextureSlot, ///< Irradiance map or cubemap environment texture
		PrefilterTextureSlot,   ///< Prefiltered environment map
		BrdfTextureSlot,        ///< BRDF lookup texture

		MaxTextures = 8 ///< Maximum number of supported textures
	};

	/// @brief Constructs a new MaterialProperties resource.
	/// @param[in] manager Optional resource manager owning this resource.
	MaterialProperties(ResourceManager* manager = nullptr);

	/// @brief Destructor.
	~MaterialProperties();

	/// @brief Creates a deep clone of this material properties object.
	/// @return Pointer to the cloned instance.
	MaterialProperties* clone() const;

	/// @brief Gets the texture assigned to a given slot.
	/// @param[in] slot Texture slot identifier.
	/// @return Pointer to the assigned texture, or nullptr.
	Texture* texture(TextureSlot slot) const;

	/// @brief Assigns a texture to a specific slot.
	/// @param[in] texture Texture to assign.
	/// @param[in] slot Target texture slot.
	void setTexture(Texture* texture, TextureSlot slot);

	/// @brief Retrieves a previously set raw shader parameter value by name.
	/// @param[in] name Parameter key.
	/// @param[in] fallback Optional default value if key is not found.
	/// @return The value or fallback.
	QVariant rawValue(QString name, QVariant fallback = QVariant());

	/// @brief Sets a raw shader parameter value.
	/// @param[in] name Parameter key.
	/// @param[in] value Value to assign.
	void setRawValue(QString name, QVariant value);

	/// @brief Returns the map of all raw shader parameter values (const).
	/// @return Const reference to the vector of all raw shader parameter values.
	std::map<QString, QVariant> const& rawValues() const;

	/// @brief Returns the map of all raw shader parameter values (modifiable).
	/// @return Reference to the vector of all raw shader parameter values.
	std::map<QString, QVariant>& rawValues();

	/// @brief Sets this material to always render as translucent.
	/// @param[in] always True to force translucency.
	void setAlwaysTranslucent(bool always);

	/// @brief Checks if the material is marked as always translucent.
	/// @return True if marked, false otherwise.
	bool isTranslucent() const;

	/// @brief Invalidates renderer-specific cache entries.
	/// @param[in] rendererID Renderer identifier to invalidate (default all).
	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	/// @brief Retrieves a typed cache object for the given renderer.
	/// @tparam T Cache subclass type.
	/// @param[in] rendererID Renderer identifier key.
	/// @return Pointer to cache object or nullptr.
	template <typename T>
	T* getMaterialPropertiesCacheT(std::uintptr_t rendererID) const {
		auto it = m_materialPropertiesCache.find(rendererID);
		if(it == m_materialPropertiesCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}

	/// @brief Retrieves or creates a cache object for the given renderer.
	/// @tparam T Cache subclass type.
	/// @param[in] rendererID Renderer identifier key.
	/// @return Pair of cache pointer and creation flag.
	template <typename T>
	std::pair<T*, bool> getOrEmplaceMaterialPropertiesCache(std::uintptr_t rendererID) {
		auto it = m_materialPropertiesCache.find(rendererID);
		if(it == m_materialPropertiesCache.end() || it->second.isNull()) {
			T* c                                  = new T(this);
			m_materialPropertiesCache[rendererID] = QPointer<MaterialPropertiesCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for MaterialProperties.");

		return std::make_pair(c, false);
	}

private:
	bool m_alwaysTranslucent;                  ///< If true, always renders as translucent
	std::map<QString, QVariant> m_rawValues;   ///< Map of raw shader parameter values
	QPointer<Texture> m_textures[MaxTextures]; ///< Array of texture pointers

	std::map<std::uintptr_t, QPointer<MaterialPropertiesCache>> m_materialPropertiesCache; ///< Renderer-specific property caches
};

}

#endif // A3DMATERIALPROPERTIES_H
