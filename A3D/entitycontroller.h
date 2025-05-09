/// @file
/// @brief Declares the EntityController interface used to control A3D entities.

#ifndef A3DENTITYCONTROLLER_H
#define A3DENTITYCONTROLLER_H

#include "common.h"
#include <QObject>

namespace A3D {

class Entity;

/// @brief Abstract base class representing a controller that updates an Entity.
class EntityController : public QObject {
	Q_OBJECT
public:
	/// @brief Constructs an EntityController.
	/// @param[in] entity Pointer to the Entity this controller affects.
	explicit EntityController(Entity* entity);

	/// @brief Updates the entity.
	/// @param[in] deltaT Time elapsed since the last update.
	/// @return true if the entity was modified.
	virtual bool update(std::chrono::milliseconds deltaT) = 0;

protected:
	/// @brief Retrieves the entity associated with this controller.
	/// @return Pointer to the controlled Entity.
	Entity* entity() const;

private:
	QPointer<Entity> m_entity; ///< Pointer to the controlled Entity.
};

}

#endif // A3DENTITYCONTROLLER_H
