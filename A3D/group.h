/// @file
/// @brief Defines the Group class, component of a Model that groups together a mesh, material, and line group.
/// @details Groups form scene graph nodes with a Model, supporting rendering options, hierarchical transforms, and intersection tests.

#ifndef A3DGROUP_H
#define A3DGROUP_H

#include "common.h"
#include "mesh.h"
#include "material.h"
#include "materialproperties.h"
#include "texture.h"
#include "linegroup.h"

namespace A3D {

class Model;

/// @brief Scene graph node managing transform, geometry, and appearance within a Model.
class Group : public QObject {
	Q_OBJECT
public:
	/// @brief Flags controlling Group rendering behavior.
	enum RenderOption {
		NoOptions = 0x0, ///< No special rendering options.
		Hidden    = 0x1  ///< Hide this Group and its descendants.
	};
	Q_DECLARE_FLAGS(RenderOptions, RenderOption)

	/// @brief Constructs a Group attached to a Model.
	/// @param[in] model The Model that owns this Group.
	explicit Group(Model* model);

	/// @brief Destructor; detaches from Model.
	~Group();

	/// @brief Clones this Group.
	/// @param[in] model The Model to attach the clone to.
	/// @param[in] deepClone If true, also clone associated Mesh and Material.
	/// @return New Group instance identical to this one.
	Group* clone(Model* model, bool deepClone) const;

	/// @brief Retrieves current render options.
	/// @return Bitmask of RenderOptions.
	RenderOptions renderOptions() const;

	/// @brief Sets render options.
	/// @param[in] options Bitmask of RenderOptions to apply.
	void setRenderOptions(RenderOptions options);

	/// @brief Retrieves the owning Model.
	/// @return Pointer to the parent Model.
	Model* model() const;

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

	/// @brief Retrieves the group's transform matrix.
	/// @return 4x4 transformation matrix.
	QMatrix4x4 const& groupMatrix() const;

	/// @brief Gets the associated LineGroup.
	/// @return Pointer to LineGroup, or nullptr if not set.
	LineGroup* lineGroup() const;

	/// @brief Gets the associated Mesh.
	/// @return Pointer to Mesh, or nullptr if not set.
	Mesh* mesh() const;

	/// @brief Gets the associated Material.
	/// @return Pointer to Material, or nullptr if not set.
	Material* material() const;

	/// @brief Gets the associated MaterialProperties.
	/// @return Pointer to MaterialProperties, or nullptr if not set.
	MaterialProperties* materialProperties() const;

	/// @brief Sets the LineGroup.
	/// @param[in] lineGroup Pointer to the LineGroup.
	void setLineGroup(LineGroup* lineGroup);

	/// @brief Sets the Mesh.
	/// @param[in] mesh Pointer to the Mesh.
	void setMesh(Mesh* mesh);

	/// @brief Sets the Material.
	/// @param[in] material Pointer to the Material.
	void setMaterial(Material* material);

	/// @brief Sets the MaterialProperties.
	/// @param[in] materialProperties Pointer to the MaterialProperties.
	void setMaterialProperties(MaterialProperties* materialProperties);

	/// @brief Performs ray intersection test.
	/// @param[in] origin World-space ray origin.
	/// @param[in] rayDirection World-space ray direction.
	/// @return IntersectionResult if hit; std::nullopt otherwise.
	std::optional<IntersectionResult> intersect(QVector3D origin, QVector3D rayDirection) const;

private:
	RenderOptions m_renderOptions; ///< Rendering flags for this Group.

	QPointer<Model> m_model;     ///< Parent Model.
	mutable bool m_matrixDirty;  ///< True if transform matrix needs update.
	mutable QMatrix4x4 m_matrix; ///< Cached group transform matrix.

	QVector3D m_position;   ///< Local position.
	QQuaternion m_rotation; ///< Local rotation.
	QVector3D m_scale;      ///< Local scale.

	QPointer<LineGroup> m_lineGroup;                   ///< Associated LineGroup for lines.
	QPointer<Mesh> m_mesh;                             ///< Associated Mesh geometry.
	QPointer<Material> m_material;                     ///< Associated Material for shading.
	QPointer<MaterialProperties> m_materialProperties; ///< Additional material settings.
};

}

#endif // A3DGROUP_H
