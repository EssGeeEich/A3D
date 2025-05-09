#include "resourcemanager.h"
#include "mesh.h"
#include "material.h"
#include "texture.h"
#include "model.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>

namespace A3D {

ResourceManager::ResourceManager(QObject* parent)
	: QObject{ parent } {}

Model* ResourceManager::getLoadedModel(QString const& name) const {
	auto it = m_models.find(name);
	if(it == m_models.end() || !it->second)
		return nullptr;
	return it->second;
}

Mesh* ResourceManager::getLoadedMesh(QString const& name) const {
	auto it = m_meshes.find(name);
	if(it == m_meshes.end() || !it->second)
		return nullptr;
	return it->second;
}

Material* ResourceManager::getLoadedMaterial(QString const& name) const {
	auto it = m_materials.find(name);
	if(it == m_materials.end() || !it->second)
		return nullptr;
	return it->second;
}

Texture* ResourceManager::getLoadedTexture(QString const& name) const {
	auto it = m_textures.find(name);
	if(it == m_textures.end() || !it->second)
		return nullptr;
	return it->second;
}

Cubemap* ResourceManager::getLoadedCubemap(QString const& name) const {
	auto it = m_cubemaps.find(name);
	if(it == m_cubemaps.end() || !it->second)
		return nullptr;
	return it->second;
}

Model* ResourceManager::registerModel(QString name, Model* model) {
	QPointer<Model>& m = m_models[std::move(name)];
	if(m)
		delete m;
	return m = model;
}

Mesh* ResourceManager::registerMesh(QString name, Mesh* resource) {
	QPointer<Mesh>& m = m_meshes[std::move(name)];
	if(m)
		delete m;
	return m = resource;
}

Material* ResourceManager::registerMaterial(QString name, Material* resource) {
	QPointer<Material>& m = m_materials[std::move(name)];
	if(m)
		delete m;
	return m = resource;
}

Texture* ResourceManager::registerTexture(QString name, Texture* resource) {
	QPointer<Texture>& t = m_textures[std::move(name)];
	if(t)
		delete t;
	return t = resource;
}

Cubemap* ResourceManager::registerCubemap(QString name, Cubemap* resource) {
	QPointer<Cubemap>& t = m_cubemaps[std::move(name)];
	if(t)
		delete t;
	return t = resource;
}

QStringList ResourceManager::registeredModels() const {
	QStringList sl;
	sl.reserve(m_models.size());
	for(auto it = m_models.begin(); it != m_models.end(); ++it) {
		sl << it->first;
	}
	return sl;
}

QStringList ResourceManager::registeredMeshes() const {
	QStringList sl;
	sl.reserve(m_meshes.size());
	for(auto it = m_meshes.begin(); it != m_meshes.end(); ++it) {
		sl << it->first;
	}
	return sl;
}

QStringList ResourceManager::registeredMaterials() const {
	QStringList sl;
	sl.reserve(m_materials.size());
	for(auto it = m_materials.begin(); it != m_materials.end(); ++it) {
		sl << it->first;
	}
	return sl;
}

QStringList ResourceManager::registeredTextures() const {
	QStringList sl;
	sl.reserve(m_textures.size());
	for(auto it = m_textures.begin(); it != m_textures.end(); ++it) {
		sl << it->first;
	}
	return sl;
}

QStringList ResourceManager::registeredCubemaps() const {
	QStringList sl;
	sl.reserve(m_cubemaps.size());
	for(auto it = m_cubemaps.begin(); it != m_cubemaps.end(); ++it) {
		sl << it->first;
	}
	return sl;
}

QString ResourceManager::locateFile(OpenFileResult const& parent, QString const& relativePath) const {
	if(relativePath.isEmpty())
		return QString();

	QString rv = parent.basePath + QDir::separator() + relativePath;
	return QFileInfo::exists(rv) ? rv : QString();
}

ResourceManager::OpenFileResult ResourceManager::openFile(QString name, QString const& path) const {
	OpenFileResult r;
	r.name = std::move(name);

	QFile* f = new QFile(path);
	if(!f->open(QFile::ReadOnly)) {
		delete f;
		return r;
	}

	QFileInfo fi(*f);
	r.stream.reset(f);
	r.uri      = fi.absoluteFilePath();
	r.basePath = fi.absolutePath();

	return r;
}

ResourceManager::OpenFileResult ResourceManager::openFile(OpenFileResult const& parent, QString const& relativePath) const {
	OpenFileResult r;
	r.name = locateFile(parent, relativePath);
	if(r.name.isEmpty())
		return r;

	QFile* f = new QFile(parent.basePath + QDir::separator() + relativePath);
	if(!f->open(QFile::ReadOnly)) {
		delete f;
		return r;
	}

	QFileInfo fi(*f);
	r.stream.reset(f);
	r.uri      = fi.absoluteFilePath();
	r.basePath = fi.absolutePath();

	return r;
}

Model* ResourceManager::loadModel(QString name, QString const& path, InputFormat fmt) {
	OpenFileResult ofr = openFile(name, path);
	if(!ofr.stream)
		return nullptr;

	if(fmt == IF_AutoDetect) {
		int lastDot = path.lastIndexOf('.');
		if(lastDot < 0)
			return nullptr;
		QString extension = path.mid(lastDot + 1).toLower();

		if(extension == "obj")
			fmt = IF_OBJ;
	}

	switch(fmt) {
	case IF_OBJ:
		return loadModel_OBJ(std::move(ofr));
	default:
		break;
	}

	return nullptr;
}

}
