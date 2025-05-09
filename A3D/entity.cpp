#include "entity.h"
#include "model.h"

namespace A3D {

Entity::Entity(Entity* parent)
	: QObject{ parent },
	  m_parent(parent),
	  m_renderOptions(NoOptions),
	  m_matrixDirty(true),
	  m_scale(1.f, 1.f, 1.f) {
	log(LC_Debug, u"Constructor: Entity");
}

Entity::~Entity() {
	log(LC_Debug, u"Destructor: Entity");
}

Entity::RenderOptions Entity::renderOptions() const {
	return m_renderOptions;
}
void Entity::setRenderOptions(RenderOptions renderOptions) {
	m_renderOptions = renderOptions;
}

Entity* Entity::parentEntity() const {
	return m_parent;
}

std::vector<QPointer<Entity>> const& Entity::childrenEntities() const {
	return m_entities;
}

void Entity::addChildEntity(Entity* entity) {
	cleanupQPointers(m_entities);
	m_entities.emplace_back(entity);
}

void Entity::setModel(Model* model) {
	m_model = model;
}
Model* Entity::model() const {
	return m_model;
}

void Entity::setPosition(QVector3D const& pos) {
	if(m_position == pos)
		return;
	m_position    = pos;
	m_matrixDirty = true;
}
QVector3D Entity::position() const {
	return m_position;
}

void Entity::setRotation(QQuaternion const& rot) {
	if(m_rotation == rot)
		return;
	m_rotation    = rot;
	m_matrixDirty = true;
}
QQuaternion Entity::rotation() const {
	return m_rotation;
}

void Entity::setScale(QVector3D const& scale) {
	if(m_scale == scale)
		return;
	m_scale       = scale;
	m_matrixDirty = true;
}
QVector3D Entity::scale() const {
	return m_scale;
}

QMatrix4x4 const& Entity::entityMatrix() const {
	if(m_matrixDirty) {
		m_matrixDirty = false;
		m_matrix.setToIdentity();
		m_matrix.rotate(m_rotation);
		m_matrix.translate(m_position);
		m_matrix.scale(m_scale);
	}
	return m_matrix;
}

void Entity::addController(EntityController* ec) {
	removeController(ec);
	m_entityControllers.push_back(ec);
}

void Entity::removeController(EntityController* ec) {
	m_entityControllers.erase(std::remove_if(m_entityControllers.begin(), m_entityControllers.end(), [ec](QPointer<EntityController> it) -> bool {
		return (!it || it != ec);
	}));
}

bool Entity::updateEntity(std::chrono::milliseconds t) {
	bool hasChanges = false;
	for(auto it = m_entityControllers.begin(); it != m_entityControllers.end();) {
		if(!*it) {
			it = m_entityControllers.erase(it);
			continue;
		}

		hasChanges = (*it)->update(t) || hasChanges;
		++it;
	}

	for(auto it = m_entities.begin(); it != m_entities.end();) {
		if(!*it) {
			it = m_entities.erase(it);
			continue;
		}

		hasChanges = (*it)->updateEntity(t) || hasChanges;
		++it;
	}

	return hasChanges;
}

std::optional<IntersectionResult> Entity::intersect(QVector3D origin, QVector3D rayDirection) const {
	// dobbiamo trasformare origin e rayDirection in base a entityMatrix()
	QMatrix4x4 const worldToEntity = entityMatrix().inverted();
	origin                         = worldToEntity.map(origin);
	rayDirection                   = worldToEntity.mapVector(rayDirection).normalized();

	for(auto it = m_entities.begin(); it != m_entities.end(); ++it) {
		if(!*it)
			continue;

		auto result = (*it)->intersect(origin, rayDirection);
		if(result) {
			result->m_hitPoint = entityMatrix().map(result->m_hitPoint);
			return std::move(result);
		}
	}

	Model const* m = model();
	if(!m)
		return std::nullopt;

	// dobbiamo trasformare origin e rayDirection in base a m->modelMatrix()
	QMatrix4x4 const worldToModel = m->modelMatrix().inverted();
	origin                        = worldToModel.map(origin);
	rayDirection                  = worldToModel.mapVector(rayDirection).normalized();

	std::map<QString, QPointer<Group>> const& groups = m->groups();
	for(auto it = groups.begin(); it != groups.end(); ++it) {
		QPointer<Group> const& g = it->second;
		if(!g)
			continue;

		QMatrix4x4 const worldToGroup = g->groupMatrix().inverted();
		QVector3D groupOrigin         = worldToGroup.map(origin);
		QVector3D groupRayDirection   = worldToGroup.mapVector(rayDirection).normalized();

		auto result = g->intersect(groupOrigin, groupRayDirection);
		if(result) {
			result->m_resultingModel  = const_cast<Model*>(m);
			result->m_resultingEntity = const_cast<Entity*>(this);

			result->m_hitPoint = g->groupMatrix().map(result->m_hitPoint);
			result->m_hitPoint = m->modelMatrix().map(result->m_hitPoint);
			result->m_hitPoint = entityMatrix().map(result->m_hitPoint);
			return std::move(result);
		}
	}

	return std::nullopt;
}

}
