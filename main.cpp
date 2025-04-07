#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QDir>
#include <QTimer>
#include "A3D/view.h"
#include "A3D/keyboardcameracontroller.h"
#include "Utils.hpp"
#include <iostream>
#include <QPainter>
#include <QImage>
#include "A3D/textbillboardmodel.h"

#define WIDTH  8
#define HEIGHT 8

int main(int argc, char* argv[]) {

	QApplication a(argc, argv);
	QMainWindow w;

	A3D::Scene* s          = new A3D::Scene(&w);
	A3D::PointLightInfo& l = s->getOrCreateLight(0);
	l.position             = QVector3D(0.f, 2.2f, 5.f);
	l.color                = QVector4D(1.f, 1.f, 1.f, 500.f);

	A3D::MaterialProperties* Concrete002  = nullptr;
	A3D::MaterialProperties* Metal035     = nullptr;
	A3D::MaterialProperties* FloorTiles06 = nullptr;
	A3D::Cubemap* Cubemap001              = nullptr;

	{
		auto loadPBRMaterial = [s](QString path, QString baseName, QString fileExtension) -> A3D::MaterialProperties* {
			A3D::MaterialProperties* matProperties                                  = new A3D::MaterialProperties(s->resourceManager());
			static std::map<A3D::MaterialProperties::TextureSlot, QString> suffixes = {
				{    A3D::MaterialProperties::AlbedoTextureSlot,     "Color" },
				{    A3D::MaterialProperties::NormalTextureSlot,  "NormalGL" },
				{  A3D::MaterialProperties::MetallicTextureSlot,  "Metallic" },
				{ A3D::MaterialProperties::RoughnessTextureSlot, "Roughness" },
				{        A3D::MaterialProperties::AOTextureSlot,        "AO" },
			};

			for(std::pair<A3D::MaterialProperties::TextureSlot, QString> const& suffix: qAsConst(suffixes)) {
				A3D::Image img(QImage(path + QDir::separator() + baseName + "_" + suffix.second + "." + fileExtension));
				if(!img.isNull()) {
					A3D::Texture* texture = new A3D::Texture(std::move(img), s->resourceManager());
					s->resourceManager()->registerTexture(baseName + "_" + suffix.second, texture);

					matProperties->setTexture(texture, suffix.first);
				}
				else if(suffix.second == "AO") {
					A3D::Texture* texture = A3D::Texture::standardTexture(A3D::Texture::WhiteTexture);
					matProperties->setTexture(texture, suffix.first);
				}
			}
			return matProperties;
		};

		auto loadCubemap = [s](QString path, QString fileExtension) -> A3D::Cubemap* {
			A3D::Cubemap* cubemap = new A3D::Cubemap(s->resourceManager());

			cubemap->setNX(A3D::Image::HDR(path + "/nx." + fileExtension));
			cubemap->setNY(A3D::Image::HDR(path + "/ny." + fileExtension));
			cubemap->setNZ(A3D::Image::HDR(path + "/nz." + fileExtension));
			cubemap->setPX(A3D::Image::HDR(path + "/px." + fileExtension));
			cubemap->setPY(A3D::Image::HDR(path + "/py." + fileExtension));
			cubemap->setPZ(A3D::Image::HDR(path + "/pz." + fileExtension));

			if(!cubemap->isValid()) {
				delete cubemap;
				return nullptr;
			}
			return cubemap;
		};

		Cubemap001 = loadCubemap(":/A3D/SampleResources/Materials/Cubemap001", "hdr");
		if(Cubemap001) {
			Cubemap001->setParent(s->resourceManager());
			s->setSkybox(Cubemap001);
		}

		Concrete002 = loadPBRMaterial(":/A3D/SampleResources/Materials/Concrete002", "Concrete002_4K", "jpg");
		Concrete002->setParent(s->resourceManager()); // Just to get the clang static analyzer to piss off...

		Metal035 = loadPBRMaterial(":/A3D/SampleResources/Materials/Metal035", "Metal035_2K-JPG", "jpg");
		Metal035->setParent(s->resourceManager()); // Just to get the clang static analyzer to piss off...

		FloorTiles06 = loadPBRMaterial(":/A3D/SampleResources/Materials/FloorTiles06", "floor_tiles_06", "png");
		FloorTiles06->setParent(s->resourceManager()); // Just to get the clang static analyzer to piss off...
	}

	{
		A3D::Mesh* restTimeMesh = surfaceMesh(
			s->resourceManager(), { 100, 200, 300, 400, 500, 600, 800, 1200 }, { 1000, 2000, 3000, 3500, 4000, 5000, 6000, 7000 },
			{ 290, 290, 290, 270, 300, 400, 500, 500, 290, 290, 290, 250, 250, 400, 500, 500, 250, 250, 220, 220, 200, 300, 400, 400, 220, 200, 180, 180, 200, 250, 300, 100,
			  220, 200, 180, 160, 100, 100, 100, 100, 180, 160, 160, 120, 50,  50,  50,  40,  170, 150, 150, 100, 50,  40,  40,  40,  150, 120, 120, 100, 50,  40,  40,  40 }
		);

		s->resourceManager()->registerMesh("restTimeMeshSurface", restTimeMesh);

		A3D::Model* model = new A3D::Model();
		s->resourceManager()->registerModel("restTimeMeshGraphs", model);

		A3D::Group* g = model->getOrAddGroup("Default");
		g->setMesh(restTimeMesh);
		g->setMaterial(A3D::Material::standardMaterial(A3D::Material::PBRMaterial));
		g->setMaterialProperties(FloorTiles06);

		A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
		e->setModel(model);
	}

	{
		A3D::Mesh* autoUpMesh = surfaceMesh(
			s->resourceManager(), { 0, 10, 15, 20, 30, 40, 50, 75, 100 }, { 0, 1, 2, 3, 4, 5 },

			{ 1800, 2000, 2600, 3000, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100,
			  1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100, 1800, 2300, 2900, 3300, 3500, 4300, 4600, 6000, 6100 }
		);
		s->resourceManager()->registerMesh("autoUpMeshSurface", autoUpMesh);

		A3D::Model* model = new A3D::Model();
		s->resourceManager()->registerModel("autoUpMeshGraphs", model);

		A3D::Group* g = model->getOrAddGroup("Default");
		g->setMesh(autoUpMesh);
		g->setMaterial(A3D::Material::standardMaterial(A3D::Material::PBRMaterial));
		g->setMaterialProperties(FloorTiles06);

		A3D::Entity* e = s->emplaceChildEntity<A3D::Entity>();
		e->setModel(model);
		model->setPosition(QVector3D(2, 1, 1));
	}
	/*A3D::Model* model = new A3D::Model(nullptr);

		A3D::Group* gText = model->getOrAddGroup("Text");
		gText->setMesh(A3D::Mesh::standardMesh(A3D::Mesh::ScreenQuadMesh));

		gText->setMaterial(A3D::Material::standardMaterial(A3D::Material::BillboardMaterial));
		A3D::Entity* eText = s->emplaceChildEntity<A3D::Entity>();
		eText->setModel(model);

		QImage textImage(400, 400, QImage::Format_RGBA8888);
		textImage.fill(Qt::transparent);
		{
			QPainter painter(&textImage);

			QFont font("Arial", 20);
			painter.setFont(font);
			painter.setPen(QColor(0, 0, 0));
			painter.drawText(50, 100, "Ciao, mondo!");
			painter.end();
		}

		A3D::MaterialProperties* materialProperties = new A3D::MaterialProperties(s->resourceManager());

		A3D::Image image(std::move(textImage));
		A3D::Texture* textTexture = new A3D::Texture(std::move(image), s->resourceManager());
		materialProperties->setTexture(textTexture, A3D::MaterialProperties::AlbedoTextureSlot);

		gText->setMaterialProperties(materialProperties);
	}
*/
	A3D::View* v = new A3D::View(&w);
	v->camera().setPosition(QVector3D(10.f, 0.f, 2.f));
	v->camera().setOrientationTarget(QVector3D(0.f, 0.f, 0.f));
	v->setScene(s);
	{
		A3D::TextBillboard* text = new A3D::TextBillboard(s->resourceManager());

		QFont f("Arial", 20, 20);
		QColor c = QColor(Qt::red);
		text->setText("Ciao mondo!", f, c);
		A3D::Entity* entity = s->emplaceChildEntity<A3D::Entity>();
		entity->setModel(text);
		int* n    = new int;
		*n        = 0;
		QTimer* t = new QTimer();
		t->start(100);
		QObject::connect(t, &QTimer::timeout, t, [=]() {
			(*n)++;
			text->setText(QString("Ciao mondo! %1").arg(*n), f, c);
			v->update();
		});
	}

	A3D::KeyboardCameraController* keyCamController = new A3D::KeyboardCameraController(v);
	keyCamController->setBaseMovementSpeed(QVector3D(9.f, 9.f, 9.f));
	v->setController(keyCamController);

	QTimer t;
	if(v->format().swapInterval() > 0)
		t.setInterval(0);
	else
		t.setInterval(10);
	t.start();

	QObject::connect(&t, &QTimer::timeout, v, &A3D::View::updateView);
	QObject::connect(&t, &QTimer::timeout, s, &A3D::Scene::updateScene);

	v->run();
	s->run();

	w.setCentralWidget(v);
	w.show();
	int rv = a.exec();
	return rv;
}

float lerp(bool capped, float Xin, float X0, float X1, float Y0, float Y1) {
	if(capped) {
		if(X0 > X1) {
			std::swap(X0, X1);
			std::swap(Y0, Y1);
		}

		if(Xin <= X0)
			return Y0;
		else if(Xin >= X1)
			return Y1;
	}

	return Y0 + (Y1 - Y0) * ((Xin - X0) / (X1 - X0));
}
