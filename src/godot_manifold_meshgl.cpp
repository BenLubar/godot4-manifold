#include "godot_manifold_defs.h"
#include "godot_manifold_converters.h"

#include <godot_cpp/core/class_db.hpp>

#include <manifold/manifold.h>

using namespace godot;

void ManifoldMesh32::_bind_methods() {
	ClassDB::bind_method(D_METHOD("num_vert"), &ManifoldMesh32::num_vert);
	ClassDB::bind_method(D_METHOD("num_tri"), &ManifoldMesh32::num_tri);

	ClassDB::bind_method(D_METHOD("get_num_prop"), &ManifoldMesh32::get_num_prop);
	ClassDB::bind_method(D_METHOD("set_num_prop", "num_prop"), &ManifoldMesh32::set_num_prop);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "num_prop"), "set_num_prop", "get_num_prop");

	ClassDB::bind_method(D_METHOD("get_vert_properties"), &ManifoldMesh32::get_vert_properties);
	ClassDB::bind_method(D_METHOD("set_vert_properties", "vert_properties"), &ManifoldMesh32::set_vert_properties);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "vert_properties"), "set_vert_properties", "get_vert_properties");

	ClassDB::bind_method(D_METHOD("get_tri_verts"), &ManifoldMesh32::get_tri_verts);
	ClassDB::bind_method(D_METHOD("set_tri_verts", "tri_verts"), &ManifoldMesh32::set_tri_verts);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "tri_verts"), "set_tri_verts", "get_tri_verts");

	ClassDB::bind_method(D_METHOD("get_merge_from_vert"), &ManifoldMesh32::get_merge_from_vert);
	ClassDB::bind_method(D_METHOD("set_merge_from_vert", "merge_from_vert"), &ManifoldMesh32::set_merge_from_vert);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "merge_from_vert"), "set_merge_from_vert", "get_merge_from_vert");

	ClassDB::bind_method(D_METHOD("get_merge_to_vert"), &ManifoldMesh32::get_merge_to_vert);
	ClassDB::bind_method(D_METHOD("set_merge_to_vert", "merge_to_vert"), &ManifoldMesh32::set_merge_to_vert);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "merge_to_vert"), "set_merge_to_vert", "get_merge_to_vert");

	ClassDB::bind_method(D_METHOD("get_run_index"), &ManifoldMesh32::get_run_index);
	ClassDB::bind_method(D_METHOD("set_run_index", "run_index"), &ManifoldMesh32::set_run_index);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "run_index"), "set_run_index", "get_run_index");

	ClassDB::bind_method(D_METHOD("get_run_original_id"), &ManifoldMesh32::get_run_original_id);
	ClassDB::bind_method(D_METHOD("set_run_original_id", "run_original_id"), &ManifoldMesh32::set_run_original_id);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "run_original_id"), "set_run_original_id", "get_run_original_id");

	ClassDB::bind_method(D_METHOD("get_run_transform"), &ManifoldMesh32::get_run_transform);
	ClassDB::bind_method(D_METHOD("set_run_transform", "run_transform"), &ManifoldMesh32::set_run_transform);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "run_transform"), "set_run_transform", "get_run_transform");

	ClassDB::bind_method(D_METHOD("get_face_id"), &ManifoldMesh32::get_face_id);
	ClassDB::bind_method(D_METHOD("set_face_id", "face_id"), &ManifoldMesh32::set_face_id);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "face_id"), "set_face_id", "get_face_id");

	ClassDB::bind_method(D_METHOD("get_halfedge_tangent"), &ManifoldMesh32::get_halfedge_tangent);
	ClassDB::bind_method(D_METHOD("set_halfedge_tangent", "halfedge_tangent"), &ManifoldMesh32::set_halfedge_tangent);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT32_ARRAY, "halfedge_tangent"), "set_halfedge_tangent", "get_halfedge_tangent");

	ClassDB::bind_method(D_METHOD("get_tolerance"), &ManifoldMesh32::get_tolerance);
	ClassDB::bind_method(D_METHOD("set_tolerance", "tolerance"), &ManifoldMesh32::set_tolerance);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tolerance"), "set_tolerance", "get_tolerance");

	ClassDB::bind_method(D_METHOD("merge"), &ManifoldMesh32::merge);
	ClassDB::bind_method(D_METHOD("to_manifold"), &ManifoldMesh32::to_manifold);
	ClassDB::bind_method(D_METHOD("to_manifold_with_original_id", "original_id"), &ManifoldMesh32::to_manifold_with_original_id);
}
void ManifoldMesh64::_bind_methods() {
	ClassDB::bind_method(D_METHOD("num_vert"), &ManifoldMesh64::num_vert);
	ClassDB::bind_method(D_METHOD("num_tri"), &ManifoldMesh64::num_tri);

	ClassDB::bind_method(D_METHOD("get_num_prop"), &ManifoldMesh64::get_num_prop);
	ClassDB::bind_method(D_METHOD("set_num_prop", "num_prop"), &ManifoldMesh64::set_num_prop);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "num_prop"), "set_num_prop", "get_num_prop");

	ClassDB::bind_method(D_METHOD("get_vert_properties"), &ManifoldMesh64::get_vert_properties);
	ClassDB::bind_method(D_METHOD("set_vert_properties", "vert_properties"), &ManifoldMesh64::set_vert_properties);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT64_ARRAY, "vert_properties"), "set_vert_properties", "get_vert_properties");

	ClassDB::bind_method(D_METHOD("get_tri_verts"), &ManifoldMesh64::get_tri_verts);
	ClassDB::bind_method(D_METHOD("set_tri_verts", "tri_verts"), &ManifoldMesh64::set_tri_verts);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT64_ARRAY, "tri_verts"), "set_tri_verts", "get_tri_verts");

	ClassDB::bind_method(D_METHOD("get_merge_from_vert"), &ManifoldMesh64::get_merge_from_vert);
	ClassDB::bind_method(D_METHOD("set_merge_from_vert", "merge_from_vert"), &ManifoldMesh64::set_merge_from_vert);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT64_ARRAY, "merge_from_vert"), "set_merge_from_vert", "get_merge_from_vert");

	ClassDB::bind_method(D_METHOD("get_merge_to_vert"), &ManifoldMesh64::get_merge_to_vert);
	ClassDB::bind_method(D_METHOD("set_merge_to_vert", "merge_to_vert"), &ManifoldMesh64::set_merge_to_vert);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT64_ARRAY, "merge_to_vert"), "set_merge_to_vert", "get_merge_to_vert");

	ClassDB::bind_method(D_METHOD("get_run_index"), &ManifoldMesh64::get_run_index);
	ClassDB::bind_method(D_METHOD("set_run_index", "run_index"), &ManifoldMesh64::set_run_index);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT64_ARRAY, "run_index"), "set_run_index", "get_run_index");

	ClassDB::bind_method(D_METHOD("get_run_original_id"), &ManifoldMesh64::get_run_original_id);
	ClassDB::bind_method(D_METHOD("set_run_original_id", "run_original_id"), &ManifoldMesh64::set_run_original_id);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT32_ARRAY, "run_original_id"), "set_run_original_id", "get_run_original_id");

	ClassDB::bind_method(D_METHOD("get_run_transform"), &ManifoldMesh64::get_run_transform);
	ClassDB::bind_method(D_METHOD("set_run_transform", "run_transform"), &ManifoldMesh64::set_run_transform);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT64_ARRAY, "run_transform"), "set_run_transform", "get_run_transform");

	ClassDB::bind_method(D_METHOD("get_face_id"), &ManifoldMesh64::get_face_id);
	ClassDB::bind_method(D_METHOD("set_face_id", "face_id"), &ManifoldMesh64::set_face_id);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_INT64_ARRAY, "face_id"), "set_face_id", "get_face_id");

	ClassDB::bind_method(D_METHOD("get_halfedge_tangent"), &ManifoldMesh64::get_halfedge_tangent);
	ClassDB::bind_method(D_METHOD("set_halfedge_tangent", "halfedge_tangent"), &ManifoldMesh64::set_halfedge_tangent);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_FLOAT64_ARRAY, "halfedge_tangent"), "set_halfedge_tangent", "get_halfedge_tangent");

	ClassDB::bind_method(D_METHOD("get_tolerance"), &ManifoldMesh64::get_tolerance);
	ClassDB::bind_method(D_METHOD("set_tolerance", "tolerance"), &ManifoldMesh64::set_tolerance);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "tolerance"), "set_tolerance", "get_tolerance");

	ClassDB::bind_method(D_METHOD("merge"), &ManifoldMesh64::merge);
	ClassDB::bind_method(D_METHOD("to_manifold"), &ManifoldMesh64::to_manifold);
	ClassDB::bind_method(D_METHOD("to_manifold_with_original_id", "original_id"), &ManifoldMesh64::to_manifold_with_original_id);
}

struct ManifoldMesh32::Inner {
	manifold::MeshGL _meshgl;
};
struct ManifoldMesh64::Inner {
	manifold::MeshGL64 _meshgl;
};

ManifoldMesh32::ManifoldMesh32() {
	_inner = memnew(Inner);
}
ManifoldMesh64::ManifoldMesh64() {
	_inner = memnew(Inner);
}
ManifoldMesh32::ManifoldMesh32(const manifold::MeshGL &p_meshgl) {
	_inner = memnew(Inner);
	_inner->_meshgl = p_meshgl;
}
ManifoldMesh64::ManifoldMesh64(const manifold::MeshGL64 &p_meshgl) {
	_inner = memnew(Inner);
	_inner->_meshgl = p_meshgl;
}
ManifoldMesh32::~ManifoldMesh32() {
	memdelete(_inner);
	_inner = nullptr;
}
ManifoldMesh64::~ManifoldMesh64() {
	memdelete(_inner);
	_inner = nullptr;
}

uint32_t ManifoldMesh32::num_vert() const {
	return _inner->_meshgl.NumVert();
}
uint64_t ManifoldMesh64::num_vert() const {
	return _inner->_meshgl.NumVert();
}
uint32_t ManifoldMesh32::num_tri() const {
	return _inner->_meshgl.NumTri();
}
uint64_t ManifoldMesh64::num_tri() const {
	return _inner->_meshgl.NumTri();
}

uint32_t ManifoldMesh32::get_num_prop() const {
	return _inner->_meshgl.numProp;
}
uint64_t ManifoldMesh64::get_num_prop() const {
	return _inner->_meshgl.numProp;
}
void ManifoldMesh32::set_num_prop(uint32_t p_num_prop) {
	_inner->_meshgl.numProp = p_num_prop;
	emit_changed();
}
void ManifoldMesh64::set_num_prop(uint64_t p_num_prop) {
	_inner->_meshgl.numProp = p_num_prop;
	emit_changed();
}

#define WRAP_VECTOR(m_class, m_array, m_name, m_struct_name, m_size_multiple) \
	m_array m_class::get_##m_name() const { \
		m_array a; \
		static_assert(sizeof(a[0]) == sizeof(_inner->_meshgl.m_struct_name[0])); \
		a.resize(_inner->_meshgl.m_struct_name.size()); \
		std::copy(_inner->_meshgl.m_struct_name.begin(), _inner->_meshgl.m_struct_name.end(), a.ptrw()); \
		return a; \
	} \
	void m_class::set_##m_name(const m_array &p_##m_name) { \
		if (unlikely(p_##m_name.size() % m_size_multiple != 0)) { \
			WARN_PRINT(#m_name " must be a multiple of " #m_size_multiple " entries in length"); \
		} \
		_inner->_meshgl.m_struct_name.resize(p_##m_name.size()); \
		std::copy(p_##m_name.begin(), p_##m_name.end(), _inner->_meshgl.m_struct_name.begin()); \
		emit_changed(); \
	}
#define WRAP_VECTOR_SIZE(m_type, m_name, m_struct_name, m_size_multiple) \
	WRAP_VECTOR(ManifoldMesh32, Packed##m_type##32Array, m_name, m_struct_name, m_size_multiple) \
	WRAP_VECTOR(ManifoldMesh64, Packed##m_type##64Array, m_name, m_struct_name, m_size_multiple)

WRAP_VECTOR_SIZE(Float, vert_properties, vertProperties, 1)
WRAP_VECTOR_SIZE(Int, tri_verts, triVerts, 3)
WRAP_VECTOR_SIZE(Int, merge_from_vert, mergeFromVert, 1)
WRAP_VECTOR_SIZE(Int, merge_to_vert, mergeToVert, 1)
WRAP_VECTOR_SIZE(Int, run_index, runIndex, 1)
WRAP_VECTOR(ManifoldMesh32, PackedInt32Array, run_original_id, runOriginalID, 1)
WRAP_VECTOR(ManifoldMesh64, PackedInt32Array, run_original_id, runOriginalID, 1)
WRAP_VECTOR_SIZE(Float, run_transform, runTransform, 12)
WRAP_VECTOR_SIZE(Int, face_id, faceID, 1)
WRAP_VECTOR_SIZE(Float, halfedge_tangent, halfedgeTangent, 12)

float ManifoldMesh32::get_tolerance() const {
	return _inner->_meshgl.tolerance;
}
double ManifoldMesh64::get_tolerance() const {
	return _inner->_meshgl.tolerance;
}
void ManifoldMesh32::set_tolerance(float p_tolerance) {
	_inner->_meshgl.tolerance = p_tolerance;
	emit_changed();
}
void ManifoldMesh64::set_tolerance(double p_tolerance) {
	_inner->_meshgl.tolerance = p_tolerance;
	emit_changed();
}

bool ManifoldMesh32::merge() {
	if (_inner->_meshgl.Merge()) {
		emit_changed();
		return true;
	}
	return false;
}
bool ManifoldMesh64::merge() {
	if (_inner->_meshgl.Merge()) {
		emit_changed();
		return true;
	}
	return false;
}

Ref<Manifold> ManifoldMesh32::to_manifold() const {
	return memnew(Manifold(manifold::Manifold(_inner->_meshgl)));
}
Ref<Manifold> ManifoldMesh32::to_manifold_with_original_id(uint32_t p_original_id) const {
	manifold::MeshGL mesh = _inner->_meshgl;
	mesh.runIndex = {0};
	mesh.runOriginalID = {p_original_id};
	mesh.runTransform = {};
	return memnew(Manifold(manifold::Manifold(mesh)));
}
Ref<Manifold> ManifoldMesh64::to_manifold() const {
	return memnew(Manifold(manifold::Manifold(_inner->_meshgl)));
}
Ref<Manifold> ManifoldMesh64::to_manifold_with_original_id(uint32_t p_original_id) const {
	manifold::MeshGL64 mesh = _inner->_meshgl;
	mesh.runIndex = {0};
	mesh.runOriginalID = {p_original_id};
	mesh.runTransform = {};
	return memnew(Manifold(manifold::Manifold(mesh)));
}
