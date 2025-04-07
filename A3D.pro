include(A3D.pri)

CONFIG += c++17 resources_big
SOURCES += main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += A3D/SampleResources.qrc

HEADERS += \
    Utils.hpp
