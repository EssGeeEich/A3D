/// @file entity.h
/// @brief Defines the Entity class, a scene graph node with transformations, models, and controllers

#ifndef A3DENTITY_H
#define A3DENTITY_H

#include "common.h"
#include <QObject>
#include <QPointer>
#include <QMatrix4x4>
#include <vector>
#include <optional>
#include "model.h"
#include "entitycontroller.h"

namespace A3D {

/// @brief Base class for scene graph nodes, supporting transforms, models, and controllers.
/// @details Entities form a hierarchy, can render models, and perform intersection queries.
class Entity : public QObject {
	Q_OBJECT

protected:
	/// @brief Constructs an Entity with an optional parent.
	/// @param[in] parent Pointer to the parent Entity; nullptr for root.
	explicit Entity(Entity* parent);

public:
	/// @brief Rendering flags to control visibility.
	enum RenderOption {
		NoOptions = 0x0, ///< No special render options.
		Hidden    = 0x1, ///< Entity and children are not rendered.
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	/// @brief Destructor; detaches from parent and cleans up controllers.
	~Entity();

	/// @brief Retrieves current render options flags.
	/// @return Bitmask of RenderOptions.
	RenderOptions renderOptions() const;

	/// @brief Sets render options flags.
	/// @param[in] options Bitmask of RenderOptions to apply.
	void setRenderOptions(RenderOptions options);

	/// @brief Gets the parent Entity in the hierarchy.
	/// @return Pointer to the parent Entity, or nullptr if root.
	Entity* parentEntity() const;

	/// @brief List of child Entities; some entries may be nullptr if removed.
	/// @return Const reference to vector of QPointer<Entity>.
	std::vector<QPointer<Entity>> const& childrenEntities() const;

	/// @brief Constructs and adds a new child Entity of type T.
	/// @tparam T Type of Entity to create.
	/// @param[in] args Constructor arguments for T (excluding parent).
	/// @return Pointer to the newly created child Entity.
	template <typename T, typename... Args>
	T* emplaceChildEntity(Args&&... args) {
		T* p = new T(std::forward<Args>(args)..., this);
		addChildEntity(p);
		return p;
	}

	/// @brief Assigns a Model to this Entity.
	/// @param[in] model Pointer to the Model to attach.
	void setModel(Model* model);

	/// @brief Retrieves the attached Model.
	/// @return Pointer to the Model, or nullptr if none set.
	Model* model() const;

	/// @brief Sets the world-space position of the Entity.
	/// @param[in] pos 3D translation vector.
	void setPosition(QVector3D const& pos);

	/// @brief Gets the world-space position of the Entity.
	/// @return Position vector.
	QVector3D position() const;

	/// @brief Sets the rotation of the Entity.
	/// @param[in] rot Quaternion representing orientation.
	void setRotation(QQuaternion const& rot);

	/// @brief Gets the current rotation of the Entity.
	/// @return Quaternion representing orientation.
	QQuaternion rotation() const;

	/// @brief Sets the scale of the Entity.
	/// @param[in] scale Scale factors; defaults to (1,1,1).
	void setScale(QVector3D const& scale = QVector3D(1.f, 1.f, 1.f));

	/// @brief Retrieves the scale of the Entity.
	/// @return Scale vector.
	QVector3D scale() const;

	/// @brief Computes or retrieves the transformation matrix.
	/// @return Const reference to the 4x4 world transform matrix.
	QMatrix4x4 const& entityMatrix() const;

	/// @brief Performs a ray intersection test against this Entity's model.
	/// @param[in] origin Ray origin in world space.
	/// @param[in] rayDirection Ray direction vector.
	/// @return Intersection result if hit; std::nullopt otherwise.
	std::optional<IntersectionResult> intersect(QVector3D origin, QVector3D rayDirection) const;

	/// @brief Adds an EntityController to this Entity.
	/// @param[in] controller Pointer to the controller to add.
	void addController(EntityController* controller);

	/// @brief Removes an EntityController from this Entity.
	/// @param[in] controller Pointer to the controller to remove.
	void removeController(EntityController* controller);

protected:
	/// @brief Updates Entity logic for a tick.
	/// @param[in] deltaMs Delta time in milliseconds.
	/// @return True if the Entity state changed and matrix must be updated.
	virtual bool updateEntity(std::chrono::milliseconds deltaMs);

private:
	/// @brief Internal helper to add a child pointer to the list.
	/// @param[in] child Pointer to the child Entity to register.
	void addChildEntity(Entity* child);

	QPointer<Entity> m_parent;                                   ///< Weak pointer to parent Entity.
	std::vector<QPointer<Entity>> m_entities;                    ///< Child Entities.
	std::vector<QPointer<EntityController>> m_entityControllers; ///< Attached controllers.
	RenderOptions m_renderOptions;                               ///< Visibility and render flags.

	mutable bool m_matrixDirty;  ///< True if transform matrix needs recompute.
	mutable QMatrix4x4 m_matrix; ///< Cached world transform matrix.

	QVector3D m_position;   ///< Local position.
	QQuaternion m_rotation; ///< Local orientation.
	QVector3D m_scale;      ///< Local scale.

	QPointer<Model> m_model; ///< Attached 3D model.
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Entity::RenderOptions)

}

#endif // A3DENTITY_H
