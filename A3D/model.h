/// @file
/// @brief Declares the Model class representing a transformable 3D model composed of named groups.

#ifndef A3DMODEL_H
#define A3DMODEL_H

#include "common.h"
#include <QObject>
#include "group.h"

namespace A3D {

/// @brief Represents a 3D model with transform and grouped components.
/// @details The Model class manages transformation properties (position, rotation, scale),
/// and groups of renderable entities identified by string names. It supports optional
/// visibility flags and deep cloning.
class Model : public QObject {
	Q_OBJECT
public:
	/// @brief Flags controlling model rendering behavior.
	enum RenderOption {
		NoOptions = 0x0, ///< No special rendering options.
		Hidden    = 0x1, ///< Hides this Model and all its descendants.
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	/// @brief Constructs an empty model with default transformation.
	/// @param[in] parent Optional parent QObject.
	explicit Model(QObject* parent = nullptr);

	/// @brief Destructor.
	~Model();

	/// @brief Creates a copy of the model.
	/// @param[in] deepClone If true, deeply clones groups' contents.
	/// @return Pointer to the cloned Model.
	Model* clone(bool deepClone) const;

	/// @brief Retrieves current render options.
	/// @return Bitmask of RenderOptions.
	RenderOptions renderOptions() const;

	/// @brief Sets render options.
	/// @param[in] options Bitmask of RenderOptions to apply.
	void setRenderOptions(RenderOptions);

	/// @brief Adds a new group to the model.
	/// @param[in] name Name identifier for the group.
	/// @remarks If a group with this name already exists, it will be deleted and
	/// a new one will be created in its place.
	/// @return Pointer to the new Group.
	Group* addGroup(QString name);

	/// @brief Gets a group by name or creates one if not present.
	/// @param[in] name Name of the group.
	/// @return Pointer to the existing or newly created Group.
	Group* getOrAddGroup(QString name);

	/// @brief Gets a group by name.
	/// @param[in] name Group name.
	/// @return Pointer to the Group, or nullptr if not found.
	Group* getGroup(QString const& name);

	/// @brief Retrieves all group names in this model.
	/// @return List of group name strings.
	QStringList groupNames() const;

	/// @brief Retrieves all groups in the model.
	/// @return Const reference to a map of group names to Group pointers.
	std::map<QString, QPointer<Group>> const& groups() const;

	/// @brief Sets local position.
	/// @param[in] pos 3D translation vector.
	void setPosition(QVector3D const& pos);

	/// @brief Gets local position.
	/// @return Position vector.
	QVector3D position() const;

	/// @brief Sets local rotation.
	/// @param[in] rot Orientation quaternion.
	void setRotation(QQuaternion const& rot);

	/// @brief Gets local rotation.
	/// @return Orientation quaternion.
	QQuaternion rotation() const;

	/// @brief Sets local scale.
	/// @param[in] scale Scale factors.
	void setScale(QVector3D const& scale = QVector3D(1.f, 1.f, 1.f));

	/// @brief Gets local scale.
	/// @return Scale factors.
	QVector3D scale() const;

	/// @brief Retrieves the model's transform matrix.
	/// @return 4x4 transformation matrix.
	QMatrix4x4 const& modelMatrix() const;

private:
	RenderOptions m_renderOptions; ///< Rendering flags for this Model.

	mutable bool m_matrixDirty;  ///< True if transform matrix needs update.
	mutable QMatrix4x4 m_matrix; ///< Cached model transform matrix.

	QVector3D m_position;   ///< World-space position
	QQuaternion m_rotation; ///< Orientation
	QVector3D m_scale;      ///< Scale vector

	std::map<QString, QPointer<Group>> m_groups; ///< Named groups in this model
};

}

#endif // A3DMODEL_H
