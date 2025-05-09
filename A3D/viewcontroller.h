/// @file
/// @brief Declaration of the A3D::ViewController class for handling a View.

#ifndef A3DVIEWCONTROLLER_H
#define A3DVIEWCONTROLLER_H

#include "common.h"
#include <QObject>

namespace A3D {

class View;

/// @brief Abstract base class for controlling A3D::View instances.
///
/// A ViewController encapsulates logic to manipulate or update a View
/// over time. Concrete subclasses implement the @c update() method to
/// perform per-frame or per-timestep updates.
class ViewController : public QObject {
	Q_OBJECT

public:
	/// @brief Constructs a ViewController for the given view.
	/// @param[in] view Pointer to the A3D::View to be controlled.
	explicit ViewController(View* view);

	/// @brief Advances the controller's state by a time delta.
	/// @param[in] deltaT Time elapsed since last update, in milliseconds.
	/// @return true if the view or underlying state changed; false otherwise.
	virtual bool update(std::chrono::milliseconds deltaT) = 0;

protected:
	/// @brief Retrieves the controlled view.
	/// @return Pointer to the A3D::View instance, or null if it has been deleted.
	View* view() const;

private:
	QPointer<View> m_view; ///< Weak pointer to the controlled view.
};

}

#endif // A3DVIEWCONTROLLER_H
