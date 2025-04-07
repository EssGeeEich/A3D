#ifndef UTILS_HPP
#define UTILS_HPP
#include <cstring>

/// @brief Contains all the parameters for a lerp function call
struct LerpDataset {
	float InputA;  ///< Left-side input of the lerp
	float InputB;  ///< Right-side input of the lerp
	float OutputA; ///< Left-side output of the lerp
	float OutputB; ///< Right-side output of the lerp
};

template <typename T, size_t N>
inline size_t count_of(T const (&)[N]) {
	return N;
}

// https://en.wikipedia.org/wiki/Linear_interpolation
float lerp(bool capped, float Xin, float X0, float X1, float Y0, float Y1);

// https://en.wikipedia.org/wiki/Linear_interpolation
template <typename Dataset>
inline float lerp(bool capped, float Xin, Dataset const& dataset) {
	return lerp(capped, Xin, static_cast<float>(dataset.InputA), static_cast<float>(dataset.InputB), static_cast<float>(dataset.OutputA), static_cast<float>(dataset.OutputB));
}

#include "A3D/mesh.h"
#include "A3D/resourcemanager.h"

void normalize(std::vector<float>& data) {
	float min = *std::min_element(data.begin(), data.end());
	float max = *std::max_element(data.begin(), data.end());

	for(size_t i = 0; i < data.size(); ++i)
		data[i] = lerp(true, data[i], min, max, 0.f, 1.f);
}

A3D::Mesh* surfaceMesh(A3D::ResourceManager* rm, std::vector<float> horizontalAxis, std::vector<float> verticalAxis, std::vector<float> data) {

	A3D::Mesh* mesh = new A3D::Mesh(rm);

	if(horizontalAxis.size() * verticalAxis.size() != data.size())
		return nullptr;

	normalize(horizontalAxis);
	normalize(verticalAxis);
	normalize(data);

	for(size_t y = 0; y < verticalAxis.size() - 1; ++y) {
		size_t horizontalSize = horizontalAxis.size();

		for(size_t x = 0; x < horizontalSize - 1; ++x) {
			float a = data[x + (y * horizontalSize)];
			float b = data[x + 1 + (y * horizontalSize)];
			float c = data[x + ((y + 1) * horizontalSize)];
			float d = data[x + 1 + ((y + 1) * horizontalSize)];
			float e = (a + b + c + d) / 4.f;
			A3D::Mesh::Vertex vertexA;
			vertexA.Position3D     = QVector3D(horizontalAxis[x], a, verticalAxis[y]);
			vertexA.TextureCoord2D = QVector2D(horizontalAxis[x], verticalAxis[y]);
			vertexA.Normal3D       = QVector3D(0, 1, 0);
			A3D::Mesh::Vertex vertexB;
			vertexB.Position3D     = QVector3D(horizontalAxis[x + 1], b, verticalAxis[y]);
			vertexB.TextureCoord2D = QVector2D(horizontalAxis[x + 1], verticalAxis[y]);
			vertexB.Normal3D       = QVector3D(0, 1, 0);
			A3D::Mesh::Vertex vertexC;
			vertexC.Position3D     = QVector3D(horizontalAxis[x], c, verticalAxis[y + 1]);
			vertexC.TextureCoord2D = QVector2D(horizontalAxis[x], verticalAxis[y + 1]);
			vertexC.Normal3D       = QVector3D(0, 1, 0);
			A3D::Mesh::Vertex vertexD;
			vertexD.Position3D     = QVector3D(horizontalAxis[x + 1], d, verticalAxis[y + 1]);
			vertexD.TextureCoord2D = QVector2D(horizontalAxis[x + 1], verticalAxis[y + 1]);
			vertexD.Normal3D       = QVector3D(0, 1, 0);
			A3D::Mesh::Vertex vertexE;
			vertexE.Position3D     = QVector3D((horizontalAxis[x] + horizontalAxis[x + 1]) / 2, e, (verticalAxis[y] + verticalAxis[y + 1]) / 2);
			vertexE.TextureCoord2D = QVector2D((horizontalAxis[x] + horizontalAxis[x + 1]) / 2, (verticalAxis[y] + verticalAxis[y + 1]) / 2);
			vertexE.Normal3D       = QVector3D(0, 1, 0);

			mesh->vertices().push_back(vertexA);
			mesh->vertices().push_back(vertexE);
			mesh->vertices().push_back(vertexB);

			mesh->vertices().push_back(vertexA);
			mesh->vertices().push_back(vertexC);
			mesh->vertices().push_back(vertexE);

			mesh->vertices().push_back(vertexC);
			mesh->vertices().push_back(vertexD);
			mesh->vertices().push_back(vertexE);

			mesh->vertices().push_back(vertexD);
			mesh->vertices().push_back(vertexB);
			mesh->vertices().push_back(vertexE);
		}
	}

	mesh->setContents(A3D::Mesh::Position3D | A3D::Mesh::TextureCoord2D | A3D::Mesh::Normal3D);

	for(size_t i = 0; i < mesh->vertices().size(); i++) {
		mesh->indices().push_back(i);
	}
	mesh->optimizeIndices();
	mesh->invalidateCache();
	return mesh;
}

#endif
