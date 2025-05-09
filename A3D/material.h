/// @file
/// @brief Defines a Material resource for managing shaders and render options.

#ifndef A3DMATERIAL_H
#define A3DMATERIAL_H

#include "common.h"
#include <QObject>
#include <QVariant>
#include <map>
#include <cstdint>
#include <stdexcept>
#include "materialcache.h"
#include "resource.h"

namespace A3D {

/// @brief Defines a shading Material with customizable shaders and render options.
class Material : public Resource {
	Q_OBJECT
public:
	/// Rendering flags for materials.
	enum RenderOption {
		NoOptions   = 0x0, ///< Default opaque rendering
		Translucent = 0x1  ///< Render in separate pass for translucency
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	/// Standard built-in material types.
	enum StandardMaterial {
		SkyboxMaterial,     ///< Material for skyboxes
		IrradianceMaterial, ///< Material generating irradiance maps
		PrefilterMaterial,  ///< Material generating prefilter maps
		BRDFMaterial,       ///< Material for BRDF lookup
		MapChart3DMaterial, ///< Material for 3D map charts
		UnshadedMaterial,   ///< Material without lighting
		PBRMaterial,        ///< Physically based rendering material
		BillboardMaterial,  ///< Material for billboards
		LineMaterial        ///< Material for lines
	};

	/// Shader language modes supported.
	enum ShaderMode {
		GLSL ///< OpenGL Shading Language
	};

	/// Shader stage types.
	enum ShaderType {
		GeometryShader, ///< Geometry shader stage
		VertexShader,   ///< Vertex shader stage
		FragmentShader  ///< Fragment shader stage
	};

	/// @brief Retrieves a built-in standard material.
	/// @param[in] material The standard material identifier.
	/// @return Pointer to the shared Material instance.
	static Material* standardMaterial(StandardMaterial material);

	/// @brief Constructs a new Material resource.
	/// @param[in] manager Optional ResourceManager owning this resource.
	explicit Material(ResourceManager* manager = nullptr);

	/// @brief Destructor.
	~Material();

	/// @brief Creates a deep copy of this material.
	/// @return Pointer to the cloned Material.
	Material* clone() const;

	/// @brief Retrieves the current render options.
	/// @return Bitmask of RenderOptions flags.
	RenderOptions renderOptions() const;

	/// @brief Sets rendering options for this material.
	/// @param[in] options Bitmask of RenderOptions to apply.
	void setRenderOptions(RenderOptions options);

	/// @brief Assigns shader source code for a shader stage.
	/// @param[in] mode Shader language mode (e.g., GLSL).
	/// @param[in] type Shader stage (vertex, fragment, etc.).
	/// @param[in] shaderContents The shader source code.
	void setShader(ShaderMode mode, ShaderType type, QString shaderContents);

	/// @brief Loads shader from file for a shader stage.
	/// @param[in] mode Shader language mode.
	/// @param[in] type Shader stage.
	/// @param[in] shaderPath Filesystem path to the shader file.
	void setShaderFile(ShaderMode mode, ShaderType type, QString shaderPath);

	/// @brief Retrieves stored shader source code.
	/// @param[in] mode Shader language mode.
	/// @param[in] type Shader stage.
	/// @return Shader source as QString.
	QString shader(ShaderMode mode, ShaderType type) const;

	/// @brief Invalidates renderer-specific cache entries.
	/// @param[in] rendererID Renderer identifier to invalidate (default all).
	void invalidateCache(std::uintptr_t rendererID = std::numeric_limits<std::uintptr_t>::max());

	/// @brief Retrieves a typed cache object for the given renderer.
	/// @tparam T Cache subclass type.
	/// @param[in] rendererID Renderer identifier key.
	/// @return Pointer to cache object or nullptr.
	template <typename T>
	T* getMaterialCacheT(std::uintptr_t rendererID) const {
		auto it = m_materialCache.find(rendererID);
		if(it == m_materialCache.end() || it->second.isNull())
			return nullptr;

		return qobject_cast<T*>(it->second);
	}

	/// @brief Retrieves or creates a cache object for the given renderer.
	/// @tparam T Cache subclass type.
	/// @param[in] rendererID Renderer identifier key.
	/// @return Pair of cache pointer and creation flag.
	template <typename T>
	std::pair<T*, bool> getOrEmplaceMaterialCache(std::uintptr_t rendererID) {
		auto it = m_materialCache.find(rendererID);
		if(it == m_materialCache.end() || it->second.isNull()) {
			T* c                        = new T(this);
			m_materialCache[rendererID] = QPointer<MaterialCache>(c);
			return std::make_pair(c, true);
		}

		T* c = qobject_cast<T*>(it->second);
		if(!c)
			throw std::runtime_error("Possibly conflicting rendererID for Material.");

		return std::make_pair(c, false);
	}

private:
	RenderOptions m_renderOptions;                                     ///< Material rendering flags
	std::map<ShaderMode, std::map<ShaderType, QString>> m_shaders;     ///< Shader source mappings per mode and stage
	std::map<std::uintptr_t, QPointer<MaterialCache>> m_materialCache; ///< Renderer-specific material caches
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Material::RenderOptions)

}

#endif // A3DMATERIAL_H
