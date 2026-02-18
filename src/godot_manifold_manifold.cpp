#include "godot_manifold_defs.h"
#include "godot_manifold_converters.h"

#include <godot_cpp/core/class_db.hpp>

#include <manifold/manifold.h>

using namespace godot;

void Manifold::_bind_methods() {
	ClassDB::bind_method(D_METHOD("to_mesh32", "normal_idx"), &Manifold::to_mesh32, DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("to_mesh64", "normal_idx"), &Manifold::to_mesh64, DEFVAL(-1));

	ClassDB::bind_method(D_METHOD("decompose"), &Manifold::decompose);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("compose", "manifolds"), &Manifold::compose);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("tetrahedron"), &Manifold::tetrahedron);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("cube", "size", "center"), &Manifold::cube, DEFVAL(Vector3(1.0f, 1.0f, 1.0f)), DEFVAL(false));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("cylinder", "height", "radius_low", "radius_high", "circular_segments", "center"), &Manifold::cylinder, DEFVAL(-1.0), DEFVAL(0), DEFVAL(false));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("sphere", "radius", "circular_segments"), &Manifold::sphere, DEFVAL(0));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("level_set", "sdf", "bounds", "edge_length", "level", "tolerance"), &Manifold::level_set_bind, DEFVAL(0), DEFVAL(-1));

	ClassDB::bind_method(D_METHOD("slice", "height"), &Manifold::slice, DEFVAL(0));
	ClassDB::bind_method(D_METHOD("project"), &Manifold::project);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("extrude", "cross_section", "height", "num_divisions", "twist_degrees", "scale_top"), &Manifold::extrude, DEFVAL(0), DEFVAL(0.0), DEFVAL(Vector2(1.0f, 1.0f)));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("revolve", "cross_section", "circular_segments", "revolve_degrees"), &Manifold::revolve, DEFVAL(0), DEFVAL(360.0f));

	BIND_ENUM_CONSTANT(NO_ERROR);
	BIND_ENUM_CONSTANT(ERROR_NON_FINITE_VERTEX);
	BIND_ENUM_CONSTANT(ERROR_NOT_MANIFOLD);
	BIND_ENUM_CONSTANT(ERROR_VERTEX_OUT_OF_BOUNDS);
	BIND_ENUM_CONSTANT(ERROR_PROPERTIES_WRONG_LENGTH);
	BIND_ENUM_CONSTANT(ERROR_MISSING_POSITION_PROPERTIES);
	BIND_ENUM_CONSTANT(ERROR_MERGE_VECTORS_DIFFERENT_LENGTHS);
	BIND_ENUM_CONSTANT(ERROR_MERGE_INDEX_OUT_OF_BOUNDS);
	BIND_ENUM_CONSTANT(ERROR_TRANSFORM_WRONG_LENGTH);
	BIND_ENUM_CONSTANT(ERROR_RUN_INDEX_WRONG_LENGTH);
	BIND_ENUM_CONSTANT(ERROR_FACE_ID_WRONG_LENGTH);
	BIND_ENUM_CONSTANT(ERROR_INVALID_CONSTRUCTION);
	BIND_ENUM_CONSTANT(ERROR_RESULT_TOO_LARGE);

	ClassDB::bind_method(D_METHOD("status"), &Manifold::status);
	ClassDB::bind_method(D_METHOD("is_empty"), &Manifold::is_empty);
	ClassDB::bind_method(D_METHOD("num_vert"), &Manifold::num_vert);
	ClassDB::bind_method(D_METHOD("num_edge"), &Manifold::num_edge);
	ClassDB::bind_method(D_METHOD("num_tri"), &Manifold::num_tri);
	ClassDB::bind_method(D_METHOD("num_prop"), &Manifold::num_prop);
	ClassDB::bind_method(D_METHOD("num_prop_vert"), &Manifold::num_prop_vert);
	ClassDB::bind_method(D_METHOD("bounding_box"), &Manifold::bounding_box);
	ClassDB::bind_method(D_METHOD("genus"), &Manifold::genus);
	ClassDB::bind_method(D_METHOD("get_tolerance"), &Manifold::get_tolerance);

	ClassDB::bind_method(D_METHOD("surface_area"), &Manifold::surface_area);
	ClassDB::bind_method(D_METHOD("volume"), &Manifold::volume);
	ClassDB::bind_method(D_METHOD("min_gap", "other", "search_length"), &Manifold::min_gap);

	ClassDB::bind_method(D_METHOD("original_id"), &Manifold::original_id);
	ClassDB::bind_method(D_METHOD("as_original"), &Manifold::as_original);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("reserve_ids", "count"), &Manifold::reserve_ids);

	ClassDB::bind_method(D_METHOD("translate", "offset"), &Manifold::translate);
	ClassDB::bind_method(D_METHOD("scale", "scale"), &Manifold::scale);
	ClassDB::bind_method(D_METHOD("rotate", "x_degrees", "y_degrees", "z_degrees"), &Manifold::rotate, DEFVAL(0.0), DEFVAL(0.0));
	ClassDB::bind_method(D_METHOD("mirror", "axis"), &Manifold::mirror);
	ClassDB::bind_method(D_METHOD("transform", "transform"), &Manifold::transform);
	ClassDB::bind_method(D_METHOD("warp", "func"), &Manifold::warp_bind);
	ClassDB::bind_method(D_METHOD("set_tolerance", "tolerance"), &Manifold::set_tolerance);
	ClassDB::bind_method(D_METHOD("simplify", "tolerance"), &Manifold::simplify, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("union_with", "second"), &Manifold::union_with);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("union_batch", "manifolds"), &Manifold::union_batch);
	ClassDB::bind_method(D_METHOD("intersection_with", "second"), &Manifold::intersection_with);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("intersection_batch", "manifolds"), &Manifold::intersection_batch);
	ClassDB::bind_method(D_METHOD("difference_with", "second"), &Manifold::difference_with);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("difference_batch", "manifolds"), &Manifold::difference_batch);
	ClassDB::bind_method(D_METHOD("split", "manifold"), &Manifold::split_bind);
	ClassDB::bind_method(D_METHOD("split_by_plane", "plane"), &Manifold::split_by_plane_bind);
	ClassDB::bind_method(D_METHOD("trim_by_plane", "plane"), &Manifold::trim_by_plane);

	ClassDB::bind_method(D_METHOD("set_properties", "num_prop", "prop_func"), &Manifold::set_properties_bind);
	ClassDB::bind_method(D_METHOD("calculate_curvature", "gaussian_idx", "mean_idx"), &Manifold::calculate_curvature);
	ClassDB::bind_method(D_METHOD("calculate_normals", "normal_idx", "min_sharp_angle"), &Manifold::calculate_normals, DEFVAL(60));

	ClassDB::bind_method(D_METHOD("refine", "splits"), &Manifold::refine);
	ClassDB::bind_method(D_METHOD("refine_to_length", "length"), &Manifold::refine_to_length);
	ClassDB::bind_method(D_METHOD("refine_to_tolerance", "tolerance"), &Manifold::refine_to_tolerance);
	ClassDB::bind_method(D_METHOD("smooth_by_normals", "normal_idx"), &Manifold::smooth_by_normals);
	ClassDB::bind_method(D_METHOD("smooth_out", "min_sharp_angle", "min_smoothness"), &Manifold::smooth_out, DEFVAL(60), DEFVAL(0));

	ClassDB::bind_method(D_METHOD("hull"), &Manifold::hull);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("hull_batch", "manifolds"), &Manifold::hull_batch);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("hull_points", "points"), &Manifold::hull_points);
}

struct Manifold::Inner {
	manifold::Manifold _manifold;

	static _FORCE_INLINE_ std::vector<manifold::Manifold> to_manifold_vec(const TypedArray<Manifold> &p_manifolds) {
		std::vector<manifold::Manifold> v;
		v.resize(p_manifolds.size());
		std::transform(p_manifolds.begin(), p_manifolds.end(), v.begin(), [](const Ref<Manifold> &p_manifold) -> const manifold::Manifold & {
			static const manifold::Manifold empty_manifold;
			ERR_FAIL_NULL_V(*p_manifold, empty_manifold);
			return p_manifold->_inner->_manifold;
		});
		return v;
	}
	static _FORCE_INLINE_ TypedArray<Manifold> from_manifold_vec(const std::vector<manifold::Manifold> &p_manifolds) {
		TypedArray<Manifold> a;
		a.resize(p_manifolds.size());
		std::transform(p_manifolds.begin(), p_manifolds.end(), a.begin(), [](const manifold::Manifold &p_manifold) -> Ref<Manifold> {
			return memnew(Manifold(p_manifold));
		});
		return a;
	}
};

Manifold::Manifold() {
	_inner = memnew(Inner);
}
Manifold::Manifold(const manifold::Manifold &p_manifold) {
	_inner = memnew(Inner);
	_inner->_manifold = p_manifold;
}
Manifold::~Manifold() {
	memdelete(_inner);
	_inner = nullptr;
}

Ref<ManifoldMesh32> Manifold::to_mesh32(int p_normal_idx) const {
	return memnew(ManifoldMesh32(_inner->_manifold.GetMeshGL(p_normal_idx)));
}
Ref<ManifoldMesh64> Manifold::to_mesh64(int p_normal_idx) const {
	return memnew(ManifoldMesh64(_inner->_manifold.GetMeshGL64(p_normal_idx)));
}

TypedArray<Manifold> Manifold::decompose() const {
	return Inner::from_manifold_vec(_inner->_manifold.Decompose());
}
Ref<Manifold> Manifold::compose(const TypedArray<Manifold> &p_manifolds) {
	return memnew(Manifold(manifold::Manifold::Compose(Inner::to_manifold_vec(p_manifolds))));
}
Ref<Manifold> Manifold::tetrahedron() {
	return memnew(Manifold(manifold::Manifold::Tetrahedron()));
}
Ref<Manifold> Manifold::cube(Vector3 p_size, bool p_center) {
	return memnew(Manifold(manifold::Manifold::Cube(to_vec3(p_size), p_center)));
}
Ref<Manifold> Manifold::cylinder(double p_height, double p_radius_low, double p_radius_high, int p_circular_segments, bool p_center) {
	return memnew(Manifold(manifold::Manifold::Cylinder(p_height, p_radius_low, p_radius_high, p_circular_segments, p_center)));
}
Ref<Manifold> Manifold::sphere(double p_radius, int p_circular_segments) {
	return memnew(Manifold(manifold::Manifold::Sphere(p_radius, p_circular_segments)));
}
Ref<Manifold> Manifold::level_set_bind(const Callable &p_sdf, AABB p_bounds, double p_edge_length, double p_level, double p_tolerance) {
	return level_set([p_sdf](Vector3 p_vec) -> double {
		return p_sdf.call(p_vec);
	}, p_bounds, p_edge_length, p_level, p_tolerance);
}
Ref<Manifold> Manifold::level_set(const std::function<double(Vector3)> &p_sdf, AABB p_bounds, double p_edge_length, double p_level, double p_tolerance) {
	const auto sdf = [p_sdf](manifold::vec3 p_vec) -> double {
		return p_sdf(from_vec3(p_vec));
	};
	return memnew(Manifold(manifold::Manifold::LevelSet(sdf, to_box(p_bounds), p_edge_length, p_level, p_tolerance)));
}

TypedArray<PackedVector2Array> Manifold::slice(double p_height) const {
	return from_polygons(_inner->_manifold.Slice(p_height));
}
TypedArray<PackedVector2Array> Manifold::project() const {
	return from_polygons(_inner->_manifold.Project());
}
Ref<Manifold> Manifold::extrude(const TypedArray<PackedVector2Array> &p_cross_section, double p_height, int p_num_divisions, double p_twist_degrees, Vector2 p_scale_top) {
	return memnew(Manifold(manifold::Manifold::Extrude(to_polygons(p_cross_section), p_height, p_num_divisions, p_twist_degrees, to_vec2(p_scale_top))));
}
Ref<Manifold> Manifold::revolve(const TypedArray<PackedVector2Array> &p_cross_section, int p_circular_segments, double p_revolve_degrees) {
	return memnew(Manifold(manifold::Manifold::Revolve(to_polygons(p_cross_section), p_circular_segments, p_revolve_degrees)));
}

Manifold::Error Manifold::status() const {
	return static_cast<Error>(_inner->_manifold.Status());
}
bool Manifold::is_empty() const {
	return _inner->_manifold.IsEmpty();
}
int64_t Manifold::num_vert() const {
	return _inner->_manifold.NumVert();
}
int64_t Manifold::num_edge() const {
	return _inner->_manifold.NumEdge();
}
int64_t Manifold::num_tri() const {
	return _inner->_manifold.NumTri();
}
int64_t Manifold::num_prop() const {
	return _inner->_manifold.NumProp();
}
int64_t Manifold::num_prop_vert() const {
	return _inner->_manifold.NumPropVert();
}
AABB Manifold::bounding_box() const {
	return from_box(_inner->_manifold.BoundingBox());
}
int Manifold::genus() const {
	return _inner->_manifold.Genus();
}
double Manifold::get_tolerance() const {
	return _inner->_manifold.GetTolerance();
}

double Manifold::surface_area() const {
	return _inner->_manifold.SurfaceArea();
}
double Manifold::volume() const {
	return _inner->_manifold.Volume();
}
double Manifold::min_gap(const Ref<Manifold> &p_other, double p_search_length) const {
	ERR_FAIL_NULL_V(*p_other, 0);
	return _inner->_manifold.MinGap(p_other->_inner->_manifold, p_search_length);
}

int Manifold::original_id() const {
	return _inner->_manifold.OriginalID();
}
Ref<Manifold> Manifold::as_original() const {
	return memnew(Manifold(_inner->_manifold.AsOriginal()));
}
uint32_t Manifold::reserve_ids(uint32_t p_count) {
	return manifold::Manifold::ReserveIDs(p_count);
}

Ref<Manifold> Manifold::translate(Vector3 p_offset) const {
	return memnew(Manifold(_inner->_manifold.Translate(to_vec3(p_offset))));
}
Ref<Manifold> Manifold::scale(Vector3 p_scale) const {
	return memnew(Manifold(_inner->_manifold.Scale(to_vec3(p_scale))));
}
Ref<Manifold> Manifold::rotate(double p_x_degrees, double p_y_degrees, double p_z_degrees) const {
	return memnew(Manifold(_inner->_manifold.Rotate(p_x_degrees, p_y_degrees, p_z_degrees)));
}
Ref<Manifold> Manifold::mirror(Vector3 p_axis) const {
	return memnew(Manifold(_inner->_manifold.Mirror(to_vec3(p_axis))));
}
Ref<Manifold> Manifold::transform(const Transform3D &p_transform) const {
	return memnew(Manifold(_inner->_manifold.Transform(to_mat3x4(p_transform))));
}
Ref<Manifold> Manifold::warp_bind(const Callable &p_func) const {
	return warp([p_func](Vector3 p_coord) -> Vector3 {
		return p_func.call(p_coord);
	});
}
Ref<Manifold> Manifold::warp(const std::function<Vector3(Vector3)> &p_func) const {
	const auto func = [p_func](manifold::vec3 &p_coord) -> void {
		p_coord = to_vec3(p_func(from_vec3(p_coord)));
	};
	return memnew(Manifold(_inner->_manifold.Warp(func)));
}
Ref<Manifold> Manifold::set_tolerance(double p_tolerance) const {
	return memnew(Manifold(_inner->_manifold.SetTolerance(p_tolerance)));
}
Ref<Manifold> Manifold::simplify(double p_tolerance) const {
	return memnew(Manifold(_inner->_manifold.Simplify(p_tolerance)));
}

Ref<Manifold> Manifold::union_with(const Ref<Manifold> &p_second) const {
	ERR_FAIL_NULL_V(*p_second, const_cast<Manifold *>(this));
	return memnew(Manifold(_inner->_manifold.Boolean(p_second->_inner->_manifold, manifold::OpType::Add)));
}
Ref<Manifold> Manifold::union_batch(const TypedArray<Manifold> &p_manifolds) {
	return memnew(Manifold(manifold::Manifold::BatchBoolean(Inner::to_manifold_vec(p_manifolds), manifold::OpType::Add)));
}
Ref<Manifold> Manifold::intersection_with(const Ref<Manifold> &p_second) const {
	ERR_FAIL_NULL_V(*p_second, const_cast<Manifold *>(this));
	return memnew(Manifold(_inner->_manifold.Boolean(p_second->_inner->_manifold, manifold::OpType::Intersect)));
}
Ref<Manifold> Manifold::intersection_batch(const TypedArray<Manifold> &p_manifolds) {
	return memnew(Manifold(manifold::Manifold::BatchBoolean(Inner::to_manifold_vec(p_manifolds), manifold::OpType::Intersect)));
}
Ref<Manifold> Manifold::difference_with(const Ref<Manifold> &p_second) const {
	ERR_FAIL_NULL_V(*p_second, const_cast<Manifold *>(this));
	return memnew(Manifold(_inner->_manifold.Boolean(p_second->_inner->_manifold, manifold::OpType::Subtract)));
}
Ref<Manifold> Manifold::difference_batch(const TypedArray<Manifold> &p_manifolds) {
	return memnew(Manifold(manifold::Manifold::BatchBoolean(Inner::to_manifold_vec(p_manifolds), manifold::OpType::Subtract)));
}
TypedArray<Manifold> Manifold::split_bind(const Ref<Manifold> &p_manifold) const {
	const Pair<Ref<Manifold>, Ref<Manifold>> s = split(p_manifold);
	return Array::make(s.first, s.second);
}
Pair<Ref<Manifold>, Ref<Manifold>> Manifold::split(const Ref<Manifold> &p_manifold) const {
	ERR_FAIL_NULL_V(*p_manifold, {});
	const std::pair<manifold::Manifold, manifold::Manifold> s = _inner->_manifold.Split(p_manifold->_inner->_manifold);
	return {memnew(Manifold(s.first)), memnew(Manifold(s.second))};
}
TypedArray<Manifold> Manifold::split_by_plane_bind(Plane p_plane) const {
	const Pair<Ref<Manifold>, Ref<Manifold>> s = split_by_plane(p_plane);
	return Array::make(s.first, s.second);
}
Pair<Ref<Manifold>, Ref<Manifold>> Manifold::split_by_plane(Plane p_plane) const {
	const std::pair<manifold::Manifold, manifold::Manifold> s = _inner->_manifold.SplitByPlane(to_vec3(p_plane.normal), p_plane.d);
	return {memnew(Manifold(s.first)), memnew(Manifold(s.second))};
}
Ref<Manifold> Manifold::trim_by_plane(Plane p_plane) const {
	return memnew(Manifold(_inner->_manifold.TrimByPlane(to_vec3(p_plane.normal), p_plane.d)));
}

Ref<Manifold> Manifold::set_properties_bind(int p_num_prop, const Callable &p_prop_func) const {
	return set_properties(p_num_prop, [p_prop_func](Vector3 p_coord, const PackedFloat64Array &p_old_props) -> PackedFloat64Array {
		return p_prop_func.call(p_coord, p_old_props);
	});
}
Ref<Manifold> Manifold::set_properties(int p_num_prop, const std::function<PackedFloat64Array(Vector3, const PackedFloat64Array &)> &p_prop_func) const {
	PackedFloat64Array old_props;
	const size_t num_old_props = _inner->_manifold.NumProp();
	old_props.resize(num_old_props);
	const auto prop_func = [p_prop_func, p_num_prop, &old_props, num_old_props](double *p_new_props, manifold::vec3 p_coord, const double *p_old_props) -> void {
		memcpy(old_props.ptrw(), p_old_props, sizeof(double) * num_old_props);
		const PackedFloat64Array new_props = p_prop_func(from_vec3(p_coord), old_props);
		memcpy(p_new_props, new_props.ptr(), sizeof(double) * Math::min(int(new_props.size()), p_num_prop));
	};
	return memnew(Manifold(_inner->_manifold.SetProperties(p_num_prop, prop_func)));
}
Ref<Manifold> Manifold::calculate_curvature(int p_gaussian_idx, int p_mean_idx) const {
	return memnew(Manifold(_inner->_manifold.CalculateCurvature(p_gaussian_idx, p_mean_idx)));
}
Ref<Manifold> Manifold::calculate_normals(int p_normal_idx, double p_min_sharp_angle) const {
	return memnew(Manifold(_inner->_manifold.CalculateCurvature(p_normal_idx, p_min_sharp_angle)));
}

Ref<Manifold> Manifold::refine(int p_splits) const {
	return memnew(Manifold(_inner->_manifold.Refine(p_splits)));
}
Ref<Manifold> Manifold::refine_to_length(double p_length) const {
	return memnew(Manifold(_inner->_manifold.RefineToLength(p_length)));
}
Ref<Manifold> Manifold::refine_to_tolerance(double p_tolerance) const {
	return memnew(Manifold(_inner->_manifold.RefineToTolerance(p_tolerance)));
}
Ref<Manifold> Manifold::smooth_by_normals(int p_normal_idx) const {
	return memnew(Manifold(_inner->_manifold.SmoothByNormals(p_normal_idx)));
}
Ref<Manifold> Manifold::smooth_out(double p_min_sharp_angle, double p_min_smoothness) const {
	return memnew(Manifold(_inner->_manifold.SmoothOut(p_min_sharp_angle, p_min_smoothness)));
}

Ref<Manifold> Manifold::hull() const {
	return memnew(Manifold(_inner->_manifold.Hull()));
}
Ref<Manifold> Manifold::hull_batch(const TypedArray<Manifold> &p_manifolds) {
	return memnew(Manifold(manifold::Manifold::Hull(Inner::to_manifold_vec(p_manifolds))));
}
Ref<Manifold> Manifold::hull_points(const PackedVector3Array &p_points) {
	std::vector<manifold::vec3> points;
	points.resize(p_points.size());
	std::transform(p_points.begin(), p_points.end(), points.begin(), &to_vec3);
	return memnew(Manifold(manifold::Manifold::Hull(points)));
}
