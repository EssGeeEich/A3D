QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += openglwidgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    $$PWD/A3D/entitycontroller.cpp \
    $$PWD/A3D/keyboardcameracontroller.cpp \
    $$PWD/A3D/scenecontroller.cpp \
    $$PWD/A3D/textbillboardmodel.cpp \
    $$PWD/A3D/viewcontroller.cpp \
    A3D/camera.cpp \
    A3D/common.cpp \
    A3D/cubemap.cpp \
    A3D/cubemapcache.cpp \
    A3D/cubemapcacheogl.cpp \
    A3D/entity.cpp \
    A3D/group.cpp \
    A3D/image.cpp \
    A3D/material.cpp \
    A3D/materialcache.cpp \
    A3D/materialcacheogl.cpp \
    A3D/materialproperties.cpp \
    A3D/materialpropertiescache.cpp \
    A3D/materialpropertiescacheogl.cpp \
    A3D/mesh.cpp \
    A3D/meshcache.cpp \
    A3D/meshcacheogl.cpp \
    A3D/model.cpp \
    A3D/renderer.cpp \
    A3D/rendererogl.cpp \
    A3D/resource.cpp \
    A3D/resourcemanager.cpp \
    A3D/resourcemanager_obj.cpp \
    A3D/scene.cpp \
    A3D/texture.cpp \
    A3D/texturecache.cpp \
    A3D/texturecacheogl.cpp \
    A3D/view.cpp

HEADERS += \
	$$PWD/A3D/entitycontroller.h \
	$$PWD/A3D/keyboardcameracontroller.h \
	$$PWD/A3D/scenecontroller.h \
	$$PWD/A3D/textbillboardmodel.h \
	$$PWD/A3D/viewcontroller.h \
	$$PWD/Dependencies/stb/stb_image_write.h \
	A3D/camera.h \
	A3D/common.h \
	A3D/cubemap.h \
	A3D/cubemapcache.h \
	A3D/cubemapcacheogl.h \
	A3D/entity.h \
	A3D/group.h \
	A3D/image.h \
	A3D/material.h \
	A3D/materialcache.h \
	A3D/materialcacheogl.h \
	A3D/materialproperties.h \
	A3D/materialpropertiescache.h \
	A3D/materialpropertiescacheogl.h \
	A3D/mesh.h \
	A3D/meshcache.h \
	A3D/meshcacheogl.h \
	A3D/model.h \
	A3D/renderer.h \
	A3D/rendererogl.h \
	A3D/resource.h \
	A3D/resourcemanager.h \
	A3D/scene.h \
	A3D/texture.h \
	A3D/texturecache.h \
	A3D/texturecacheogl.h \
	A3D/view.h \
	Dependencies/stb/stb_image.h

DEFINES += STBI_NO_STDIO=1 STBI_NO_ZLIB=1 STBI_ONLY_HDR=1
INCLUDEPATH += Dependencies/stb/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += A3D/A3D.qrc
