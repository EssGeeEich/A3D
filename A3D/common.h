#ifndef A3DCOMMON_H
#define A3DCOMMON_H

#include <QGenericMatrix>
#include <QElapsedTimer>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <QPointer>
#include <QtMath>
#include <QString>
#include <QStringView>
#include <QVariant>
#include <QObject>
#include <QImage>

#include <QOpenGLContext>
#include <QOpenGLFunctions_3_3_Core>

#include <memory>
#include <limits>
#include <map>
#include <vector>
#include <list>
#include <chrono>
#include <stdexcept>

namespace A3D {

enum LogChannel {
	LC_Debug = 0,
	LC_Info,
	LC_Warning,
	LC_Critical,
	LC_Fatal,
};

void log(LogChannel, QString);
void log(LogChannel, QStringView);

class NonCopyable {
	NonCopyable(NonCopyable const&)            = delete;
	NonCopyable& operator=(NonCopyable const&) = delete;

public:
	inline NonCopyable() {}
};

typedef QOpenGLFunctions_3_3_Core CoreGLFunctions;

template <typename T>
void cleanupQPointers(std::vector<QPointer<T>>& container) {
	container.erase(
		std::remove_if(
			container.begin(), container.end(),
			[](QPointer<T> const& e) -> bool {
				return e.isNull();
			}
		),
		container.end()
	);
}

template <typename K, typename T>
void cleanupQPointers(std::map<K, QPointer<T>>& container) {
	for(auto it = container.begin(); it != container.end();) {
		if(it->second.isNull())
			it = container.erase(it);
		else
			++it;
	}
}

inline QImage const* imageWithFormat(QImage::Format format, QImage const& base, QImage& storage) {
	if(base.format() == format)
		return &base;
	storage = base.convertToFormat(format);
	return &storage;
}

}

#endif // A3DCOMMON_H
