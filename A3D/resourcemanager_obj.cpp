#include "resourcemanager.h"
#include "model.h"

#include <QIODevice>

namespace A3D {

Model* ResourceManager::loadModel_OBJ(OpenFileResult ofr) {
	if(!ofr.stream)
		return nullptr;

	struct FaceInfo {
		std::uint16_t smoothingGroup;
		std::int32_t indices_v[3];
		std::int32_t indices_vt[3];
		std::int32_t indices_vn[3];
	};

	struct GroupInfo {
		std::uint16_t lastSmoothingGroup;
		Mesh::Contents contents;
		QString material;
		std::vector<FaceInfo> faces;
	};

	std::map<QString, GroupInfo> groups;

	std::vector<QVector3D> v;
	std::vector<QVector2D> vt;
	std::vector<QVector3D> vn;

	QStringList materialFiles;

	// Load .obj
	{
		v.reserve(128);
		vt.reserve(128);
		vn.reserve(128);
		auto currentGroup = groups.end();

		QIODevice& device = *ofr.stream;
		QTextStream fin(&device);
		QString line;
		QString token;

		while(fin.readLineInto(&line)) {
			line = line.trimmed();

			QTextStream lineStream(&line);
			lineStream >> token;

			if(token.isEmpty() || token.startsWith('#'))
				continue;
			else if(token == "mtllib") {
				QString materialFile;
				lineStream.skipWhiteSpace();
				lineStream.readLineInto(&materialFile);
				materialFiles.append(std::move(materialFile));
				continue;
			}
			else if(token == "v") {
				float x, y, z;
				lineStream >> x >> y >> z;
				v.emplace_back(x, y, z);
				continue;
			}
			else if(token == "vt") {
				float x, y;
				lineStream >> x >> y;
				vt.emplace_back(x, y);
				continue;
			}
			else if(token == "vn") {
				float x, y, z;
				lineStream >> x >> y >> z;
				vn.emplace_back(x, y, z);
				continue;
			}
			else if(token == "g") {
				QString currentGroupName;
				lineStream.skipWhiteSpace();
				lineStream.readLineInto(&currentGroupName);
				auto iter    = groups.insert_or_assign(currentGroupName, GroupInfo{ 0, Mesh::Position3D | Mesh::TextureCoord2D | Mesh::Normal3D | Mesh::SmoothingGroup });
				currentGroup = iter.first;
				continue;
			}

			auto requireGroup = [&]() {
				if(currentGroup == groups.end()) {
					auto iter    = groups.insert_or_assign("[default]", GroupInfo{ 0, Mesh::Position3D | Mesh::TextureCoord2D | Mesh::Normal3D | Mesh::SmoothingGroup });
					currentGroup = iter.first;
				}
			};

			if(token == "usemtl") {
				requireGroup();
				lineStream.skipWhiteSpace();
				lineStream.readLineInto(&currentGroup->second.material);
				continue;
			}
			else if(token == "s") {
				requireGroup();
				lineStream >> currentGroup->second.lastSmoothingGroup;
				continue;
			}
			else if(token == "f") {
				requireGroup();
				QString vertexDefinition;
				std::int32_t indices[4][3] = {};

				std::size_t verticesRead = 0;
				for(std::size_t currentVertex = 0; currentVertex < 4; ++currentVertex, ++verticesRead) {
					lineStream >> vertexDefinition;

					QStringList vertexDefinitions = vertexDefinition.split(QChar('/'));

					bool ok            = true;
					int lastAttribRead = -1;

					for(int attribIndex = 0; attribIndex < vertexDefinitions.size() && attribIndex < 3; ++attribIndex) {
						indices[verticesRead][attribIndex] = vertexDefinitions[attribIndex].toFloat(&ok);

						if(ok) {
							lastAttribRead = attribIndex;
						}
						else {
							indices[verticesRead][attribIndex] = 0.f;
							// Could not read Position attribute; Break the loop.
							if(attribIndex == 0)
								break;

							if(attribIndex == 1)
								currentGroup->second.contents = currentGroup->second.contents & ~Mesh::TextureCoord2D;

							if(attribIndex == 2)
								currentGroup->second.contents = currentGroup->second.contents & ~Mesh::Normal3D;
						}
					}

					// If we couldn't read the position of the last vertex,
					// do not count this vertex as "read".
					if(!ok && lastAttribRead <= 0)
						break;
				}

				if(verticesRead == 3) {
					std::vector<FaceInfo>& vector = currentGroup->second.faces;
					vector.emplace_back();
					FaceInfo& fi = vector.back();

					fi.smoothingGroup = currentGroup->second.lastSmoothingGroup;
					fi.indices_v[0]   = indices[0][0];
					fi.indices_v[1]   = indices[1][0];
					fi.indices_v[2]   = indices[2][0];

					if(currentGroup->second.contents & Mesh::TextureCoord2D) {
						fi.indices_vt[0] = indices[0][1];
						fi.indices_vt[1] = indices[1][1];
						fi.indices_vt[2] = indices[2][1];
					}

					if(currentGroup->second.contents & Mesh::Normal3D) {
						fi.indices_vn[0] = indices[0][2];
						fi.indices_vn[1] = indices[1][2];
						fi.indices_vn[2] = indices[2][2];
					}
				}
				else if(verticesRead == 4) {
					std::vector<FaceInfo>& vector = currentGroup->second.faces;

					// Quad: Triangulate.
					vector.emplace_back();
					{
						FaceInfo& fi = vector.back();

						fi.smoothingGroup = currentGroup->second.lastSmoothingGroup;
						fi.indices_v[0]   = indices[0][0];
						fi.indices_v[1]   = indices[1][0];
						fi.indices_v[2]   = indices[2][0];

						if(currentGroup->second.contents & Mesh::TextureCoord2D) {
							fi.indices_vt[0] = indices[0][1];
							fi.indices_vt[1] = indices[1][1];
							fi.indices_vt[2] = indices[2][1];
						}

						if(currentGroup->second.contents & Mesh::Normal3D) {
							fi.indices_vn[0] = indices[0][2];
							fi.indices_vn[1] = indices[1][2];
							fi.indices_vn[2] = indices[2][2];
						}
					}

					vector.emplace_back();
					{
						FaceInfo& fi = vector.back();

						fi.smoothingGroup = currentGroup->second.lastSmoothingGroup;
						fi.indices_v[0]   = indices[0][0];
						fi.indices_v[1]   = indices[2][0];
						fi.indices_v[2]   = indices[3][0];

						if(currentGroup->second.contents & Mesh::TextureCoord2D) {
							fi.indices_vt[0] = indices[0][1];
							fi.indices_vt[1] = indices[2][1];
							fi.indices_vt[2] = indices[3][1];
						}

						if(currentGroup->second.contents & Mesh::Normal3D) {
							fi.indices_vn[0] = indices[0][2];
							fi.indices_vn[1] = indices[2][2];
							fi.indices_vn[2] = indices[3][2];
						}
					}
				}
			}
		}
	}

	struct MaterialInformations {
		float opacity;
		float specularExponent;

		QVector3D diffuse;
		QVector3D ambient;
		QVector3D specular;
		QVector3D emissive;

		QString diffuseMap;
		QString ambientMap;
		QString specularMap;
		QString emissiveMap;
		QString bumpMap;
	};

	std::map<QString, MaterialInformations> materials;

	// Load .mtl
	{
		auto currentMaterial = materials.end();

		for(auto it = materialFiles.begin(); it != materialFiles.end(); ++it) {
			OpenFileResult mtl = openFile(ofr, *it);
			if(!mtl.stream) {
				log(LC_Debug, u"MTL file couldn't be opened: " + *it);
				continue;
			}

			QIODevice& device = *mtl.stream;
			QTextStream fin(&device);
			QString line;
			QString token;

			while(fin.readLineInto(&line)) {
				line = line.trimmed();

				QTextStream lineStream(&line);
				lineStream >> token;

				if(token.isEmpty() || token.startsWith('#'))
					continue;
				else if(token == "newmtl") {
					QString currentMaterialName;
					lineStream.skipWhiteSpace();
					lineStream.readLineInto(&currentMaterialName);
					auto iter       = materials.insert_or_assign(currentMaterialName, MaterialInformations{ 1.f, 1.f });
					currentMaterial = iter.first;
				}

				if(currentMaterial == materials.end())
					continue;

				if(token == "Kd") {
					float r, g, b;
					lineStream >> r >> g >> b;
					currentMaterial->second.diffuse = QVector3D(r, g, b);
				}
				else if(token == "Ka") {
					float r, g, b;
					lineStream >> r >> g >> b;
					currentMaterial->second.ambient = QVector3D(r, g, b);
				}
				else if(token == "Ks") {
					float r, g, b;
					lineStream >> r >> g >> b;
					currentMaterial->second.specular = QVector3D(r, g, b);
				}
				else if(token == "Ke") {
					float r, g, b;
					lineStream >> r >> g >> b;
					currentMaterial->second.emissive = QVector3D(r, g, b);
				}
				else if(token == "Ns") {
					lineStream >> currentMaterial->second.specularExponent;
				}
				else if(token == "d") {
					lineStream >> currentMaterial->second.opacity;
				}
				else if(token == "map_Kd") {
					QString location;
					lineStream >> location;
					currentMaterial->second.diffuseMap = locateFile(mtl, location);
				}
				else if(token == "map_Ka") {
					QString location;
					lineStream >> location;
					currentMaterial->second.ambientMap = locateFile(mtl, location);
				}
				else if(token == "map_Ks") {
					QString location;
					lineStream >> location;
					currentMaterial->second.specularMap = locateFile(mtl, location);
				}
				else if(token == "map_Ke") {
					QString location;
					lineStream >> location;
					currentMaterial->second.emissiveMap = locateFile(mtl, location);
				}
			}
		}
	}

	std::unique_ptr<Model> newModel = std::make_unique<Model>(this);

	auto getVertex3D = [&](std::vector<QVector3D> const& in, std::int32_t ix) -> QVector3D {
		std::int32_t realIndex = ix;
		if(realIndex < 0)
			realIndex = static_cast<std::int32_t>(in.size()) - realIndex;
		else
			realIndex -= 1;

		if(realIndex < 0 || realIndex >= static_cast<std::int32_t>(in.size()))
			return QVector3D();
		return in[realIndex];
	};

	auto getVertex2D = [&](std::vector<QVector2D> const& in, std::int32_t ix) -> QVector2D {
		std::int32_t realIndex = ix;
		if(realIndex < 0)
			realIndex = static_cast<std::int32_t>(in.size()) - realIndex;
		else
			realIndex -= 1;

		if(realIndex < 0 || realIndex >= static_cast<std::int32_t>(in.size()))
			return QVector2D();
		return in[realIndex];
	};

	for(auto it = groups.begin(); it != groups.end(); ++it) {
		Group* newGroup     = newModel->addGroup(it->first);
		GroupInfo const& gi = it->second;

		newGroup->setProperty("obj_Material", gi.material);

		MaterialProperties* matProp = new MaterialProperties(this);
		newGroup->setMaterialProperties(matProp);
		newGroup->setMaterial(Material::standardMaterial(Material::PBRMaterial));

		auto foundMaterial = materials.find(gi.material);
		if(foundMaterial != materials.end()) {

			MaterialInformations const& mat = foundMaterial->second;

			if(!mat.diffuseMap.isEmpty()) {
				Texture* t = getLoadedTexture(mat.diffuseMap);
				if(!t) {
					Image i(QImage(mat.diffuseMap));
					if(!i.isNull())
						t = registerTexture(mat.diffuseMap, new Texture(i, this));
				}

				if(t)
					matProp->setTexture(t, MaterialProperties::AlbedoTextureSlot);
			}

			if(!mat.ambientMap.isEmpty()) {
				Texture* t = getLoadedTexture(mat.ambientMap);
				if(!t) {
					Image i(QImage(mat.ambientMap));
					if(!i.isNull())
						t = registerTexture(mat.ambientMap, new Texture(i, this));
				}

				if(t)
					matProp->setTexture(t, MaterialProperties::AlbedoTextureSlot);
			}

			if(!mat.specularMap.isEmpty()) {
				Texture* t = getLoadedTexture(mat.specularMap);
				if(!t) {
					Image i(QImage(mat.specularMap));
					if(!i.isNull())
						t = registerTexture(mat.specularMap, new Texture(i, this));
				}

				if(t)
					matProp->setTexture(t, MaterialProperties::MetallicTextureSlot);
			}

			/*
			if(!mat.emissiveMap.isEmpty()) {
				Texture* t = getLoadedTexture(mat.emissiveMap);
				if(!t) {
					Image i(QImage(mat.emissiveMap));
					if(!i.isNull())
						t = registerTexture(mat.emissiveMap, new Texture(i, this));
				}

				if(t)
					matProp->setTexture(t, MaterialProperties::MetallicTextureSlot);
			}
			*/

			if(!mat.bumpMap.isEmpty()) {
				Texture* t = getLoadedTexture(mat.emissiveMap);
				if(!t) {
					Image i(QImage(mat.emissiveMap));
					if(!i.isNull())
						t = registerTexture(mat.emissiveMap, new Texture(i, this));
				}

				if(t)
					matProp->setTexture(t, MaterialProperties::NormalTextureSlot);
			}
		}

		Mesh* mesh = new Mesh(this);
		mesh->setContents(gi.contents);
		mesh->setDrawMode(Mesh::IndexedTriangles);

		std::vector<Mesh::Vertex>& vertices = mesh->vertices();
		std::vector<std::uint32_t>& indices = mesh->indices();

		// Reserve memory for worst-case scenario
		vertices.reserve(gi.faces.size() * 3);
		indices.reserve(gi.faces.size() * 3);

		for(auto faceIter = gi.faces.begin(); faceIter != gi.faces.end(); ++faceIter) {
			FaceInfo const& face = *faceIter;

			for(std::size_t vertexIndex = 0; vertexIndex < 3; ++vertexIndex) {
				indices.emplace_back(static_cast<std::uint32_t>(vertices.size()));
				vertices.emplace_back();
				Mesh::Vertex& vertex = vertices.back();

				if(gi.contents & Mesh::Position3D)
					vertex.Position3D = getVertex3D(v, face.indices_v[vertexIndex]);
				if(gi.contents & Mesh::Normal3D)
					vertex.Normal3D = getVertex3D(vn, face.indices_vn[vertexIndex]);
				if(gi.contents & Mesh::TextureCoord2D)
					vertex.TextureCoord2D = getVertex2D(vt, face.indices_vt[vertexIndex]);
				vertex.SmoothingGroup = face.smoothingGroup;
			}
		}

		// We will rebuild triangle indices in-engine.
		mesh->optimizeIndices();

		this->registerMesh(ofr.uri + "/" + it->first, mesh);
		newGroup->setMesh(mesh);
	}

	this->registerModel(ofr.name, newModel.get());
	return newModel.release();
}

}
