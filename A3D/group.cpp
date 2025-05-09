#include "group.h"
#include "model.h"

namespace A3D {

Group::Group(Model* model)
	: QObject{ model },
	  m_renderOptions(NoOptions),
	  m_model(model),
	  m_matrixDirty(true),
	  m_scale(1.f, 1.f, 1.f) {
	log(LC_Debug, u"Constructor: Group");
}

Group::~Group() {
	log(LC_Debug, u"Destructor: Group");
}

Group* Group::clone(Model* m, bool deepClone) const {
	Group* newGroup         = new Group(m);
	newGroup->m_matrixDirty = m_matrixDirty;
	if(!m_matrixDirty)
		newGroup->m_matrix = m_matrix;

	newGroup->m_position = m_position;
	newGroup->m_rotation = m_rotation;
	newGroup->m_scale    = m_scale;

	if(deepClone) {
		// ...
		if(m_mesh)
			newGroup->m_mesh = m_mesh->clone();
		if(m_material)
			newGroup->m_material = m_material->clone();
		if(m_materialProperties)
			newGroup->m_materialProperties = m_materialProperties->clone();
		if(m_lineGroup)
			newGroup->m_lineGroup = m_lineGroup->clone();
	}
	else {
		newGroup->m_mesh               = m_mesh;
		newGroup->m_material           = m_material;
		newGroup->m_materialProperties = m_materialProperties;
		newGroup->m_lineGroup          = m_lineGroup;
	}

	return newGroup;
}

Group::RenderOptions Group::renderOptions() const {
	return m_renderOptions;
}
void Group::setRenderOptions(RenderOptions renderOptions) {
	m_renderOptions = renderOptions;
}

Model* Group::model() const {
	return m_model;
}

void Group::setPosition(QVector3D const& pos) {
	if(m_position == pos)
		return;
	m_position    = pos;
	m_matrixDirty = true;
}
QVector3D Group::position() const {
	return m_position;
}

void Group::setRotation(QQuaternion const& rot) {
	if(m_rotation == rot)
		return;
	m_rotation    = rot;
	m_matrixDirty = true;
}
QQuaternion Group::rotation() const {
	return m_rotation;
}

void Group::setScale(QVector3D const& scale) {
	if(m_scale == scale)
		return;
	m_scale       = scale;
	m_matrixDirty = true;
}
QVector3D Group::scale() const {
	return m_scale;
}

QMatrix4x4 const& Group::groupMatrix() const {
	if(m_matrixDirty) {
		m_matrixDirty = false;
		m_matrix.setToIdentity();
		m_matrix.rotate(m_rotation);
		m_matrix.scale(m_scale);
		m_matrix.translate(m_position);
	}
	return m_matrix;
}

Mesh* Group::mesh() const {
	return m_mesh;
}
Material* Group::material() const {
	return m_material;
}
MaterialProperties* Group::materialProperties() const {
	return m_materialProperties;
}
LineGroup* Group::lineGroup() const {
	return m_lineGroup;
}

void Group::setMesh(Mesh* mesh) {
	if(mesh == m_mesh)
		return;
	if(m_mesh && m_mesh->parent() == this)
		delete m_mesh;
	m_mesh = mesh;
}

void Group::setMaterial(Material* material) {
	if(material == m_material)
		return;
	if(m_material && m_material->parent() == this)
		delete m_material;
	m_material = material;
}

void Group::setMaterialProperties(MaterialProperties* materialProperties) {
	if(materialProperties == m_materialProperties)
		return;
	if(m_materialProperties && m_materialProperties->parent() == this)
		delete m_materialProperties;
	m_materialProperties = materialProperties;
}

void Group::setLineGroup(LineGroup* lineGroup) {
	if(lineGroup == m_lineGroup)
		return;
	if(m_lineGroup && m_lineGroup->parent() == this)
		delete m_lineGroup;
	m_lineGroup = lineGroup;
}

std::optional<IntersectionResult> Group::intersect(QVector3D origin, QVector3D dir) const {
	if(!mesh())
		return std::nullopt;

	std::optional<QVector3D> result = mesh()->intersect(origin, dir);

	if(!result)
		return std::nullopt;

	IntersectionResult res;
	res.m_resultingEntity    = nullptr;
	res.m_resultingModel     = nullptr;
	res.m_resultingGroup     = const_cast<Group*>(this);
	res.m_groupLocalHitPoint = *result;
	res.m_hitPoint           = *result;
	return res;
}

}