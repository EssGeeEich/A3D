#include "model.h"

namespace A3D {

Model::Model(QObject* parent)
	: QObject{ parent },
	  m_renderOptions(NoOptions),
	  m_matrixDirty(true),
	  m_scale(1.f, 1.f, 1.f) {
	log(LC_Debug, u"Constructor: Model");
}

Model::~Model() {
	log(LC_Debug, u"Destructor: Model");
}

Model* Model::clone(bool deepClone) const {
	Model* newModel         = new Model(parent());
	newModel->m_matrixDirty = m_matrixDirty;
	if(!m_matrixDirty)
		newModel->m_matrix = m_matrix;
	newModel->m_position = m_position;
	newModel->m_rotation = m_rotation;
	newModel->m_scale    = m_scale;
	for(auto it = m_groups.begin(); it != m_groups.end(); ++it) {
		if(it->second)
			newModel->m_groups[it->first] = it->second->clone(newModel, deepClone);
	}

	return newModel;
}

Model::RenderOptions Model::renderOptions() const {
	return m_renderOptions;
}
void Model::setRenderOptions(RenderOptions renderOptions) {
	m_renderOptions = renderOptions;
}

Group* Model::addGroup(QString name) {
	QPointer<Group>& g = m_groups[std::move(name)];
	if(g)
		delete g;
	g = new Group(this);
	return g;
}
Group* Model::getOrAddGroup(QString name) {
	QPointer<Group>& g = m_groups[std::move(name)];
	if(!g)
		g = new Group(this);
	return g;
}
Group* Model::getGroup(QString const& name) {
	auto it = m_groups.find(name);
	if(it == m_groups.end())
		return nullptr;
	return it->second;
}

QStringList Model::groupNames() const {
	QStringList sl;
	sl.reserve(m_groups.size());
	for(auto it = m_groups.begin(); it != m_groups.end(); ++it) {
		sl << it->first;
	}
	return sl;
}

std::map<QString, QPointer<Group>> const& Model::groups() const {
	return m_groups;
}

void Model::setPosition(QVector3D const& pos) {
	if(m_position == pos)
		return;
	m_position    = pos;
	m_matrixDirty = true;
}
QVector3D Model::position() const {
	return m_position;
}

void Model::setRotation(QQuaternion const& rot) {
	if(m_rotation == rot)
		return;
	m_rotation    = rot;
	m_matrixDirty = true;
}
QQuaternion Model::rotation() const {
	return m_rotation;
}

void Model::setScale(QVector3D const& scale) {
	if(m_scale == scale)
		return;
	m_scale       = scale;
	m_matrixDirty = true;
}
QVector3D Model::scale() const {
	return m_scale;
}

QMatrix4x4 const& Model::modelMatrix() const {
	if(m_matrixDirty) {
		m_matrixDirty = false;
		m_matrix.setToIdentity();
		m_matrix.rotate(m_rotation);
		m_matrix.scale(m_scale);
		m_matrix.translate(m_position);
	}
	return m_matrix;
}

}
