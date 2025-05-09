/// @file
/// @brief Declares the ResourceManager class for managing the loading, registration, and retrieval of application resources (models, meshes, materials, textures, cubemaps).

#ifndef A3DRESOURCEMANAGER_H
#define A3DRESOURCEMANAGER_H

#include "common.h"
#include <QObject>

namespace A3D {

class Texture;
class Mesh;
class Material;
class Model;
class Cubemap;

/// @brief Manages Resource lifetimes and lookups.
/// @details Maintains internal caches to avoid duplicate resource loads and resolves relative file paths for asset loading.
class ResourceManager : public QObject {
	Q_OBJECT
public:
	/// @brief Supported input formats for loading models.
	enum InputFormat {
		IF_AutoDetect, ///< Automatically detect file format based on extension or content
		IF_OBJ         ///< Explicitly treat file as Wavefront OBJ format
	};

	/// @brief Constructs a ResourceManager.
	/// @param[in] parent Optional QObject parent.
	explicit ResourceManager(QObject* parent = nullptr);

	/// @brief Retrieves a previously loaded Model by name.
	/// @param[in] name Unique identifier of the model.
	/// @return Pointer to the Model, or nullptr if not loaded.
	Model* getLoadedModel(QString const& name) const;

	/// @brief Retrieves a previously loaded Mesh by name.
	/// @param[in] name Unique identifier of the mesh.
	/// @return Pointer to the Mesh, or nullptr if not loaded.
	Mesh* getLoadedMesh(QString const& name) const;

	/// @brief Retrieves a previously loaded Material by name.
	/// @param[in] name Unique identifier of the material.
	/// @return Pointer to the Material, or nullptr if not loaded.
	Material* getLoadedMaterial(QString const& name) const;

	/// @brief Retrieves a previously loaded Texture by name.
	/// @param[in] name Unique identifier of the texture.
	/// @return Pointer to the Texture, or nullptr if not loaded.
	Texture* getLoadedTexture(QString const& name) const;

	/// @brief Retrieves a previously loaded Cubemap by name.
	/// @param[in] name Unique identifier of the cubemap.
	/// @return Pointer to the Cubemap, or nullptr if not loaded.
	Cubemap* getLoadedCubemap(QString const& name) const;

	/// @brief Registers a Model under a given name.
	/// @param[in] name Unique identifier for future retrieval.
	/// @param[in] model Pointer to the Model resource.
	/// @return The registered Model pointer.
	Model* registerModel(QString name, Model* model);

	/// @brief Registers a Mesh under a given name.
	/// @param[in] name Unique identifier for future retrieval.
	/// @param[in] resource Pointer to the Mesh resource.
	/// @return The registered Mesh pointer.
	Mesh* registerMesh(QString name, Mesh* resource);

	/// @brief Registers a Material under a given name.
	/// @param[in] name Unique identifier for future retrieval.
	/// @param[in] resource Pointer to the Material resource.
	/// @return The registered Material pointer.
	Material* registerMaterial(QString name, Material* resource);

	/// @brief Registers a Texture under a given name.
	/// @param[in] name Unique identifier for future retrieval.
	/// @param[in] resource Pointer to the Texture resource.
	/// @return The registered Texture pointer.
	Texture* registerTexture(QString name, Texture* resource);

	/// @brief Registers a Cubemap under a given name.
	/// @param[in] name Unique identifier for future retrieval.
	/// @param[in] resource Pointer to the Cubemap resource.
	/// @return The registered Cubemap pointer.
	Cubemap* registerCubemap(QString name, Cubemap* resource);

	/// @brief Lists names of all registered Models.
	/// @return List of registered model names.
	QStringList registeredModels() const;

	/// @brief Lists names of all registered Meshes.
	/// @return List of registered mesh names.
	QStringList registeredMeshes() const;

	/// @brief Lists names of all registered Materials.
	/// @return List of registered material names.
	QStringList registeredMaterials() const;

	/// @brief Lists names of all registered Textures.
	/// @return List of registered texture names.
	QStringList registeredTextures() const;

	/// @brief Lists names of all registered Cubemaps.
	/// @return List of registered cubemap names.
	QStringList registeredCubemaps() const;

	/// @brief Loads a Model from file, registers, and returns it.
	/// @param[in] name Unique identifier for the loaded model.
	/// @param[in] path Filesystem path or URI to the model file.
	/// @param[in] fmt Input format.
	/// @return Loaded Model pointer, or nullptr on error.
	Model* loadModel(QString name, QString const& path, InputFormat fmt = IF_AutoDetect);

private:
	/// @brief Internal result containing file stream and path info.
	struct OpenFileResult {
		std::unique_ptr<QIODevice> stream; ///< Opened file stream
		QString name;                      ///< Resource name
		QString uri;                       ///< Resolved file URI
		QString basePath;                  ///< Base path for resolving relatives
	};

	/// @brief Opens a file given a name and path.
	/// @param[in] name Logical name for the resource.
	/// @param[in] path URI or filesystem path.
	/// @return OpenFileResult with stream and path info.
	OpenFileResult openFile(QString name, QString const& path) const;

	/// @brief Opens a file relative to a parent.
	/// @param[in] parent OpenFileResult of parent resource.
	/// @param[in] relativePath Relative path to open.
	/// @return OpenFileResult of the child resource.
	OpenFileResult openFile(OpenFileResult const& parent, QString const& relativePath) const;

	/// @brief Resolves a relative path against a parent resource.
	/// @param[in] parent OpenFileResult of parent resource.
	/// @param[in] relativePath Relative path to resolve.
	/// @return Absolute URI to the file.
	QString locateFile(OpenFileResult const& parent, QString const& relativePath) const;

	/// @brief Loads an OBJ model from an OpenFileResult.
	/// @param[in] fileResult OpenFileResult containing stream.
	/// @return Loaded Model pointer.
	Model* loadModel_OBJ(OpenFileResult fileResult);

	std::map<QString, QPointer<Model>> m_models;       ///< Cached models by name
	std::map<QString, QPointer<Mesh>> m_meshes;        ///< Cached meshes by name
	std::map<QString, QPointer<Material>> m_materials; ///< Cached materials by name
	std::map<QString, QPointer<Texture>> m_textures;   ///< Cached textures by name
	std::map<QString, QPointer<Cubemap>> m_cubemaps;   ///< Cached cubemaps by name
};

}

#endif // A3DRESOURCEMANAGER_H
