#include "entity.h"

namespace A3D {

Entity::Entity(Entity *parent)
	: QObject{parent},
	  m_parent(parent),
	  m_renderOptions(NoOptions),
	  m_scale(1.f, 1.f, 1.f),
	  m_matrixDirty(true),
	  m_mesh(nullptr),
	  m_material(nullptr)
{
	dbgConstruct("Entity")
}

Entity::~Entity() {
	dbgDestruct("Entity")
}

Entity::RenderOptions Entity::renderOptions() const {
	return m_renderOptions;
}
void Entity::setRenderOptions(RenderOptions renderOptions) {
	m_renderOptions = renderOptions;
}

Entity* Entity::parentEntity() const {
	return m_parent.get();
}

std::vector<QPointer<Entity>> const& Entity::childrenEntities() const {
	return m_entities;
}

void Entity::addChildEntity(Entity* entity) {
	cleanupQPointers(m_entities);
	m_entities.emplace_back(entity);
}

void Entity::setPosition(QVector3D const& pos) {
	if(m_position == pos)
	   return;
	m_position = pos;
	m_matrixDirty = true;
}
QVector3D Entity::position() const { return m_position; }

void Entity::setRotation(QQuaternion const& rot) {
	if(m_rotation == rot)
		return;
	m_rotation = rot;
	m_matrixDirty = true;
}
QQuaternion Entity::rotation() const { return m_rotation; }

void Entity::setScale(QVector3D const& scale) {
	if(m_scale == scale)
		return;
	m_scale = scale;
	m_matrixDirty = true;
}
QVector3D Entity::scale() const {
	return m_scale;
}

QMatrix4x4 const& Entity::modelMatrix() const {
	if(m_matrixDirty) {
		m_matrixDirty = false;
		m_matrix.setToIdentity();
		m_matrix.rotate(m_rotation);
		m_matrix.scale(m_scale);
		m_matrix.translate(m_position);
	}
	return m_matrix;
}

Mesh* Entity::mesh() const {
	return m_mesh;
}

Material* Entity::material() const {
	return m_material;
}
MaterialProperties& Entity::materialProperties() {
	return m_materialProperties;
}
MaterialProperties const& Entity::materialProperties() const {
	return m_materialProperties;
}

void Entity::setMesh(Mesh* mesh) {
	if(m_mesh && m_mesh->parent() == this)
		delete m_mesh;
	m_mesh = mesh;
}

void Entity::setMaterial(Material* material) {
	if(m_material && m_material->parent() == this)
		delete m_material;
	m_material = material;
}

void Entity::setMaterialProperties(MaterialProperties properties) {
	m_materialProperties = std::move(properties);
}

}