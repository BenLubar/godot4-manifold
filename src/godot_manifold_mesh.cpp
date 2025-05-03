#include "godot_manifold_defs.h"
#include "godot_manifold_converters.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/local_vector.hpp>

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/importer_mesh.hpp>
#include <godot_cpp/classes/surface_tool.hpp>

#include <manifold/manifold.h>

using namespace godot;

const static uint32_t NULL_MATERIAL_ORIGINAL_ID = manifold::Manifold::ReserveIDs(1);
static HashMap<ObjectID, uint32_t> material_original_id;

uint32_t ManifoldMesh::get_material_original_id(const Ref<Material> &p_material) {
	if (unlikely(p_material.is_null())) {
		return NULL_MATERIAL_ORIGINAL_ID;
	}

	const ObjectID object_id(p_material->get_instance_id());
	if (likely(material_original_id.has(object_id))) {
		return material_original_id.get(object_id);
	}

	// clean up any deleted materials
	for (auto it = material_original_id.begin(), next = it; it; it = next) {
		++next;
		if (unlikely(!ObjectDB::get_instance(it->key))) {
			material_original_id.remove(it);
		}
	}

	const uint32_t original_id = manifold::Manifold::ReserveIDs(1);
	material_original_id.insert(object_id, original_id);
	return original_id;
}

void ManifoldMesh::_bind_methods() {
	DEV_ASSERT(NULL_MATERIAL_ORIGINAL_ID == 1);

	ADD_GROUP("Surfaces", "");
	ClassDB::bind_method(D_METHOD("set_surface_formats", "surface_formats"), &ManifoldMesh::set_surface_formats);
	ClassDB::bind_method(D_METHOD("get_surface_formats"), &ManifoldMesh::get_surface_formats);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "surface_formats"), "set_surface_formats", "get_surface_formats");

	ClassDB::bind_method(D_METHOD("set_surface_original_ids", "surface_original_ids"), &ManifoldMesh::set_surface_original_ids);
	ClassDB::bind_method(D_METHOD("get_surface_original_ids"), &ManifoldMesh::get_surface_original_ids);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "surface_original_ids"), "set_surface_original_ids", "get_surface_original_ids");

	ClassDB::bind_method(D_METHOD("set_surface_materials", "surface_materials"), &ManifoldMesh::set_surface_materials);
	ClassDB::bind_method(D_METHOD("get_surface_materials"), &ManifoldMesh::get_surface_materials);
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "surface_materials", PROPERTY_HINT_ARRAY_TYPE, itos(Variant::OBJECT) + "/" + itos(PROPERTY_HINT_RESOURCE_TYPE) + ":Material"), "set_surface_materials", "get_surface_materials");

	ClassDB::bind_method(D_METHOD("set_surface_names", "surface_names"), &ManifoldMesh::set_surface_names);
	ClassDB::bind_method(D_METHOD("get_surface_names"), &ManifoldMesh::get_surface_names);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_STRING_ARRAY, "surface_names"), "set_surface_names", "get_surface_names");

	ADD_GROUP("", "");
	ClassDB::bind_method(D_METHOD("is_valid"), &ManifoldMesh::is_valid);
	ClassDB::bind_method(D_METHOD("is_empty"), &ManifoldMesh::is_empty);
	ClassDB::bind_method(D_METHOD("remove_unused_materials"), &ManifoldMesh::remove_unused_materials);

	constexpr Variant::Type PACKED_REAL_ARRAY = static_cast<Variant::Type>(GetTypeInfo<PackedRealArray>::VARIANT_TYPE);
	constexpr Variant::Type PACKED_I_ARRAY = static_cast<Variant::Type>(GetTypeInfo<PackedIArray>::VARIANT_TYPE);

	ADD_GROUP("Mesh", "");
#define BIND_PROPERTY(m_variant_type, m_name) \
	ClassDB::bind_method(D_METHOD("set_" #m_name, #m_name), &ManifoldMesh::set_##m_name); \
	ClassDB::bind_method(D_METHOD("get_" #m_name), &ManifoldMesh::get_##m_name); \
	ADD_PROPERTY(PropertyInfo(m_variant_type, #m_name), "set_" #m_name, "get_" #m_name)
	BIND_PROPERTY(Variant::INT, num_prop);
	BIND_PROPERTY(PACKED_REAL_ARRAY, vert_properties);
	BIND_PROPERTY(PACKED_I_ARRAY, tri_verts);
	BIND_PROPERTY(PACKED_I_ARRAY, merge_from_vert);
	BIND_PROPERTY(PACKED_I_ARRAY, merge_to_vert);
	BIND_PROPERTY(PACKED_I_ARRAY, run_index);
	BIND_PROPERTY(Variant::PACKED_INT32_ARRAY, run_original_id);
	BIND_PROPERTY(PACKED_REAL_ARRAY, run_transform);
	BIND_PROPERTY(PACKED_I_ARRAY, face_id);
	BIND_PROPERTY(PACKED_REAL_ARRAY, halfedge_tangent);
	BIND_PROPERTY(Variant::FLOAT, tolerance);
#undef BIND_PROPERTY

	ADD_GROUP("", "");
	ClassDB::bind_static_method(get_class_static(), D_METHOD("from_mesh", "mesh"), &ManifoldMesh::from_mesh);
	ClassDB::bind_method(D_METHOD("to_mesh", "generate_lods", "create_shadow_mesh", "skip_material"), &ManifoldMesh::to_mesh, DEFVAL(true), DEFVAL(true), DEFVAL(TypedArray<Material>()));

	ClassDB::bind_method(D_METHOD("decompose"), &ManifoldMesh::decompose);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("compose", "manifolds"), &ManifoldMesh::compose);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("tetrahedron", "material"), &ManifoldMesh::tetrahedron, DEFVAL(nullptr));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("cube", "size", "center", "material"), &ManifoldMesh::cube, DEFVAL(Vector3(1.0f, 1.0f, 1.0f)), DEFVAL(false), DEFVAL(nullptr));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("cylinder", "height", "radius_low", "radius_high", "circular_segments", "center", "material"), &ManifoldMesh::cylinder, DEFVAL(-1.0), DEFVAL(0), DEFVAL(false), DEFVAL(nullptr));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("sphere", "radius", "circular_segments", "material"), &ManifoldMesh::sphere, DEFVAL(0), DEFVAL(nullptr));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("level_set", "sdf", "bounds", "edge_length", "level", "tolerance", "material"), &ManifoldMesh::level_set, DEFVAL(0.0), DEFVAL(-1.0), DEFVAL(nullptr));

	ClassDB::bind_method(D_METHOD("slice", "height"), &ManifoldMesh::slice, DEFVAL(0.0));
	ClassDB::bind_method(D_METHOD("project"), &ManifoldMesh::project);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("extrude", "cross_section", "height", "divisions", "twist_degrees", "scale_top", "material"), &ManifoldMesh::extrude, DEFVAL(0), DEFVAL(0.0), DEFVAL(Vector2(1.0f, 1.0f)), DEFVAL(nullptr));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("revolve", "cross_section", "circular_segments", "revolve_degrees", "material"), &ManifoldMesh::revolve, DEFVAL(0), DEFVAL(360.0), DEFVAL(nullptr));

	ClassDB::bind_method(D_METHOD("get_vertex_count"), &ManifoldMesh::get_vertex_count);
	ClassDB::bind_method(D_METHOD("get_edge_count"), &ManifoldMesh::get_edge_count);
	ClassDB::bind_method(D_METHOD("get_triangle_count"), &ManifoldMesh::get_triangle_count);
	ClassDB::bind_method(D_METHOD("get_property_vertex_count"), &ManifoldMesh::get_property_vertex_count);
	ClassDB::bind_method(D_METHOD("get_aabb"), &ManifoldMesh::get_aabb);

	ClassDB::bind_method(D_METHOD("get_genus"), &ManifoldMesh::get_genus);
	ClassDB::bind_method(D_METHOD("get_surface_area"), &ManifoldMesh::get_surface_area);
	ClassDB::bind_method(D_METHOD("get_volume"), &ManifoldMesh::get_volume);
	ClassDB::bind_method(D_METHOD("get_min_gap", "other", "search_length"), &ManifoldMesh::get_min_gap);

	ClassDB::bind_method(D_METHOD("translate", "offset"), &ManifoldMesh::translate);
	ClassDB::bind_method(D_METHOD("scale", "scale"), &ManifoldMesh::scale);
	ClassDB::bind_method(D_METHOD("rotate", "rotation_degrees"), &ManifoldMesh::rotate);
	ClassDB::bind_method(D_METHOD("mirror", "normal"), &ManifoldMesh::mirror);
	ClassDB::bind_method(D_METHOD("transform", "transform"), &ManifoldMesh::transform);
	ClassDB::bind_method(D_METHOD("warp", "warp_vertex"), &ManifoldMesh::warp);
	ClassDB::bind_method(D_METHOD("warp_batch", "warp_vertices"), &ManifoldMesh::warp_batch);
	ClassDB::bind_method(D_METHOD("simplify", "tolerance"), &ManifoldMesh::simplify, DEFVAL(0.0));

	ClassDB::bind_method(D_METHOD("hull"), &ManifoldMesh::hull);
	ClassDB::bind_method(D_METHOD("refine", "subdivisions"), &ManifoldMesh::refine);
	ClassDB::bind_method(D_METHOD("refine_to_length", "length"), &ManifoldMesh::refine_to_length);
	ClassDB::bind_method(D_METHOD("refine_to_tolerance", "tolerance"), &ManifoldMesh::refine_to_tolerance);

	ClassDB::bind_method(D_METHOD("union", "with"), &ManifoldMesh::union_with);
	ClassDB::bind_method(D_METHOD("intersection", "with"), &ManifoldMesh::intersection_with);
	ClassDB::bind_method(D_METHOD("difference", "with"), &ManifoldMesh::difference_with);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("batch_union", "manifolds"), &ManifoldMesh::batch_union);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("batch_intersection", "manifolds"), &ManifoldMesh::batch_intersection);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("batch_difference", "manifolds"), &ManifoldMesh::batch_difference);

	ClassDB::bind_method(D_METHOD("split", "manifold"), &ManifoldMesh::split_bind);
	ClassDB::bind_method(D_METHOD("split_by_plane", "plane", "material"), &ManifoldMesh::split_by_plane_bind, DEFVAL(nullptr));
	ClassDB::bind_method(D_METHOD("trim_by_plane", "plane", "material"), &ManifoldMesh::trim_by_plane, DEFVAL(nullptr));

	ClassDB::bind_method(D_METHOD("modify_normal", "modify", "normalize"), &ManifoldMesh::modify_normal, DEFVAL(true));
	ClassDB::bind_method(D_METHOD("modify_tex_uv", "modify"), &ManifoldMesh::modify_tex_uv);
	ClassDB::bind_method(D_METHOD("modify_tex_uv2", "modify"), &ManifoldMesh::modify_tex_uv2);
	ClassDB::bind_method(D_METHOD("modify_color", "modify"), &ManifoldMesh::modify_color);
	ClassDB::bind_method(D_METHOD("modify_custom0", "modify"), &ManifoldMesh::modify_custom0);
	ClassDB::bind_method(D_METHOD("modify_custom1", "modify"), &ManifoldMesh::modify_custom1);
	ClassDB::bind_method(D_METHOD("modify_custom2", "modify"), &ManifoldMesh::modify_custom2);
	ClassDB::bind_method(D_METHOD("modify_custom3", "modify"), &ManifoldMesh::modify_custom3);
}

struct ManifoldMesh::Inner {
	manifold::Manifold _manifold;
	manifold::MeshGLP<Precision, I> _meshgl;
	bool _manifold_dirty = false;
	bool _meshgl_dirty = false;
	bool _has_bad_original_ids = true;

	Vector<Array> _arrays;
};

ManifoldMesh::ManifoldMesh() {
	_inner = memnew(Inner);
}

ManifoldMesh::~ManifoldMesh() {
	memdelete(_inner);
	_inner = nullptr;
}

void ManifoldMesh::set_surface_formats(const PackedInt32Array &p_surface_formats) {
	if (_surface_formats != p_surface_formats) {
		_surface_formats = p_surface_formats;
		emit_changed();
	}
}
PackedInt32Array ManifoldMesh::get_surface_formats() const {
	return _surface_formats;
}
void ManifoldMesh::set_surface_original_ids(const PackedInt32Array &p_surface_original_ids) {
	if (_surface_original_ids != p_surface_original_ids) {
		_surface_original_ids = p_surface_original_ids;
		emit_changed();
	}
}
PackedInt32Array ManifoldMesh::get_surface_original_ids() const {
	return _surface_original_ids;
}
void ManifoldMesh::set_surface_materials(const TypedArray<Material> &p_surface_materials) {
	if (_surface_materials != p_surface_materials) {
		_surface_materials = p_surface_materials;
		emit_changed();
	}
}
TypedArray<Material> ManifoldMesh::get_surface_materials() const {
	return _surface_materials;
}
void ManifoldMesh::set_surface_names(const PackedStringArray &p_surface_names) {
	if (_surface_names != p_surface_names) {
		_surface_names = p_surface_names;
		emit_changed();
	}
}
PackedStringArray ManifoldMesh::get_surface_names() const {
	return _surface_names;
}

bool ManifoldMesh::is_valid() const {
	_ensure_manifold();
	DEV_ASSERT(_inner->_manifold.Status() <= manifold::Manifold::Error::NotManifold);
	return _inner->_manifold.Status() == manifold::Manifold::Error::NoError;
}

bool ManifoldMesh::is_empty() const {
	_ensure_manifold();
	return _inner->_manifold.IsEmpty();
}

void ManifoldMesh::remove_unused_materials() {
	ERR_FAIL_COND(_surface_original_ids.size() != _surface_materials.size());
	ERR_FAIL_COND(_surface_original_ids.size() != _surface_formats.size());

	if (unlikely(_inner->_has_bad_original_ids)) {
		_reallocate_original_ids();
	}

	_ensure_meshgl();

	if (unlikely(_surface_names.size() < _surface_original_ids.size())) {
		_surface_names.resize(_surface_original_ids.size());
	}

	bool any_removed = false;
	for (int32_t i = 0; i < _surface_original_ids.size(); i++) {
		const uint32_t original_id = _surface_original_ids[i];
		if (std::find(_inner->_meshgl.runOriginalID.begin(), _inner->_meshgl.runOriginalID.end(), original_id) != _inner->_meshgl.runOriginalID.end()) {
			continue;
		}

		_surface_formats.remove_at(i);
		_surface_original_ids.remove_at(i);
		_surface_materials.remove_at(i);
		_surface_names.remove_at(i);
		i--;
		any_removed = true;
	}

	if (any_removed) {
		emit_changed();
	}
}

#define SET_VALUE(m_prop, m_param) \
	_ensure_meshgl(); \
	m_prop = m_param; \
	_inner->_manifold_dirty = true; \
	emit_changed()
#define SET_ARRAY(m_prop, m_param) \
	_ensure_meshgl(); \
	m_prop.resize(m_param.size()); \
	static_assert(sizeof(m_param[0]) == sizeof(m_prop[0])); \
	std::copy(m_param.ptr(), m_param.ptr() + m_param.size(), m_prop.begin()); \
	_inner->_manifold_dirty = true; \
	emit_changed()

#define GET_VALUE(m_prop) \
	_ensure_meshgl(); \
	return m_prop
#define GET_ARRAY(m_packed, m_prop) \
	_ensure_meshgl(); \
	m_packed array; \
	static_assert(sizeof(array[0]) == sizeof(m_prop[0])); \
	array.resize(m_prop.size()); \
	std::copy(m_prop.begin(), m_prop.end(), array.ptrw()); \
	return array

void ManifoldMesh::set_num_prop(I p_num_prop) {
	SET_VALUE(_inner->_meshgl.numProp, p_num_prop);
}
ManifoldMesh::I ManifoldMesh::get_num_prop() const {
	GET_VALUE(_inner->_meshgl.numProp);
}

void ManifoldMesh::set_vert_properties(const PackedRealArray &p_vert_properties) {
	SET_ARRAY(_inner->_meshgl.vertProperties, p_vert_properties);
}
PackedRealArray ManifoldMesh::get_vert_properties() const {
	GET_ARRAY(PackedRealArray, _inner->_meshgl.vertProperties);
}

void ManifoldMesh::set_tri_verts(const PackedIArray &p_tri_verts) {
	SET_ARRAY(_inner->_meshgl.triVerts, p_tri_verts);
}
ManifoldMesh::PackedIArray ManifoldMesh::get_tri_verts() const {
	GET_ARRAY(PackedIArray, _inner->_meshgl.triVerts);
}

void ManifoldMesh::set_merge_from_vert(const PackedIArray &p_merge_from_vert) {
	SET_ARRAY(_inner->_meshgl.mergeFromVert, p_merge_from_vert);
}
ManifoldMesh::PackedIArray ManifoldMesh::get_merge_from_vert() const {
	GET_ARRAY(PackedIArray, _inner->_meshgl.mergeFromVert);
}

void ManifoldMesh::set_merge_to_vert(const PackedIArray &p_merge_to_vert) {
	SET_ARRAY(_inner->_meshgl.mergeToVert, p_merge_to_vert);
}
ManifoldMesh::PackedIArray ManifoldMesh::get_merge_to_vert() const {
	GET_ARRAY(PackedIArray, _inner->_meshgl.mergeToVert);
}

void ManifoldMesh::set_run_index(const PackedIArray &p_run_index) {
	SET_ARRAY(_inner->_meshgl.runIndex, p_run_index);
}
ManifoldMesh::PackedIArray ManifoldMesh::get_run_index() const {
	GET_ARRAY(PackedIArray, _inner->_meshgl.runIndex);
}

void ManifoldMesh::set_run_original_id(const PackedInt32Array &p_run_original_id) {
	SET_ARRAY(_inner->_meshgl.runOriginalID, p_run_original_id);
}
PackedInt32Array ManifoldMesh::get_run_original_id() const {
	GET_ARRAY(PackedInt32Array, _inner->_meshgl.runOriginalID);
}

void ManifoldMesh::set_run_transform(const PackedRealArray &p_run_transform) {
	SET_ARRAY(_inner->_meshgl.runTransform, p_run_transform);
}
PackedRealArray ManifoldMesh::get_run_transform() const {
	GET_ARRAY(PackedRealArray, _inner->_meshgl.runTransform);
}

void ManifoldMesh::set_face_id(const PackedIArray &p_face_id) {
	SET_ARRAY(_inner->_meshgl.faceID, p_face_id);
}
ManifoldMesh::PackedIArray ManifoldMesh::get_face_id() const {
	GET_ARRAY(PackedIArray, _inner->_meshgl.faceID);
}

void ManifoldMesh::set_halfedge_tangent(const PackedRealArray &p_halfedge_tangent) {
	SET_ARRAY(_inner->_meshgl.halfedgeTangent, p_halfedge_tangent);
}
PackedRealArray ManifoldMesh::get_halfedge_tangent() const {
	GET_ARRAY(PackedRealArray, _inner->_meshgl.halfedgeTangent);
}

void ManifoldMesh::set_tolerance(Precision p_tolerance) {
	SET_VALUE(_inner->_meshgl.tolerance, p_tolerance);
}
ManifoldMesh::Precision ManifoldMesh::get_tolerance() const {
	GET_VALUE(_inner->_meshgl.tolerance);
}

#undef SET_VALUE
#undef SET_ARRAY
#undef GET_VALUE
#undef GET_ARRAY

int32_t ManifoldMesh::_get_surface_count() const {
	return _surface_materials.size();
}
int32_t ManifoldMesh::_surface_get_array_len(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, _surface_materials.size(), 0);

	_commit_to_arrays();

	const PackedVector3Array vertices = _inner->_arrays[p_index][ARRAY_VERTEX];
	return vertices.size();
}
int32_t ManifoldMesh::_surface_get_array_index_len(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, _surface_materials.size(), 0);

	_commit_to_arrays();

	const PackedInt32Array indices = _inner->_arrays[p_index][ARRAY_INDEX];
	return indices.size();
}
Array ManifoldMesh::_surface_get_arrays(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, _surface_materials.size(), Array());

	_commit_to_arrays();

	return _inner->_arrays[p_index];
}
TypedArray<Array> ManifoldMesh::_surface_get_blend_shape_arrays(int32_t p_index) const {
	return TypedArray<Array>();
}
Dictionary ManifoldMesh::_surface_get_lods(int32_t p_index) const {
	return Dictionary();
}
uint32_t ManifoldMesh::_surface_get_format(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, _surface_formats.size(), 0);

	return _surface_formats[p_index] & ~ARRAY_FORMAT_INDEX;
}
uint32_t ManifoldMesh::_surface_get_primitive_type(int32_t p_index) const {
	return PRIMITIVE_TRIANGLES;
}
void ManifoldMesh::_surface_set_material(int32_t p_index, const Ref<Material> &p_material) {
	ERR_FAIL_INDEX(p_index, _surface_materials.size());
	if (_surface_materials[p_index] != p_material) {
		_surface_materials[p_index] = p_material;
		emit_changed();
	}
}
Ref<Material> ManifoldMesh::_surface_get_material(int32_t p_index) const {
	ERR_FAIL_INDEX_V(p_index, _surface_materials.size(), Ref<Material>());
	return _surface_materials[p_index];
}
int32_t ManifoldMesh::_get_blend_shape_count() const {
	return 0;
}
StringName ManifoldMesh::_get_blend_shape_name(int32_t p_index) const {
	ERR_FAIL_V(StringName());
}
void ManifoldMesh::_set_blend_shape_name(int32_t p_index, const StringName &p_name) {
	ERR_FAIL();
}
AABB ManifoldMesh::_get_aabb() const {
	_ensure_manifold();
	return from_box(_inner->_manifold.BoundingBox());
}

static BitField<Mesh::ArrayFormat> get_surface_format_hack(const Array &p_arrays) {
	// Godot's Mesh::surface_get_format method isn't exposed to scripting, so:
	Ref<ArrayMesh> mesh;
	mesh.instantiate();
	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, p_arrays);
	return mesh->surface_get_format(0);
}
Ref<ManifoldMesh> ManifoldMesh::from_mesh(const Ref<Mesh> &p_mesh) {
	ERR_FAIL_COND_V(p_mesh.is_null(), Ref<ManifoldMesh>());

	const Ref<ArrayMesh> array_mesh = p_mesh;
	ERR_FAIL_COND_V(array_mesh.is_valid() && array_mesh->get_blend_shape_count() != 0, Ref<ManifoldMesh>());

	Ref<ManifoldMesh> mesh;
	mesh.instantiate();

	const int32_t num_surfaces = p_mesh->get_surface_count();

	mesh->_surface_formats.resize(num_surfaces);
	mesh->_surface_original_ids.resize(num_surfaces);
	mesh->_surface_materials.resize(num_surfaces);
	mesh->_surface_names.resize(num_surfaces);

	uint32_t format_union = 0;
	I total_vertices = 0, total_indices = 0;
	for (int32_t surface = 0; surface < num_surfaces; surface++) {
		const uint32_t format = array_mesh.is_valid() ? array_mesh->surface_get_format(surface) : get_surface_format_hack(p_mesh->surface_get_arrays(surface));

		ERR_FAIL_COND_V(!(format & ARRAY_FORMAT_VERTEX), Ref<ManifoldMesh>());
		ERR_FAIL_COND_V(format & ARRAY_FORMAT_BONES, Ref<ManifoldMesh>());
		ERR_FAIL_COND_V(format & ARRAY_FORMAT_WEIGHTS, Ref<ManifoldMesh>());
		ERR_FAIL_COND_V((format & ARRAY_FLAG_USE_2D_VERTICES), Ref<ManifoldMesh>());
		ERR_FAIL_COND_V((format & ARRAY_FORMAT_TANGENT) && (!(format & ARRAY_FORMAT_NORMAL) || !(format & ARRAY_FORMAT_TEX_UV)), Ref<ManifoldMesh>());

		format_union |= format;

		if (likely(array_mesh.is_valid())) {
			const int32_t vertices = array_mesh->surface_get_array_len(surface);
			const int32_t indices = array_mesh->surface_get_array_index_len(surface);
			total_vertices += vertices;
			total_indices += (format & ARRAY_FORMAT_INDEX) ? indices : vertices;
		} else {
			const Array &arrays = p_mesh->surface_get_arrays(surface);
			const PackedVector3Array vertices = arrays[ARRAY_VERTEX];
			const PackedInt32Array indices = arrays[ARRAY_INDEX];
			total_vertices += vertices.size();
			total_indices += (format & ARRAY_FORMAT_INDEX) ? indices.size() : vertices.size();
		}
	}

	// always include normals in what we give to manifold
	// never include tangents - they are computed when we turn back into a mesh
	mesh->_inner->_meshgl.numProp = 6;

	if (format_union & ARRAY_FORMAT_TEX_UV) {
		mesh->_inner->_meshgl.numProp = 8;
	}
	if (format_union & ARRAY_FORMAT_TEX_UV2) {
		mesh->_inner->_meshgl.numProp = 10;
	}
	if (format_union & ARRAY_FORMAT_COLOR) {
		mesh->_inner->_meshgl.numProp = 14;
	}
	if (format_union & ARRAY_FORMAT_CUSTOM0) {
		mesh->_inner->_meshgl.numProp = 18;
	}
	if (format_union & ARRAY_FORMAT_CUSTOM1) {
		mesh->_inner->_meshgl.numProp = 22;
	}
	if (format_union & ARRAY_FORMAT_CUSTOM2) {
		mesh->_inner->_meshgl.numProp = 26;
	}
	if (format_union & ARRAY_FORMAT_CUSTOM3) {
		mesh->_inner->_meshgl.numProp = 30;
	}

	const I stride = mesh->_inner->_meshgl.numProp;

	mesh->_inner->_meshgl.vertProperties.resize(total_vertices * stride);
	mesh->_inner->_meshgl.triVerts.resize(total_indices);
	mesh->_inner->_meshgl.runIndex.resize(num_surfaces + 1);
	mesh->_inner->_meshgl.runOriginalID.resize(num_surfaces);

	mesh->_inner->_meshgl.runIndex[0] = 0;

	I base_vertex = 0, base_index = 0;
	for (int32_t surface = 0; surface < num_surfaces; surface++) {
		ERR_FAIL_COND_V(array_mesh.is_valid() && array_mesh->surface_get_primitive_type(surface) != PRIMITIVE_TRIANGLES, Ref<ManifoldMesh>());

		const uint32_t format = array_mesh.is_valid() ? array_mesh->surface_get_format(surface) : get_surface_format_hack(p_mesh->surface_get_arrays(surface));
		mesh->_surface_formats[surface] = format;

		const Ref<Material> material = p_mesh->surface_get_material(surface);
		const uint32_t original_id = get_material_original_id(material);
		mesh->_surface_original_ids[surface] = original_id;
		mesh->_surface_materials[surface] = material;
		mesh->_surface_names[surface] = array_mesh.is_valid() ? array_mesh->surface_get_name(surface) : String();

		I vertex = base_vertex, index = base_index;

		const Array arrays = p_mesh->surface_get_arrays(surface);

		const PackedVector3Array positions = arrays[ARRAY_VERTEX];
		for (int32_t i = 0; i < positions.size(); i++) {
			mesh->_inner->_meshgl.vertProperties[vertex * stride + 0] = positions[i].x;
			mesh->_inner->_meshgl.vertProperties[vertex * stride + 1] = positions[i].y;
			mesh->_inner->_meshgl.vertProperties[vertex * stride + 2] = positions[i].z;
			vertex++;
		}

		DEV_ASSERT(stride >= 6);
		if (stride >= 6) {
			mesh->_init_normals(arrays, base_vertex, stride);
		}
		if (stride >= 8) {
			mesh->_init_tex_uv(arrays, base_vertex, stride);
		}
		if (stride >= 10) {
			mesh->_init_tex_uv2(arrays, base_vertex, stride);
		}
		if (stride >= 14) {
			mesh->_init_color(arrays, base_vertex, stride);
		}
		if (stride >= 18) {
			mesh->_init_custom(arrays, base_vertex, stride, 14, ARRAY_CUSTOM0, static_cast<ArrayCustomFormat>((format >> ARRAY_FORMAT_CUSTOM0_SHIFT) & ARRAY_FORMAT_CUSTOM_MASK));
		}
		if (stride >= 22) {
			mesh->_init_custom(arrays, base_vertex, stride, 18, ARRAY_CUSTOM1, static_cast<ArrayCustomFormat>((format >> ARRAY_FORMAT_CUSTOM1_SHIFT) & ARRAY_FORMAT_CUSTOM_MASK));
		}
		if (stride >= 26) {
			mesh->_init_custom(arrays, base_vertex, stride, 22, ARRAY_CUSTOM2, static_cast<ArrayCustomFormat>((format >> ARRAY_FORMAT_CUSTOM2_SHIFT) & ARRAY_FORMAT_CUSTOM_MASK));
		}
		if (stride >= 30) {
			mesh->_init_custom(arrays, base_vertex, stride, 26, ARRAY_CUSTOM3, static_cast<ArrayCustomFormat>((format >> ARRAY_FORMAT_CUSTOM3_SHIFT) & ARRAY_FORMAT_CUSTOM_MASK));
		}
		DEV_ASSERT(stride <= 30);

		if (format & ARRAY_FORMAT_INDEX) {
			const PackedInt32Array indices = arrays[ARRAY_INDEX];
			for (int32_t i = 0; i < indices.size(); i += 3) {
				mesh->_inner->_meshgl.triVerts[index++] = base_vertex + indices[i + 0];
				mesh->_inner->_meshgl.triVerts[index++] = base_vertex + indices[i + 2];
				mesh->_inner->_meshgl.triVerts[index++] = base_vertex + indices[i + 1];
			}
		} else {
			for (int32_t i = 0; i < positions.size(); i += 3) {
				mesh->_inner->_meshgl.triVerts[index++] = base_vertex + i + 0;
				mesh->_inner->_meshgl.triVerts[index++] = base_vertex + i + 2;
				mesh->_inner->_meshgl.triVerts[index++] = base_vertex + i + 1;
			}
		}

		mesh->_inner->_meshgl.runIndex[surface + 1] = index;
		mesh->_inner->_meshgl.runOriginalID[surface] = original_id;

		base_vertex = vertex;
		base_index = index;
	}

	DEV_ASSERT(base_vertex == total_vertices);
	DEV_ASSERT(base_index == total_indices);

	mesh->_inner->_meshgl.tolerance = UNIT_EPSILON;

	mesh->_inner->_meshgl.Merge();
	mesh->_inner->_manifold = manifold::Manifold(mesh->_inner->_meshgl);
	mesh->_inner->_has_bad_original_ids = false;

	if (unlikely(mesh->_inner->_manifold.Status() != manifold::Manifold::Error::NoError)) {
		ERR_PRINT(vformat("%s: mesh is non-manifold (%s)", p_mesh, p_mesh->get_path()));
	}

	return mesh;
}

Ref<ArrayMesh> ManifoldMesh::to_mesh(bool p_generate_lods, bool p_create_shadow_mesh, const TypedArray<Material> &p_skip_material) const {
	ERR_FAIL_COND_V(is_empty(), Ref<ArrayMesh>());

	_commit_to_arrays();

	Ref<ImporterMesh> mesh;
	mesh.instantiate();

	for (int32_t i = 0; i < _inner->_arrays.size(); i++) {
		const Ref<Material> material = likely(i < _surface_materials.size()) ? _surface_materials[i] : Variant();
		if (unlikely(p_skip_material.has(material))) {
			continue;
		}

		const PackedVector3Array positions = _inner->_arrays[i][ARRAY_VERTEX];
		if (unlikely(positions.is_empty())) {
			continue;
		}

		const String name = likely(i < _surface_names.size()) ? _surface_names[i] : String();

		mesh->add_surface(Mesh::PRIMITIVE_TRIANGLES, _inner->_arrays[i], TypedArray<Array>(), Dictionary(), material, name, _surface_get_format(i));
	}

	if (p_generate_lods) {
		mesh->generate_lods(60.0f, 0.0f, Array());
	}
#if 0
	// TODO: https://github.com/godotengine/godot/pull/103948
	if (p_create_shadow_mesh) {
		mesh->create_shadow_mesh();
	}
#endif

	return mesh->get_mesh();
}

TypedArray<ManifoldMesh> ManifoldMesh::decompose() const {
	_ensure_manifold();

	const std::vector<manifold::Manifold> decomposed = _inner->_manifold.Decompose();

	TypedArray<ManifoldMesh> decomposed_wrapped;
	decomposed_wrapped.resize(decomposed.size());
	for (size_t i = 0; i < decomposed.size(); i++) {
		decomposed_wrapped[i] = _new_manifold(decomposed[i]);
	}

	return decomposed_wrapped;
}

Ref<ManifoldMesh> ManifoldMesh::compose(const TypedArray<ManifoldMesh> &p_manifolds) {
	Vector<Ref<ManifoldMesh>> wrapped_manifolds;
	wrapped_manifolds.resize(p_manifolds.size());

	std::vector<manifold::Manifold> manifolds;
	manifolds.resize(p_manifolds.size());

	for (int64_t i = 0; i < p_manifolds.size(); i++) {
		const Ref<ManifoldMesh> manifold = p_manifolds[i];
		ERR_FAIL_COND_V(manifold.is_null(), Ref<ManifoldMesh>());
		wrapped_manifolds.write[i] = manifold;
		manifold->_ensure_manifold();
		manifolds[i] = manifold->_inner->_manifold;
	}

	return _new_merged_manifold(manifold::Manifold::Compose(manifolds), wrapped_manifolds);
}
Ref<ManifoldMesh> ManifoldMesh::tetrahedron(const Ref<Material> &p_material) {
	return _primitive(manifold::Manifold::Tetrahedron(), p_material, "tetrahedron");
}
Ref<ManifoldMesh> ManifoldMesh::cube(const Vector3 &p_size, bool p_center, const Ref<Material> &p_material) {
	return _primitive(manifold::Manifold::Cube(to_vec3(p_size), p_center), p_material, "cube");
}
Ref<ManifoldMesh> ManifoldMesh::cylinder(double p_height, double p_radius_low, double p_radius_high, int32_t p_circular_segments, bool p_center, const Ref<Material> &p_material) {
	return _primitive(manifold::Manifold::Cylinder(p_height, p_radius_low, p_radius_high, p_circular_segments, p_center), p_material, "cylinder");
}
Ref<ManifoldMesh> ManifoldMesh::sphere(double p_radius, int32_t p_circular_segments, const Ref<Material> &p_material) {
	return _primitive(manifold::Manifold::Sphere(p_radius, p_circular_segments), p_material, "sphere");
}
Ref<ManifoldMesh> ManifoldMesh::level_set(const Callable &p_sdf, const AABB &p_bounds, double p_edge_length, double p_level, double p_tolerance, const Ref<Material> &p_material) {
	return _primitive(manifold::Manifold::LevelSet([p_sdf](manifold::vec3 p_coord) -> double {
		return p_sdf.call(from_vec3(p_coord));
	}, to_box(p_bounds), p_edge_length, p_level, p_tolerance, false), p_material, "level_set");
}

TypedArray<PackedVector2Array> ManifoldMesh::slice(double p_height) const {
	_ensure_manifold();
	return from_polygons(_inner->_manifold.Slice(p_height));
}
TypedArray<PackedVector2Array> ManifoldMesh::project() const {
	_ensure_manifold();
	return from_polygons(_inner->_manifold.Project());
}
Ref<ManifoldMesh> ManifoldMesh::extrude(const TypedArray<PackedVector2Array> &p_cross_section, double p_height, int32_t p_divisions, double p_twist_degrees, const Vector2 &p_scale_top, const Ref<Material> &p_material) {
	return _primitive(manifold::Manifold::Extrude(to_polygons(p_cross_section), p_height, p_divisions, p_twist_degrees, to_vec2(p_scale_top)), p_material, "extrude");
}
Ref<ManifoldMesh> ManifoldMesh::revolve(const TypedArray<PackedVector2Array> &p_cross_section, int32_t p_circular_segments, double p_revolve_degrees, const Ref<Material> &p_material) {
	return _primitive(manifold::Manifold::Revolve(to_polygons(p_cross_section), p_circular_segments, p_revolve_degrees), p_material, "revolve");
}

size_t ManifoldMesh::get_vertex_count() const {
	_ensure_manifold();
	return _inner->_manifold.NumVert();
}
size_t ManifoldMesh::get_edge_count() const {
	_ensure_manifold();
	return _inner->_manifold.NumEdge();
}
size_t ManifoldMesh::get_triangle_count() const {
	_ensure_manifold();
	return _inner->_manifold.NumTri();
}
size_t ManifoldMesh::get_property_vertex_count() const {
	_ensure_manifold();
	return _inner->_manifold.NumPropVert();
}
AABB ManifoldMesh::get_aabb() const {
	return self_type::_get_aabb();
}

int32_t ManifoldMesh::get_genus() const {
	_ensure_manifold();
	return _inner->_manifold.Genus();
}
double ManifoldMesh::get_surface_area() const {
	_ensure_manifold();
	return _inner->_manifold.SurfaceArea();
}
double ManifoldMesh::get_volume() const {
	_ensure_manifold();
	return _inner->_manifold.Volume();
}
double ManifoldMesh::get_min_gap(const Ref<ManifoldMesh> &p_other, double p_search_length) const {
	ERR_FAIL_COND_V(p_other.is_null(), 0.0);
	p_other->_ensure_manifold();
	_ensure_manifold();
	return _inner->_manifold.MinGap(p_other->_inner->_manifold, p_search_length);
}

Ref<ManifoldMesh> ManifoldMesh::translate(const Vector3 &p_offset) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.Translate(to_vec3(p_offset)));
}
Ref<ManifoldMesh> ManifoldMesh::scale(const Vector3 &p_scale) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.Scale(to_vec3(p_scale)));
}
Ref<ManifoldMesh> ManifoldMesh::rotate(const Vector3 &p_rotation_degrees) const {
	_ensure_manifold();
	// TODO: make sure these rotations match what Godot does
	return _new_manifold(_inner->_manifold.Rotate(p_rotation_degrees.x, p_rotation_degrees.y, p_rotation_degrees.z));
}
Ref<ManifoldMesh> ManifoldMesh::mirror(const Vector3 &p_normal) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.Mirror(to_vec3(p_normal)));
}
Ref<ManifoldMesh> ManifoldMesh::transform(const Transform3D &p_transform) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.Transform(to_mat3x4(p_transform)));
}
Ref<ManifoldMesh> ManifoldMesh::warp(const Callable &p_warp_vertex) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.Warp([p_warp_vertex](manifold::vec3 &p_vec) -> void {
		p_vec = to_vec3(p_warp_vertex.call(from_vec3(p_vec)));
	}));
}
Ref<ManifoldMesh> ManifoldMesh::warp_batch(const Callable &p_warp_vertices) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.WarpBatch([p_warp_vertices](manifold::VecView<manifold::vec3> p_view) -> void {
		PackedVector3Array view;
		view.resize(p_view.size());
		std::transform(p_view.begin(), p_view.end(), view.ptrw(), &from_vec3);

		const PackedVector3Array warped = p_warp_vertices.call(view);
		ERR_FAIL_COND_MSG(warped.size() != int64_t(p_view.size()), "The Callable given to warp_batch should modify the PackedVector3Array argument in-place and return it");

		std::transform(warped.begin(), warped.end(), p_view.begin(), &to_vec3);
	}));
}
Ref<ManifoldMesh> ManifoldMesh::simplify(double p_tolerance) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.Simplify(p_tolerance));
}

Ref<ManifoldMesh> ManifoldMesh::hull() const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.Hull());
}
Ref<ManifoldMesh> ManifoldMesh::refine(int32_t p_subdivisions) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.Refine(p_subdivisions));
}
Ref<ManifoldMesh> ManifoldMesh::refine_to_length(double p_length) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.RefineToLength(p_length));
}
Ref<ManifoldMesh> ManifoldMesh::refine_to_tolerance(double p_tolerance) const {
	_ensure_manifold();
	return _new_manifold(_inner->_manifold.RefineToTolerance(p_tolerance));
}

Ref<ManifoldMesh> ManifoldMesh::union_with(const Ref<ManifoldMesh> &p_with) const {
	ERR_FAIL_COND_V(p_with.is_null(), Ref<ManifoldMesh>());
	p_with->_ensure_manifold();
	_ensure_manifold();
	return _new_merged_manifold(_inner->_manifold + p_with->_inner->_manifold, {{{this}, p_with}});
}
Ref<ManifoldMesh> ManifoldMesh::intersection_with(const Ref<ManifoldMesh> &p_with) const {
	ERR_FAIL_COND_V(p_with.is_null(), Ref<ManifoldMesh>());
	p_with->_ensure_manifold();
	_ensure_manifold();
	return _new_merged_manifold(_inner->_manifold ^ p_with->_inner->_manifold, {{{this}, p_with}});
}
Ref<ManifoldMesh> ManifoldMesh::difference_with(const Ref<ManifoldMesh> &p_with) const {
	ERR_FAIL_COND_V(p_with.is_null(), Ref<ManifoldMesh>());
	p_with->_ensure_manifold();
	_ensure_manifold();
	return _new_merged_manifold(_inner->_manifold - p_with->_inner->_manifold, {{{this}, p_with}});
}
Ref<ManifoldMesh> ManifoldMesh::batch_union(const TypedArray<ManifoldMesh> &p_manifolds) {
	Vector<Ref<ManifoldMesh>> wrapped_manifolds;
	wrapped_manifolds.resize(p_manifolds.size());

	std::vector<manifold::Manifold> manifolds;
	manifolds.resize(p_manifolds.size());

	for (int64_t i = 0; i < p_manifolds.size(); i++) {
		const Ref<ManifoldMesh> manifold = p_manifolds[i];
		ERR_FAIL_COND_V(manifold.is_null(), Ref<ManifoldMesh>());
		wrapped_manifolds.write[i] = manifold;
		manifold->_ensure_manifold();
		manifolds[i] = manifold->_inner->_manifold;
	}

	return _new_merged_manifold(manifold::Manifold::BatchBoolean(manifolds, manifold::OpType::Add), wrapped_manifolds);
}
Ref<ManifoldMesh> ManifoldMesh::batch_intersection(const TypedArray<ManifoldMesh> &p_manifolds) {
	Vector<Ref<ManifoldMesh>> wrapped_manifolds;
	wrapped_manifolds.resize(p_manifolds.size());

	std::vector<manifold::Manifold> manifolds;
	manifolds.resize(p_manifolds.size());

	for (int64_t i = 0; i < p_manifolds.size(); i++) {
		const Ref<ManifoldMesh> manifold = p_manifolds[i];
		ERR_FAIL_COND_V(manifold.is_null(), Ref<ManifoldMesh>());
		wrapped_manifolds.write[i] = manifold;
		manifold->_ensure_manifold();
		manifolds[i] = manifold->_inner->_manifold;
	}

	return _new_merged_manifold(manifold::Manifold::BatchBoolean(manifolds, manifold::OpType::Intersect), wrapped_manifolds);
}
Ref<ManifoldMesh> ManifoldMesh::batch_difference(const TypedArray<ManifoldMesh> &p_manifolds) {
	Vector<Ref<ManifoldMesh>> wrapped_manifolds;
	wrapped_manifolds.resize(p_manifolds.size());

	std::vector<manifold::Manifold> manifolds;
	manifolds.resize(p_manifolds.size());

	for (int64_t i = 0; i < p_manifolds.size(); i++) {
		const Ref<ManifoldMesh> manifold = p_manifolds[i];
		ERR_FAIL_COND_V(manifold.is_null(), Ref<ManifoldMesh>());
		wrapped_manifolds.write[i] = manifold;
		manifold->_ensure_manifold();
		manifolds[i] = manifold->_inner->_manifold;
	}

	return _new_merged_manifold(manifold::Manifold::BatchBoolean(manifolds, manifold::OpType::Subtract), wrapped_manifolds);
}

Pair<Ref<ManifoldMesh>, Ref<ManifoldMesh>> ManifoldMesh::split(const Ref<ManifoldMesh> &p_manifold) const {
	ERR_FAIL_COND_V(p_manifold.is_null(), {});
	p_manifold->_ensure_manifold();
	_ensure_manifold();
	const std::pair<manifold::Manifold, manifold::Manifold> pair = _inner->_manifold.Split(p_manifold->_inner->_manifold);
	return {_new_manifold(pair.first), _new_manifold(pair.second)};
}
TypedArray<ManifoldMesh> ManifoldMesh::split_bind(const Ref<ManifoldMesh> &p_manifold) const {
	const Pair<Ref<ManifoldMesh>, Ref<ManifoldMesh>> pair = split(p_manifold);
	return Array::make(pair.first, pair.second);
}
Pair<Ref<ManifoldMesh>, Ref<ManifoldMesh>> ManifoldMesh::split_by_plane(const Plane &p_plane, const godot::Ref<godot::Material> &p_material) const {
	return split(_halfspace(p_plane, p_material));
}
TypedArray<ManifoldMesh> ManifoldMesh::split_by_plane_bind(const Plane &p_plane, const godot::Ref<godot::Material> &p_material) const {
	const Pair<Ref<ManifoldMesh>, Ref<ManifoldMesh>> pair = split_by_plane(p_plane, p_material);
	return Array::make(pair.first, pair.second);
}
Ref<ManifoldMesh> ManifoldMesh::trim_by_plane(const Plane &p_plane, const godot::Ref<godot::Material> &p_material) const {
	return intersection_with(_halfspace(p_plane, p_material));
}

#define WARN_IF_UNUSED_FORMAT(m_format) \
	{ \
		bool uses_format = false; \
		for (int64_t i = 0; i < _surface_formats.size(); i++) { \
			if (_surface_formats[i] & ARRAY_FORMAT_##m_format) { \
				uses_format = true; \
				break; \
			} \
		} \
		if (unlikely(!uses_format)) { \
			WARN_PRINT("No material in this mesh uses the " #m_format " vertex attribute. Modify the surface_formats property before calling this function."); \
		} \
	}
Ref<ManifoldMesh> ManifoldMesh::modify_normal(const Callable &p_modify, bool p_normalize) const {
	WARN_IF_UNUSED_FORMAT(NORMAL);
	_ensure_manifold();
	const int32_t num_prop = _inner->_manifold.NumProp();
	DEV_ASSERT(num_prop <= 0 || num_prop >= 3);
	return _new_manifold(_inner->_manifold.SetProperties(Math::max(num_prop, 3), [p_modify, p_normalize, num_prop](double *p_new_prop, manifold::vec3 p_position, const double *p_old_prop) -> void {
		Vector3 normal;
		if (num_prop >= 3) {
			normal.x = static_cast<real_t>(p_old_prop[0]);
			normal.y = static_cast<real_t>(p_old_prop[1]);
			normal.z = static_cast<real_t>(p_old_prop[2]);
		}
		normal = p_modify.call(from_vec3(p_position), normal);
		if (p_normalize) {
			normal.normalize();
		}
		p_new_prop[0] = normal.x;
		p_new_prop[1] = normal.y;
		p_new_prop[2] = normal.z;
	}).SmoothByNormals(0));
}
Ref<ManifoldMesh> ManifoldMesh::modify_tex_uv(const Callable &p_modify) const {
	WARN_IF_UNUSED_FORMAT(TEX_UV);
	_ensure_manifold();
	const int32_t num_prop = _inner->_manifold.NumProp();
	DEV_ASSERT(num_prop <= 3 || num_prop >= 5);
	return _new_manifold(_inner->_manifold.SetProperties(Math::max(num_prop, 5), [p_modify, num_prop](double *p_new_prop, manifold::vec3 p_position, const double *p_old_prop) -> void {
		Vector2 uv;
		if (num_prop >= 5) {
			uv.x = static_cast<real_t>(p_old_prop[3]);
			uv.y = static_cast<real_t>(p_old_prop[4]);
		}
		uv = p_modify.call(from_vec3(p_position), uv);
		p_new_prop[3] = uv.x;
		p_new_prop[4] = uv.y;
	}));
}
Ref<ManifoldMesh> ManifoldMesh::modify_tex_uv2(const Callable &p_modify) const {
	WARN_IF_UNUSED_FORMAT(TEX_UV2);
	_ensure_manifold();
	const int32_t num_prop = _inner->_manifold.NumProp();
	DEV_ASSERT(num_prop <= 5 || num_prop >= 7);
	return _new_manifold(_inner->_manifold.SetProperties(Math::max(num_prop, 7), [p_modify, num_prop](double *p_new_prop, manifold::vec3 p_position, const double *p_old_prop) -> void {
		Vector2 uv2;
		if (num_prop >= 7) {
			uv2.x = static_cast<real_t>(p_old_prop[5]);
			uv2.y = static_cast<real_t>(p_old_prop[6]);
		}
		uv2 = p_modify.call(from_vec3(p_position), uv2);
		p_new_prop[5] = uv2.x;
		p_new_prop[6] = uv2.y;
	}));
}
Ref<ManifoldMesh> ManifoldMesh::modify_color(const Callable &p_modify) const {
	WARN_IF_UNUSED_FORMAT(COLOR);
	return _modify_color(7, p_modify);
}
Ref<ManifoldMesh> ManifoldMesh::modify_custom0(const Callable &p_modify) const {
	WARN_IF_UNUSED_FORMAT(CUSTOM0);
	return _modify_color(11, p_modify);
}
Ref<ManifoldMesh> ManifoldMesh::modify_custom1(const Callable &p_modify) const {
	WARN_IF_UNUSED_FORMAT(CUSTOM1);
	return _modify_color(15, p_modify);
}
Ref<ManifoldMesh> ManifoldMesh::modify_custom2(const Callable &p_modify) const {
	WARN_IF_UNUSED_FORMAT(CUSTOM2);
	return _modify_color(19, p_modify);
}
Ref<ManifoldMesh> ManifoldMesh::modify_custom3(const Callable &p_modify) const {
	WARN_IF_UNUSED_FORMAT(CUSTOM3);
	return _modify_color(23, p_modify);
}

void ManifoldMesh::_ensure_manifold() const {
	if (unlikely(_inner->_has_bad_original_ids)) {
		const_cast<ManifoldMesh *>(this)->_reallocate_original_ids();
	}

	if (unlikely(_inner->_manifold_dirty)) {
		DEV_ASSERT(!_inner->_meshgl_dirty);

		_inner->_manifold = manifold::Manifold(_inner->_meshgl);
		_inner->_manifold_dirty = false;

		if (unlikely(_inner->_manifold.Status() != manifold::Manifold::Error::NoError)) {
			ERR_PRINT(vformat("%s: mesh is non-manifold", this));
		}

		if (_inner->_meshgl.halfedgeTangent.empty() && likely(is_valid())) {
			_inner->_manifold = _inner->_manifold.SmoothByNormals(0);
			_inner->_meshgl_dirty = true;
		}
	}
}

void ManifoldMesh::_ensure_meshgl() const {
	if (unlikely(_inner->_meshgl_dirty)) {
		DEV_ASSERT(!_inner->_manifold_dirty);
		ERR_FAIL_COND(!is_valid());
#ifdef REAL_T_IS_DOUBLE
		_inner->_meshgl = _inner->_manifold.GetMeshGL64(0);
#else
		_inner->_meshgl = _inner->_manifold.GetMeshGL(0);
#endif
		_inner->_arrays.clear();
		_inner->_meshgl_dirty = false;
	}
}

void ManifoldMesh::_reallocate_original_ids() {
	DEV_ASSERT(_inner->_has_bad_original_ids);
	DEV_ASSERT(!_inner->_meshgl_dirty);

	if (unlikely(_surface_original_ids.size() > _surface_materials.size())) {
		ERR_PRINT(vformat("%s surface ID array is %d elements, but only %d materials; padding with nulls", this, _surface_original_ids.size(), _surface_materials.size()));
		_surface_materials.resize(_surface_original_ids.size());
	}

	HashMap<uint32_t, uint32_t> replace;
	for (int32_t i = 0; i < _surface_original_ids.size(); i++) {
		const uint32_t new_original_id = get_material_original_id(_surface_materials[i]);
		DEV_ASSERT(!replace.has(_surface_original_ids[i]) || replace.get(_surface_original_ids[i]) == new_original_id);
		replace[_surface_original_ids[i]] = new_original_id;
		_surface_original_ids[i] = new_original_id;
	}

	for (uint32_t &original_id : _inner->_meshgl.runOriginalID) {
		ERR_CONTINUE(!replace.has(original_id));
		original_id = replace.get(original_id);
	}

	_inner->_has_bad_original_ids = false;
	_inner->_manifold_dirty = true;
	emit_changed();
}

static Variant _encode_custom_array(Mesh::ArrayCustomFormat format, const PackedColorArray &colors) {
	PackedByteArray bytes;
	PackedFloat32Array floats;

	switch (format) {
	case Mesh::ARRAY_CUSTOM_RGBA8_UNORM:
		bytes.resize(colors.size() * 4);
		for (int64_t i = 0, j = 0; i < colors.size(); i++, j += 4) {
			bytes[j + 0] = uint8_t(Math::clamp(colors[i].r * 255.0f, 0.0f, 255.0f));
			bytes[j + 1] = uint8_t(Math::clamp(colors[i].g * 255.0f, 0.0f, 255.0f));
			bytes[j + 2] = uint8_t(Math::clamp(colors[i].b * 255.0f, 0.0f, 255.0f));
			bytes[j + 3] = uint8_t(Math::clamp(colors[i].a * 255.0f, 0.0f, 255.0f));
		}
		return bytes;
	case Mesh::ARRAY_CUSTOM_RGBA8_SNORM:
		bytes.resize(colors.size() * 4);
		for (int64_t i = 0, j = 0; i < colors.size(); i++, j += 4) {
			bytes[j + 0] = uint8_t(int8_t(Math::clamp(colors[i].r * 127.0f, -127.0f, 127.0f)));
			bytes[j + 1] = uint8_t(int8_t(Math::clamp(colors[i].g * 127.0f, -127.0f, 127.0f)));
			bytes[j + 2] = uint8_t(int8_t(Math::clamp(colors[i].b * 127.0f, -127.0f, 127.0f)));
			bytes[j + 3] = uint8_t(int8_t(Math::clamp(colors[i].a * 127.0f, -127.0f, 127.0f)));
		}
		return bytes;
	case Mesh::ARRAY_CUSTOM_RG_HALF:
		bytes.resize(colors.size() * 4);
		for (int64_t i = 0, j = 0; i < colors.size(); i++, j += 4) {
			bytes.encode_half(j + 0, colors[i].r);
			bytes.encode_half(j + 2, colors[i].g);
		}
		return bytes;
	case Mesh::ARRAY_CUSTOM_RGBA_HALF:
		bytes.resize(colors.size() * 8);
		for (int64_t i = 0, j = 0; i < colors.size(); i++, j += 8) {
			bytes.encode_half(j + 0, colors[i].r);
			bytes.encode_half(j + 2, colors[i].g);
			bytes.encode_half(j + 4, colors[i].b);
			bytes.encode_half(j + 6, colors[i].a);
		}
		return bytes;
	case Mesh::ARRAY_CUSTOM_R_FLOAT:
		floats.resize(colors.size());
		for (int64_t i = 0; i < colors.size(); i++) {
			floats[i] = colors[i].r;
		}
		return floats;
	case Mesh::ARRAY_CUSTOM_RG_FLOAT:
		floats.resize(colors.size() * 2);
		for (int64_t i = 0, j = 0; i < colors.size(); i++, j += 2) {
			floats[j + 0] = colors[i].r;
			floats[j + 1] = colors[i].g;
		}
		return floats;
	case Mesh::ARRAY_CUSTOM_RGB_FLOAT:
		floats.resize(colors.size() * 3);
		for (int64_t i = 0, j = 0; i < colors.size(); i++, j += 3) {
			floats[j + 0] = colors[i].r;
			floats[j + 1] = colors[i].g;
			floats[j + 2] = colors[i].b;
		}
		return floats;
	case Mesh::ARRAY_CUSTOM_RGBA_FLOAT:
		floats.resize(colors.size() * 4);
		for (int64_t i = 0, j = 0; i < colors.size(); i++, j += 4) {
			floats[j + 0] = colors[i].r;
			floats[j + 1] = colors[i].g;
			floats[j + 2] = colors[i].b;
			floats[j + 3] = colors[i].a;
		}
		return floats;
	case Mesh::ARRAY_CUSTOM_MAX:
		break;
	}

	ERR_FAIL_V(Variant());
}

void ManifoldMesh::_commit_to_arrays() const {
	_ensure_meshgl();

	if (likely(_inner->_arrays.size() == _surface_materials.size())) {
		return;
	}

	_inner->_arrays.resize(_surface_materials.size());

	for (int32_t i = 0; i < _surface_materials.size(); i++) {
		Array array;
		array.resize(Mesh::ARRAY_MAX);

		if (likely(i < _surface_original_ids.size())) {
			PackedVector3Array positions, normals;
			PackedVector2Array tex_uv, tex_uv2;
			PackedColorArray colors, custom0, custom1, custom2, custom3;

			_unpack_to_arrays(_surface_original_ids[i], positions, normals, tex_uv, tex_uv2, colors, custom0, custom1, custom2, custom3);

			const BitField<ArrayFormat> format = _surface_get_format(i);
			if (format.has_flag(ARRAY_FORMAT_VERTEX)) {
				array[ARRAY_VERTEX] = positions;
			}
			if (format.has_flag(Mesh::ARRAY_FORMAT_NORMAL)) {
				array[ARRAY_NORMAL] = normals;
			}
			if (format.has_flag(Mesh::ARRAY_FORMAT_TEX_UV)) {
				array[ARRAY_TEX_UV] = tex_uv;
			}
			if (format.has_flag(Mesh::ARRAY_FORMAT_TEX_UV2)) {
				array[ARRAY_TEX_UV2] = tex_uv2;
			}
			if (format.has_flag(Mesh::ARRAY_FORMAT_COLOR)) {
				array[ARRAY_COLOR] = colors;
			}
			if (format.has_flag(Mesh::ARRAY_FORMAT_CUSTOM0)) {
				array[ARRAY_CUSTOM0] = _encode_custom_array(static_cast<ArrayCustomFormat>((format >> ARRAY_FORMAT_CUSTOM0_SHIFT) & ARRAY_FORMAT_CUSTOM_MASK), custom0);
			}
			if (format.has_flag(ARRAY_FORMAT_CUSTOM1)) {
				array[ARRAY_CUSTOM1] = _encode_custom_array(static_cast<ArrayCustomFormat>((format >> ARRAY_FORMAT_CUSTOM1_SHIFT) & ARRAY_FORMAT_CUSTOM_MASK), custom1);
			}
			if (format.has_flag(ARRAY_FORMAT_CUSTOM2)) {
				array[ARRAY_CUSTOM2] = _encode_custom_array(static_cast<ArrayCustomFormat>((format >> ARRAY_FORMAT_CUSTOM2_SHIFT) & ARRAY_FORMAT_CUSTOM_MASK), custom2);
			}
			if (format.has_flag(ARRAY_FORMAT_CUSTOM3)) {
				array[ARRAY_CUSTOM3] = _encode_custom_array(static_cast<ArrayCustomFormat>((format >> ARRAY_FORMAT_CUSTOM3_SHIFT) & ARRAY_FORMAT_CUSTOM_MASK), custom3);
			}

			if (format.has_flag(ARRAY_FORMAT_TANGENT)) {
				Ref<SurfaceTool> st;
				st.instantiate();

				st->create_from_arrays(array);
				st->generate_tangents();

				const Array st_array = st->commit_to_arrays();
				array[ARRAY_TANGENT] = st_array[ARRAY_TANGENT];
			}
		}

		_inner->_arrays.write[i] = array;
	}
}

void ManifoldMesh::_unpack_to_arrays(uint32_t original_id, PackedVector3Array &positions, PackedVector3Array &normals, PackedVector2Array &tex_uv, PackedVector2Array &tex_uv2, PackedColorArray &colors, PackedColorArray &custom0, PackedColorArray &custom1, PackedColorArray &custom2, PackedColorArray &custom3) const {
	DEV_ASSERT(!_inner->_meshgl_dirty);

	size_t num_runs = _inner->_meshgl.runIndex.size() - 1;
	for (size_t i = 0; i < num_runs; i++) {
		if (_inner->_meshgl.runOriginalID[i] != original_id) {
			continue;
		}

		const size_t first_index = _inner->_meshgl.runIndex[i];
		const size_t last_index = _inner->_meshgl.runIndex[i + 1];
		for (size_t j0 = first_index; j0 < last_index; j0 += 3) {
			for (size_t j : {j0 + 0, j0 + 2, j0 + 1}) {
				const uint32_t vertex = _inner->_meshgl.triVerts[j] * _inner->_meshgl.numProp;

				DEV_ASSERT(_inner->_meshgl.numProp >= 3);
				if (_inner->_meshgl.numProp >= 3) {
					positions.append(Vector3(_inner->_meshgl.vertProperties[vertex], _inner->_meshgl.vertProperties[vertex + 1], _inner->_meshgl.vertProperties[vertex + 2]));
				}

				DEV_ASSERT(_inner->_meshgl.numProp <= 3 || _inner->_meshgl.numProp >= 6);
				if (_inner->_meshgl.numProp >= 6) {
					normals.append(Vector3(_inner->_meshgl.vertProperties[vertex + 3], _inner->_meshgl.vertProperties[vertex + 4], _inner->_meshgl.vertProperties[vertex + 5]));
				}

				DEV_ASSERT(_inner->_meshgl.numProp <= 6 || _inner->_meshgl.numProp >= 8);
				if (_inner->_meshgl.numProp >= 8) {
					tex_uv.append(Vector2(_inner->_meshgl.vertProperties[vertex + 6], _inner->_meshgl.vertProperties[vertex + 7]));
				}

				DEV_ASSERT(_inner->_meshgl.numProp <= 8 || _inner->_meshgl.numProp >= 10);
				if (_inner->_meshgl.numProp >= 10) {
					tex_uv2.append(Vector2(_inner->_meshgl.vertProperties[vertex + 8], _inner->_meshgl.vertProperties[vertex + 9]));
				}

				DEV_ASSERT(_inner->_meshgl.numProp <= 10 || _inner->_meshgl.numProp >= 14);
				if (_inner->_meshgl.numProp >= 14) {
					colors.append(Color(_inner->_meshgl.vertProperties[vertex + 10], _inner->_meshgl.vertProperties[vertex + 11], _inner->_meshgl.vertProperties[vertex + 12], _inner->_meshgl.vertProperties[vertex + 13]));
				}

				DEV_ASSERT(_inner->_meshgl.numProp <= 14 || _inner->_meshgl.numProp >= 18);
				if (_inner->_meshgl.numProp >= 18) {
					custom0.append(Color(_inner->_meshgl.vertProperties[vertex + 14], _inner->_meshgl.vertProperties[vertex + 15], _inner->_meshgl.vertProperties[vertex + 16], _inner->_meshgl.vertProperties[vertex + 17]));
				}

				DEV_ASSERT(_inner->_meshgl.numProp <= 18 || _inner->_meshgl.numProp >= 22);
				if (_inner->_meshgl.numProp >= 22) {
					custom1.append(Color(_inner->_meshgl.vertProperties[vertex + 18], _inner->_meshgl.vertProperties[vertex + 19], _inner->_meshgl.vertProperties[vertex + 20], _inner->_meshgl.vertProperties[vertex + 21]));
				}

				DEV_ASSERT(_inner->_meshgl.numProp <= 22 || _inner->_meshgl.numProp >= 26);
				if (_inner->_meshgl.numProp >= 26) {
					custom2.append(Color(_inner->_meshgl.vertProperties[vertex + 22], _inner->_meshgl.vertProperties[vertex + 23], _inner->_meshgl.vertProperties[vertex + 24], _inner->_meshgl.vertProperties[vertex + 25]));
				}

				DEV_ASSERT(_inner->_meshgl.numProp <= 26 || _inner->_meshgl.numProp >= 30);
				if (_inner->_meshgl.numProp >= 30) {
					custom3.append(Color(_inner->_meshgl.vertProperties[vertex + 26], _inner->_meshgl.vertProperties[vertex + 27], _inner->_meshgl.vertProperties[vertex + 28], _inner->_meshgl.vertProperties[vertex + 29]));
				}
			}
		}
	}
}

void ManifoldMesh::_init_normals(const Array &arrays, I vertex, I stride) {
	PackedVector3Array normals;
	if (likely(arrays[ARRAY_NORMAL] != Variant())) {
		normals = arrays[ARRAY_NORMAL];
	} else {
		// we aren't going to export normals for this material, but we should still give manifold realistic normals.
		Ref<SurfaceTool> st;
		st.instantiate();

		st->create_from_arrays(arrays);
		st->generate_normals();

		const Array st_arrays = st->commit_to_arrays();
		normals = st_arrays[ARRAY_NORMAL];
	}

	for (I i = 0; i < normals.size(); i++) {
		_inner->_meshgl.vertProperties[vertex * stride + 3] = normals[i].x;
		_inner->_meshgl.vertProperties[vertex * stride + 4] = normals[i].y;
		_inner->_meshgl.vertProperties[vertex * stride + 5] = normals[i].z;
		vertex++;
	}
}
void ManifoldMesh::_init_tex_uv(const Array &arrays, I vertex, I stride) {
	PackedVector2Array uv;
	if (likely(arrays[ARRAY_TEX_UV] != Variant())) {
		uv = arrays[ARRAY_TEX_UV];
	} else {
		uv.resize(arrays[ARRAY_VERTEX].operator PackedVector3Array().size());
	}

	for (I i = 0; i < uv.size(); i++) {
		_inner->_meshgl.vertProperties[vertex * stride + 6] = uv[i].x;
		_inner->_meshgl.vertProperties[vertex * stride + 7] = uv[i].y;
		vertex++;
	}
}
void ManifoldMesh::_init_tex_uv2(const Array &arrays, I vertex, I stride) {
	PackedVector2Array uv;
	if (likely(arrays[ARRAY_TEX_UV2] != Variant())) {
		uv = arrays[ARRAY_TEX_UV2];
	} else {
		uv.resize(arrays[ARRAY_VERTEX].operator PackedVector3Array().size());
	}

	for (I i = 0; i < uv.size(); i++) {
		_inner->_meshgl.vertProperties[vertex * stride + 8] = uv[i].x;
		_inner->_meshgl.vertProperties[vertex * stride + 9] = uv[i].y;
		vertex++;
	}
}
void ManifoldMesh::_init_color(const Array &arrays, I vertex, I stride) {
	PackedColorArray color;
	if (likely(arrays[ARRAY_COLOR] != Variant())) {
		color = arrays[ARRAY_COLOR];
	} else {
		color.resize(arrays[ARRAY_VERTEX].operator PackedVector3Array().size());
		// for consistency, set alpha to 0 like it would be if manifold extended the properties array
		color.fill(Color(0, 0, 0, 0));
	}

	for (I i = 0; i < color.size(); i++) {
		_inner->_meshgl.vertProperties[vertex * stride + 10] = color[i].r;
		_inner->_meshgl.vertProperties[vertex * stride + 11] = color[i].g;
		_inner->_meshgl.vertProperties[vertex * stride + 12] = color[i].b;
		_inner->_meshgl.vertProperties[vertex * stride + 13] = color[i].a;
		vertex++;
	}
}
void ManifoldMesh::_init_custom(const Array &arrays, I vertex, I stride, I offset, ArrayType type, ArrayCustomFormat custom_format) {
	if (unlikely(arrays[type] == Variant())) {
		for (I i = 0; i < arrays[ARRAY_VERTEX].operator PackedVector3Array().size(); i++) {
			_inner->_meshgl.vertProperties[vertex * stride + offset + 0] = 0;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 1] = 0;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 2] = 0;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 3] = 0;
			vertex++;
		}

		return;
	}

	PackedByteArray bytes;
	PackedFloat32Array floats;

	switch (custom_format) {
	case ARRAY_CUSTOM_RGBA8_UNORM:
		bytes = arrays[type];
		DEV_ASSERT(bytes.size() % 4 == 0);

		for (I i = 0; i < bytes.size(); i += 4) {
			_inner->_meshgl.vertProperties[vertex * stride + offset + 0] = bytes[i + 0] / 255.0f;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 1] = bytes[i + 1] / 255.0f;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 2] = bytes[i + 2] / 255.0f;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 3] = bytes[i + 3] / 255.0f;
			vertex++;
		}

		break;
	case ARRAY_CUSTOM_RGBA8_SNORM:
		bytes = arrays[type];
		DEV_ASSERT(bytes.size() % 4 == 0);

		for (I i = 0; i < bytes.size(); i += 4) {
			_inner->_meshgl.vertProperties[vertex * stride + offset + 0] = Math::max(int8_t(bytes[i + 0]) / 127.0f, -1.0f);
			_inner->_meshgl.vertProperties[vertex * stride + offset + 1] = Math::max(int8_t(bytes[i + 1]) / 127.0f, -1.0f);
			_inner->_meshgl.vertProperties[vertex * stride + offset + 2] = Math::max(int8_t(bytes[i + 2]) / 127.0f, -1.0f);
			_inner->_meshgl.vertProperties[vertex * stride + offset + 3] = Math::max(int8_t(bytes[i + 3]) / 127.0f, -1.0f);
			vertex++;
		}

		break;
	case ARRAY_CUSTOM_RG_HALF:
		bytes = arrays[type];
		DEV_ASSERT(bytes.size() % 4 == 0);

		for (I i = 0; i < bytes.size(); i += 4) {
			_inner->_meshgl.vertProperties[vertex * stride + offset + 0] = bytes.decode_half(i + 0);
			_inner->_meshgl.vertProperties[vertex * stride + offset + 1] = bytes.decode_half(i + 2);
			_inner->_meshgl.vertProperties[vertex * stride + offset + 2] = 0;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 3] = 0;
			vertex++;
		}

		break;
	case ARRAY_CUSTOM_RGBA_HALF:
		bytes = arrays[type];
		DEV_ASSERT(bytes.size() % 8 == 0);

		for (I i = 0; i < bytes.size(); i += 4) {
			_inner->_meshgl.vertProperties[vertex * stride + offset + 0] = bytes.decode_half(i + 0);
			_inner->_meshgl.vertProperties[vertex * stride + offset + 1] = bytes.decode_half(i + 2);
			_inner->_meshgl.vertProperties[vertex * stride + offset + 2] = bytes.decode_half(i + 4);
			_inner->_meshgl.vertProperties[vertex * stride + offset + 3] = bytes.decode_half(i + 6);
			vertex++;
		}

		break;
	case ARRAY_CUSTOM_R_FLOAT:
		floats = arrays[type];
		DEV_ASSERT(floats.size() % 1 == 0);

		for (I i = 0; i < floats.size(); i++) {
			_inner->_meshgl.vertProperties[vertex * stride + offset + 0] = floats[i + 0];
			_inner->_meshgl.vertProperties[vertex * stride + offset + 1] = 0;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 2] = 0;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 3] = 0;
			vertex++;
		}

		break;
	case ARRAY_CUSTOM_RG_FLOAT:
		floats = arrays[type];
		DEV_ASSERT(floats.size() % 2 == 0);

		for (I i = 0; i < floats.size(); i += 2) {
			_inner->_meshgl.vertProperties[vertex * stride + offset + 0] = floats[i + 0];
			_inner->_meshgl.vertProperties[vertex * stride + offset + 1] = floats[i + 1];
			_inner->_meshgl.vertProperties[vertex * stride + offset + 2] = 0;
			_inner->_meshgl.vertProperties[vertex * stride + offset + 3] = 0;
			vertex++;
		}

		break;
	case ARRAY_CUSTOM_RGB_FLOAT:
		floats = arrays[type];
		DEV_ASSERT(floats.size() % 3 == 0);

		for (I i = 0; i < floats.size(); i += 3) {
			_inner->_meshgl.vertProperties[vertex * stride + offset + 0] = floats[i + 0];
			_inner->_meshgl.vertProperties[vertex * stride + offset + 1] = floats[i + 1];
			_inner->_meshgl.vertProperties[vertex * stride + offset + 2] = floats[i + 2];
			_inner->_meshgl.vertProperties[vertex * stride + offset + 3] = 0;
			vertex++;
		}

		break;
	case ARRAY_CUSTOM_RGBA_FLOAT:
		floats = arrays[type];
		DEV_ASSERT(floats.size() % 4 == 0);

		for (I i = 0; i < floats.size(); i += 4) {
			_inner->_meshgl.vertProperties[vertex * stride + offset + 0] = floats[i + 0];
			_inner->_meshgl.vertProperties[vertex * stride + offset + 1] = floats[i + 1];
			_inner->_meshgl.vertProperties[vertex * stride + offset + 2] = floats[i + 2];
			_inner->_meshgl.vertProperties[vertex * stride + offset + 3] = floats[i + 3];
			vertex++;
		}

		break;
	case ARRAY_CUSTOM_MAX:
		break;
	}
}

Ref<ManifoldMesh> ManifoldMesh::_primitive(const manifold::Manifold &new_manifold, const Ref<Material> &material, const String &name) {
	Ref<ManifoldMesh> m;
	m.instantiate();

	m->_surface_formats.append(ARRAY_FORMAT_VERTEX | ARRAY_FORMAT_NORMAL);
	m->_surface_original_ids.append(new_manifold.OriginalID());
	m->_surface_materials.append(material);
	m->_surface_names.append(name);

	m->_inner->_manifold = new_manifold.CalculateNormals(0).SmoothByNormals(0);
#ifdef REAL_T_IS_DOUBLE
	m->_inner->_meshgl = m->_inner->_manifold.GetMeshGL64(0);
#else
	m->_inner->_meshgl = m->_inner->_manifold.GetMeshGL(0);
#endif
	m->_reallocate_original_ids();

	return m;
}

Ref<ManifoldMesh> ManifoldMesh::_new_merged_manifold(const manifold::Manifold &new_manifold, const Vector<Ref<ManifoldMesh>> &originals) {
	Ref<ManifoldMesh> m;
	m.instantiate();

	m->_inner->_manifold = new_manifold;
	m->_inner->_meshgl_dirty = true;
	m->_inner->_has_bad_original_ids = false;

	for (const Ref<ManifoldMesh> &original : originals) {
		for (int32_t i = 0; i < original->_surface_original_ids.size(); i++) {
			const uint32_t original_id = original->_surface_original_ids[i];
			const int64_t index = m->_surface_original_ids.find(original_id);
			if (index != -1) {
				m->_surface_formats[index] |= original->_surface_get_format(i);
				continue;
			}

			m->_surface_formats.append(original->_surface_get_format(i));
			m->_surface_original_ids.append(original_id);
			m->_surface_materials.append(original->_surface_get_material(i));
			m->_surface_names.append(likely(i < original->_surface_names.size()) ? original->_surface_names[i] : String());
		}
	}

	return m;
}

Ref<ManifoldMesh> ManifoldMesh::_new_manifold(const manifold::Manifold &new_manifold) const {
	Ref<ManifoldMesh> m;
	m.instantiate();

	m->_surface_formats = const_cast<PackedInt32Array *>(&_surface_formats)->duplicate();
	m->_surface_original_ids = const_cast<PackedInt32Array *>(&_surface_original_ids)->duplicate();
	m->_surface_materials = _surface_materials.duplicate();
	m->_surface_names = const_cast<PackedStringArray *>(&_surface_names)->duplicate();

	m->_inner->_manifold = new_manifold;
	m->_inner->_meshgl_dirty = true;
	m->_inner->_has_bad_original_ids = false;

	return m;
}

Ref<ManifoldMesh> ManifoldMesh::_modify_color(const int32_t min_prop, const Callable &p_modify) const {
	_ensure_manifold();
	const int32_t num_prop = _inner->_manifold.NumProp();
	DEV_ASSERT(num_prop <= min_prop || num_prop >= min_prop + 4);
	return _new_manifold(_inner->_manifold.SetProperties(Math::max(num_prop, min_prop + 4), [p_modify, min_prop, num_prop](double *p_new_prop, manifold::vec3 p_position, const double *p_old_prop) -> void {
		Color color;
		if (num_prop >= min_prop + 4) {
			color.r = static_cast<float>(p_old_prop[min_prop + 0]);
			color.g = static_cast<float>(p_old_prop[min_prop + 1]);
			color.b = static_cast<float>(p_old_prop[min_prop + 2]);
			color.a = static_cast<float>(p_old_prop[min_prop + 3]);
		} else {
			color.a = 0.0f;
		}
		color = p_modify.call(from_vec3(p_position), color);
		p_new_prop[min_prop + 0] = color.r;
		p_new_prop[min_prop + 1] = color.g;
		p_new_prop[min_prop + 2] = color.b;
		p_new_prop[min_prop + 3] = color.a;
	}));
}
Ref<ManifoldMesh> ManifoldMesh::_halfspace(const Plane &p_plane, const Ref<Material> &p_material) const {
	_ensure_manifold();

	// copied from manifold.cpp
	manifold::Box bBox = _inner->_manifold.BoundingBox();
	manifold::vec3 normal = manifold::la::normalize(to_vec3(p_plane.normal));
	manifold::Manifold cutter = manifold::Manifold::Cube(manifold::vec3(2.0), true).Translate({1.0, 0.0, 0.0});
	double size = manifold::la::length(bBox.Center() - normal * double(p_plane.d)) + 0.5 * manifold::la::length(bBox.Size());
	cutter = cutter.Scale(manifold::vec3(size)).Translate({p_plane.d, 0.0, 0.0});
	double yDeg = manifold::degrees(-std::asin(normal.z));
	double zDeg = manifold::degrees(std::atan2(normal.y, normal.x));
	cutter = cutter.Rotate(0.0, yDeg, zDeg);

	return _primitive(cutter, p_material, "halfspace");
}
