/// @file
/// @brief Base class for all resources managed by A3D's ResourceManager.

#ifndef A3DRESOURCE_H
#define A3DRESOURCE_H

#include "common.h"
#include <QObject>
#include <QPointer>
#include "resourcemanager.h"

namespace A3D {

/// @brief Abstract base for objects requiring lifetime management via ResourceManager.
class Resource : public QObject {
	Q_OBJECT
public:
	/// @brief Constructs a Resource and registers it with the manager.
	/// @param[in] manager Pointer to the ResourceManager that will own this resource.
	explicit Resource(ResourceManager* manager);

	/// @brief Retrieves the associated ResourceManager.
	/// @return Pointer to the ResourceManager managing this resource.
	ResourceManager* resourceManager() const;

private:
	QPointer<ResourceManager> m_resourceManager; ///< Weak pointer to the associated resource manager
};

}

#endif // A3DRESOURCE_H
