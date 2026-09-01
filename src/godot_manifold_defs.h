#pragma once

#include <gdextension_interface.h>

#include <functional>

#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/templates/pair.hpp>

namespace godot {
class ArrayMesh;
}

namespace manifold {
class CrossSection;
template <typename, typename>
struct MeshGLP;
class Manifold;
} //namespace manifold

class Manifold;

class CrossSection : public godot::RefCounted {
	GDCLASS(CrossSection, godot::RefCounted);

protected:
	static void _bind_methods();

public:
	enum FillRule {
		EVEN_ODD,
		NON_ZERO,
		POSITIVE,
		NEGATIVE,
	};

	enum JoinType {
		SQUARE,
		ROUND,
		MITER,
	};

	CrossSection();
	CrossSection(const manifold::CrossSection &p_cross_section);
	~CrossSection();

	static godot::Ref<CrossSection> from_simple_polygon(const godot::PackedVector2Array &p_simple_polygon, FillRule p_fill_rule = POSITIVE);
	static godot::Ref<CrossSection> from_polygons(const godot::TypedArray<godot::PackedVector2Array> &p_polygons, FillRule p_fill_rule = POSITIVE);
	static godot::Ref<CrossSection> from_rect(const godot::Rect2 &p_rect);
	godot::TypedArray<godot::PackedVector2Array> to_polygons() const;
	godot::TypedArray<godot::PackedVector2Array> to_convex_polygons() const;
	godot::PackedVector2Array to_triangles() const;
	godot::PackedVector2Array to_triangles_with_vertices_from(const godot::TypedArray<CrossSection> &p_others) const;

	godot::TypedArray<CrossSection> decompose() const;
	static godot::Ref<CrossSection> compose(const godot::TypedArray<CrossSection> &p_cross_sections);
	static godot::Ref<CrossSection> square(const godot::Vector2 &p_dimensions, bool p_center = false);
	static godot::Ref<CrossSection> circle(double p_radius, int32_t p_circular_segments = 0);

	bool is_empty() const;
	uint64_t get_vertex_count() const;
	uint64_t get_contour_count() const;
	godot::Rect2 get_bounds() const;
	double get_area() const;

	godot::Ref<CrossSection> translate(const godot::Vector2 &p_offset) const;
	godot::Ref<CrossSection> rotate(double p_degrees) const;
	godot::Ref<CrossSection> scale(const godot::Vector2 &p_scale) const;
	godot::Ref<CrossSection> mirror(const godot::Vector2 &p_axis) const;
	godot::Ref<CrossSection> transform(const godot::Transform2D &p_transform) const;
	godot::Ref<CrossSection> warp(const godot::Callable &p_warp_vertex) const;
	godot::Ref<CrossSection> warp_batch(const godot::Callable &p_warp_vertices) const;
	godot::Ref<CrossSection> simplify(double p_epsilon = 1e-6) const;
	godot::Ref<CrossSection> offset(double p_delta, JoinType p_join_type, double p_miter_limit = 2.0, int32_t p_circular_segments = 0) const;

	godot::Ref<CrossSection> union_with(const godot::Ref<CrossSection> &p_with) const;
	godot::Ref<CrossSection> intersection_with(const godot::Ref<CrossSection> &p_with) const;
	godot::Ref<CrossSection> difference_with(const godot::Ref<CrossSection> &p_with) const;
	static godot::Ref<CrossSection> batch_union(const godot::TypedArray<CrossSection> &p_cross_sections);
	static godot::Ref<CrossSection> batch_intersection(const godot::TypedArray<CrossSection> &p_cross_sections);
	static godot::Ref<CrossSection> batch_difference(const godot::TypedArray<CrossSection> &p_cross_sections);

	godot::Ref<CrossSection> hull() const;
	static godot::Ref<CrossSection> from_simple_hull(const godot::PackedVector2Array &p_simple_polygon);
	static godot::Ref<CrossSection> from_complex_hull(const godot::TypedArray<godot::PackedVector2Array> &p_polygons);
	static godot::Ref<CrossSection> from_combined_hull(const godot::TypedArray<CrossSection> &p_cross_sections);

private:
	struct Inner;
	Inner *_inner;
};
VARIANT_ENUM_CAST(CrossSection::FillRule);
VARIANT_ENUM_CAST(CrossSection::JoinType);

class ManifoldMesh32 : public godot::Resource {
	GDCLASS(ManifoldMesh32, godot::Resource);

protected:
	static void _bind_methods();

public:
	ManifoldMesh32();
	ManifoldMesh32(const manifold::MeshGLP<float, uint32_t> &p_meshgl);
	~ManifoldMesh32();

	uint32_t num_vert() const;
	uint32_t num_tri() const;
	uint32_t get_num_prop() const;
	void set_num_prop(uint32_t p_num_prop);
	godot::PackedFloat32Array get_vert_properties() const;
	void set_vert_properties(const godot::PackedFloat32Array &p_vert_properties);
	godot::PackedInt32Array get_tri_verts() const;
	void set_tri_verts(const godot::PackedInt32Array &p_tri_verts);
	godot::PackedInt32Array get_merge_from_vert() const;
	void set_merge_from_vert(const godot::PackedInt32Array &p_merge_from_vert);
	godot::PackedInt32Array get_merge_to_vert() const;
	void set_merge_to_vert(const godot::PackedInt32Array &p_merge_to_vert);
	godot::PackedInt32Array get_run_index() const;
	void set_run_index(const godot::PackedInt32Array &p_run_index);
	godot::PackedInt32Array get_run_original_id() const;
	void set_run_original_id(const godot::PackedInt32Array &p_run_original_id);
	godot::PackedFloat32Array get_run_transform() const;
	void set_run_transform(const godot::PackedFloat32Array &p_run_transform);
	godot::PackedInt32Array get_face_id() const;
	void set_face_id(const godot::PackedInt32Array &p_face_id);
	godot::PackedFloat32Array get_halfedge_tangent() const;
	void set_halfedge_tangent(const godot::PackedFloat32Array &p_halfedge_tangent);
	float get_tolerance() const;
	void set_tolerance(float p_tolerance);

	bool merge();
	godot::Ref<Manifold> to_manifold() const;
	godot::Ref<Manifold> to_manifold_with_original_id(uint32_t p_original_id) const;

private:
	struct Inner;
	Inner *_inner;
};

class ManifoldMesh64 : public godot::Resource {
	GDCLASS(ManifoldMesh64, godot::Resource);

protected:
	static void _bind_methods();

public:
	ManifoldMesh64();
	ManifoldMesh64(const manifold::MeshGLP<double, uint64_t> &p_meshgl);
	~ManifoldMesh64();

	uint64_t num_vert() const;
	uint64_t num_tri() const;
	uint64_t get_num_prop() const;
	void set_num_prop(uint64_t p_num_prop);
	godot::PackedFloat64Array get_vert_properties() const;
	void set_vert_properties(const godot::PackedFloat64Array &p_vert_properties);
	godot::PackedInt64Array get_tri_verts() const;
	void set_tri_verts(const godot::PackedInt64Array &p_tri_verts);
	godot::PackedInt64Array get_merge_from_vert() const;
	void set_merge_from_vert(const godot::PackedInt64Array &p_merge_from_vert);
	godot::PackedInt64Array get_merge_to_vert() const;
	void set_merge_to_vert(const godot::PackedInt64Array &p_merge_to_vert);
	godot::PackedInt64Array get_run_index() const;
	void set_run_index(const godot::PackedInt64Array &p_run_index);
	godot::PackedInt32Array get_run_original_id() const;
	void set_run_original_id(const godot::PackedInt32Array &p_run_original_id);
	godot::PackedFloat64Array get_run_transform() const;
	void set_run_transform(const godot::PackedFloat64Array &p_run_transform);
	godot::PackedInt64Array get_face_id() const;
	void set_face_id(const godot::PackedInt64Array &p_face_id);
	godot::PackedFloat64Array get_halfedge_tangent() const;
	void set_halfedge_tangent(const godot::PackedFloat64Array &p_halfedge_tangent);
	double get_tolerance() const;
	void set_tolerance(double p_tolerance);

	bool merge();
	godot::Ref<Manifold> to_manifold() const;
	godot::Ref<Manifold> to_manifold_with_original_id(uint32_t p_original_id) const;

private:
	struct Inner;
	Inner *_inner;
};

class Manifold : public godot::RefCounted {
	GDCLASS(Manifold, godot::RefCounted);

protected:
	static void _bind_methods();

public:
	Manifold();
	Manifold(const manifold::Manifold &p_manifold);
	~Manifold();

	godot::Ref<ManifoldMesh32> to_mesh32(int p_normal_idx = -1) const;
	godot::Ref<ManifoldMesh64> to_mesh64(int p_normal_idx = -1) const;

	godot::TypedArray<Manifold> decompose() const;
	static godot::Ref<Manifold> tetrahedron();
	static godot::Ref<Manifold> cube(godot::Vector3 p_size = godot::Vector3(1.0f, 1.0f, 1.0f), bool p_center = false);
	static godot::Ref<Manifold> cylinder(double p_height, double p_radius_low, double p_radius_high = -1.0, int p_circular_segments = 0, bool p_center = false);
	static godot::Ref<Manifold> sphere(double p_radius, int p_circular_segments = 0);
	static godot::Ref<Manifold> level_set_bind(const godot::Callable &p_sdf, godot::AABB p_bounds, double p_edge_length, double p_level = 0, double p_tolerance = -1);
	static godot::Ref<Manifold> level_set(const std::function<double(godot::Vector3)> &p_sdf, godot::AABB p_bounds, double p_edge_length, double p_level = 0, double p_tolerance = -1);

	godot::TypedArray<godot::PackedVector2Array> slice(double p_height = 0) const;
	godot::TypedArray<godot::PackedVector2Array> project() const;
	static godot::Ref<Manifold> extrude(const godot::TypedArray<godot::PackedVector2Array> &p_cross_section, double p_height, int p_num_divisions = 0, double p_twist_degrees = 0.0, godot::Vector2 p_scale_top = godot::Vector2(1.0f, 1.0f));
	static godot::Ref<Manifold> revolve(const godot::TypedArray<godot::PackedVector2Array> &p_cross_section, int p_circular_segments = 0, double p_revolve_degrees = 360.0f);

	enum Error {
		NO_ERROR = 0,
		ERROR_NON_FINITE_VERTEX = 1,
		ERROR_NOT_MANIFOLD = 2,
		ERROR_VERTEX_OUT_OF_BOUNDS = 3,
		ERROR_PROPERTIES_WRONG_LENGTH = 4,
		ERROR_MISSING_POSITION_PROPERTIES = 5,
		ERROR_MERGE_VECTORS_DIFFERENT_LENGTHS = 6,
		ERROR_MERGE_INDEX_OUT_OF_BOUNDS = 7,
		ERROR_TRANSFORM_WRONG_LENGTH = 8,
		ERROR_RUN_INDEX_WRONG_LENGTH = 9,
		ERROR_FACE_ID_WRONG_LENGTH = 10,
		ERROR_INVALID_CONSTRUCTION = 11,
		ERROR_RESULT_TOO_LARGE = 12,
	};

	Error status() const;
	bool is_empty() const;
	int64_t num_vert() const;
	int64_t num_edge() const;
	int64_t num_tri() const;
	int64_t num_prop() const;
	int64_t num_prop_vert() const;
	godot::AABB bounding_box() const;
	int genus() const;
	double get_tolerance() const;

	double surface_area() const;
	double volume() const;
	double min_gap(const godot::Ref<Manifold> &p_other, double p_search_length) const;

	int original_id() const;
	godot::Ref<Manifold> as_original() const;
	godot::Ref<Manifold> merge_runs() const;
	static uint32_t reserve_ids(uint32_t p_count);

	godot::Ref<Manifold> translate(godot::Vector3 p_offset) const;
	godot::Ref<Manifold> scale(godot::Vector3 p_scale) const;
	godot::Ref<Manifold> rotate(double p_x_degrees, double p_y_degrees = 0.0, double p_z_degrees = 0.0) const;
	godot::Ref<Manifold> mirror(godot::Vector3 p_axis) const;
	godot::Ref<Manifold> transform(const godot::Transform3D &p_transform) const;
	godot::Ref<Manifold> warp(const std::function<godot::Vector3(godot::Vector3)> &p_func) const;
	godot::Ref<Manifold> warp_bind(const godot::Callable &p_func) const;
	godot::Ref<Manifold> set_tolerance(double p_tolerance) const;
	godot::Ref<Manifold> simplify(double p_tolerance = 0) const;

	godot::Ref<Manifold> union_with(const godot::Ref<Manifold> &p_second) const;
	static godot::Ref<Manifold> union_batch(const godot::TypedArray<Manifold> &p_manifolds);
	godot::Ref<Manifold> intersection_with(const godot::Ref<Manifold> &p_second) const;
	static godot::Ref<Manifold> intersection_batch(const godot::TypedArray<Manifold> &p_manifolds);
	godot::Ref<Manifold> difference_with(const godot::Ref<Manifold> &p_second) const;
	static godot::Ref<Manifold> difference_batch(const godot::TypedArray<Manifold> &p_manifolds);
	godot::Pair<godot::Ref<Manifold>, godot::Ref<Manifold>> split(const godot::Ref<Manifold> &p_manifold) const;
	godot::TypedArray<Manifold> split_bind(const godot::Ref<Manifold> &p_manifold) const;
	godot::Pair<godot::Ref<Manifold>, godot::Ref<Manifold>> split_by_plane(godot::Plane p_plane) const;
	godot::TypedArray<Manifold> split_by_plane_bind(godot::Plane p_plane) const;
	godot::Ref<Manifold> trim_by_plane(godot::Plane p_plane) const;

	godot::Ref<Manifold> set_properties(int p_num_prop, const std::function<godot::PackedFloat64Array(godot::Vector3, const godot::PackedFloat64Array &)> &p_prop_func) const;
	godot::Ref<Manifold> set_properties_bind(int p_num_prop, const godot::Callable &p_prop_func) const;
	godot::Ref<Manifold> calculate_curvature(int p_gaussian_idx, int p_mean_idx) const;
	godot::Ref<Manifold> calculate_normals(int p_normal_idx = 0, double p_min_sharp_angle = 52.5) const;

	godot::Ref<Manifold> refine(int p_splits) const;
	godot::Ref<Manifold> refine_to_length(double p_length) const;
	godot::Ref<Manifold> refine_to_tolerance(double p_tolerance) const;
	godot::Ref<Manifold> smooth_by_normals(int p_normal_idx) const;
	godot::Ref<Manifold> smooth_out(double p_min_sharp_angle = 52.5, double p_min_smoothness = 0) const;

	godot::Ref<Manifold> hull() const;
	static godot::Ref<Manifold> hull_batch(const godot::TypedArray<Manifold> &p_manifolds);
	static godot::Ref<Manifold> hull_points(const godot::PackedVector3Array &p_points);

private:
	struct Inner;
	Inner *_inner;
	friend struct Inner;
};
VARIANT_ENUM_CAST(Manifold::Error);

class ManifoldMesh : public godot::Mesh {
	GDCLASS(ManifoldMesh, godot::Mesh);

protected:
	static void _bind_methods();

public:
#ifdef REAL_T_IS_DOUBLE
	using I = uint64_t;
	using PackedIArray = godot::PackedInt64Array;
	using Precision = double;
#else
	using I = uint32_t;
	using PackedIArray = godot::PackedInt32Array;
	using Precision = float;
#endif
	static_assert(std::is_same_v<Precision, godot::real_t>);

	static uint32_t get_material_original_id(const godot::Ref<godot::Material> &p_material);

private:
	godot::PackedInt32Array _surface_formats;
	godot::PackedInt32Array _surface_original_ids;
	godot::TypedArray<godot::Material> _surface_materials;
	godot::PackedStringArray _surface_names;

public:
	ManifoldMesh();
	~ManifoldMesh();

	void set_surface_formats(const godot::PackedInt32Array &p_surface_formats);
	godot::PackedInt32Array get_surface_formats() const;
	void set_surface_original_ids(const godot::PackedInt32Array &p_surface_original_ids);
	godot::PackedInt32Array get_surface_original_ids() const;
	void set_surface_materials(const godot::TypedArray<godot::Material> &p_surface_materials);
	godot::TypedArray<godot::Material> get_surface_materials() const;
	void set_surface_names(const godot::PackedStringArray &p_surface_names);
	godot::PackedStringArray get_surface_names() const;

	bool is_valid() const;
	bool is_empty() const;
	void remove_unused_materials();

	void set_num_prop(I p_num_prop);
	I get_num_prop() const;

	void set_vert_properties(const godot::PackedRealArray &p_vert_properties);
	godot::PackedRealArray get_vert_properties() const;

	void set_tri_verts(const PackedIArray &p_tri_verts);
	PackedIArray get_tri_verts() const;

	void set_merge_from_vert(const PackedIArray &p_merge_from_vert);
	PackedIArray get_merge_from_vert() const;

	void set_merge_to_vert(const PackedIArray &p_merge_to_vert);
	PackedIArray get_merge_to_vert() const;

	void set_run_index(const PackedIArray &p_run_index);
	PackedIArray get_run_index() const;

	void set_run_original_id(const godot::PackedInt32Array &p_run_original_id);
	godot::PackedInt32Array get_run_original_id() const;

	void set_run_transform(const godot::PackedRealArray &p_run_transform);
	godot::PackedRealArray get_run_transform() const;

	void set_face_id(const PackedIArray &p_face_id);
	PackedIArray get_face_id() const;

	void set_halfedge_tangent(const godot::PackedRealArray &p_halfedge_tangent);
	godot::PackedRealArray get_halfedge_tangent() const;

	void set_tolerance(Precision p_tolerance);
	Precision get_tolerance() const;

	int32_t _get_surface_count() const override;
	int32_t _surface_get_array_len(int32_t p_index) const override;
	int32_t _surface_get_array_index_len(int32_t p_index) const override;
	godot::Array _surface_get_arrays(int32_t p_index) const override;
	godot::TypedArray<godot::Array> _surface_get_blend_shape_arrays(int32_t p_index) const override;
	godot::Dictionary _surface_get_lods(int32_t p_index) const override;
	uint32_t _surface_get_format(int32_t p_index) const override;
	uint32_t _surface_get_primitive_type(int32_t p_index) const override;
	void _surface_set_material(int32_t p_index, const godot::Ref<godot::Material> &p_material) override;
	godot::Ref<godot::Material> _surface_get_material(int32_t p_index) const override;
	int32_t _get_blend_shape_count() const override;
	godot::StringName _get_blend_shape_name(int32_t p_index) const override;
	void _set_blend_shape_name(int32_t p_index, const godot::StringName &p_name) override;
	godot::AABB _get_aabb() const override;
	godot::RID _get_rid() const override;

	static godot::Ref<ManifoldMesh> from_mesh(const godot::Ref<godot::Mesh> &p_mesh);
	godot::Ref<godot::ArrayMesh> to_mesh(bool p_generate_lods = true, bool p_create_shadow_mesh = true, const godot::TypedArray<godot::Material> &p_skip_material = {}) const;

	godot::TypedArray<ManifoldMesh> decompose() const;
	static godot::Ref<ManifoldMesh> tetrahedron(const godot::Ref<godot::Material> &p_material = nullptr);
	static godot::Ref<ManifoldMesh> cube(const godot::Vector3 &p_size = godot::Vector3(1.0f, 1.0f, 1.0f), bool p_center = false, const godot::Ref<godot::Material> &p_material = nullptr);
	static godot::Ref<ManifoldMesh> cylinder(double p_height, double p_radius_low, double p_radius_high = -1.0, int32_t p_circular_segments = 0, bool p_center = false, const godot::Ref<godot::Material> &p_material = nullptr);
	static godot::Ref<ManifoldMesh> sphere(double p_radius, int32_t p_circular_segments = 0, const godot::Ref<godot::Material> &p_material = nullptr);
	static godot::Ref<ManifoldMesh> level_set(const godot::Callable &p_sdf, const godot::AABB &p_bounds, double p_edge_length, double p_level = 0.0, double p_tolerance = -1.0, const godot::Ref<godot::Material> &p_material = nullptr);

	godot::TypedArray<godot::PackedVector2Array> slice(double p_height = 0.0) const;
	godot::TypedArray<godot::PackedVector2Array> project() const;
	static godot::Ref<ManifoldMesh> extrude(const godot::TypedArray<godot::PackedVector2Array> &p_cross_section, double p_height, int32_t p_divisions = 0, double p_twist_degrees = 0.0, const godot::Vector2 &p_scale_top = godot::Vector2(1.0f, 1.0f), const godot::Ref<godot::Material> &p_material = nullptr);
	static godot::Ref<ManifoldMesh> revolve(const godot::TypedArray<godot::PackedVector2Array> &p_cross_section, int32_t p_circular_segments = 0, double p_revolve_degrees = 360.0, const godot::Ref<godot::Material> &p_material = nullptr);

	uint64_t get_vertex_count() const;
	uint64_t get_edge_count() const;
	uint64_t get_triangle_count() const;
	uint64_t get_property_vertex_count() const;
	godot::AABB get_aabb() const;

	int32_t get_genus() const;
	double get_surface_area() const;
	double get_volume() const;
	double get_min_gap(const godot::Ref<ManifoldMesh> &p_other, double p_search_length) const;

	godot::Ref<ManifoldMesh> translate(const godot::Vector3 &p_offset) const;
	godot::Ref<ManifoldMesh> scale(const godot::Vector3 &p_scale) const;
	godot::Ref<ManifoldMesh> rotate(const godot::Vector3 &p_rotation_degrees) const;
	godot::Ref<ManifoldMesh> mirror(const godot::Vector3 &p_normal) const;
	godot::Ref<ManifoldMesh> transform(const godot::Transform3D &p_transform) const;
	godot::Ref<ManifoldMesh> warp(const godot::Callable &p_warp_vertex) const;
	godot::Ref<ManifoldMesh> warp_batch(const godot::Callable &p_warp_vertices) const;
	godot::Ref<ManifoldMesh> simplify(double p_tolerance = 0.0) const;

	godot::Ref<ManifoldMesh> hull() const;
	godot::Ref<ManifoldMesh> refine(int32_t p_subdivisions) const;
	godot::Ref<ManifoldMesh> refine_to_length(double p_length) const;
	godot::Ref<ManifoldMesh> refine_to_tolerance(double p_tolerance) const;

	godot::Ref<ManifoldMesh> union_with(const godot::Ref<ManifoldMesh> &p_with) const;
	godot::Ref<ManifoldMesh> intersection_with(const godot::Ref<ManifoldMesh> &p_with) const;
	godot::Ref<ManifoldMesh> difference_with(const godot::Ref<ManifoldMesh> &p_with) const;
	static godot::Ref<ManifoldMesh> batch_union(const godot::TypedArray<ManifoldMesh> &p_manifolds);
	static godot::Ref<ManifoldMesh> batch_intersection(const godot::TypedArray<ManifoldMesh> &p_manifolds);
	static godot::Ref<ManifoldMesh> batch_difference(const godot::TypedArray<ManifoldMesh> &p_manifolds);

	godot::Pair<godot::Ref<ManifoldMesh>, godot::Ref<ManifoldMesh>> split(const godot::Ref<ManifoldMesh> &p_manifold) const;
	godot::TypedArray<ManifoldMesh> split_bind(const godot::Ref<ManifoldMesh> &p_manifold) const;
	godot::Pair<godot::Ref<ManifoldMesh>, godot::Ref<ManifoldMesh>> split_by_plane(const godot::Plane &p_plane, const godot::Ref<godot::Material> &p_material = nullptr) const;
	godot::TypedArray<ManifoldMesh> split_by_plane_bind(const godot::Plane &p_plane, const godot::Ref<godot::Material> &p_material = nullptr) const;
	godot::Ref<ManifoldMesh> trim_by_plane(const godot::Plane &p_plane, const godot::Ref<godot::Material> &p_material = nullptr) const;

	godot::Ref<ManifoldMesh> modify_normal(const godot::Callable &p_modify, bool p_normalize = true) const;
	godot::Ref<ManifoldMesh> modify_tex_uv(const godot::Callable &p_modify) const;
	godot::Ref<ManifoldMesh> modify_tex_uv2(const godot::Callable &p_modify) const;
	godot::Ref<ManifoldMesh> modify_color(const godot::Callable &p_modify) const;
	godot::Ref<ManifoldMesh> modify_custom0(const godot::Callable &p_modify) const;
	godot::Ref<ManifoldMesh> modify_custom1(const godot::Callable &p_modify) const;
	godot::Ref<ManifoldMesh> modify_custom2(const godot::Callable &p_modify) const;
	godot::Ref<ManifoldMesh> modify_custom3(const godot::Callable &p_modify) const;

private:
	struct Inner;
	Inner *_inner;

	void _ensure_manifold() const;
	void _ensure_meshgl() const;
	void _reallocate_original_ids();

	void _commit_to_arrays() const;
	void _unpack_to_arrays(uint32_t original_id, godot::PackedVector3Array &positions, godot::PackedVector3Array &normals, godot::PackedVector2Array &tex_uv, godot::PackedVector2Array &tex_uv2, godot::PackedColorArray &colors, godot::PackedColorArray &custom0, godot::PackedColorArray &custom1, godot::PackedColorArray &custom2, godot::PackedColorArray &custom3) const;

	void _init_normals(const godot::Array &arrays, I vertex, I stride);
	void _init_tex_uv(const godot::Array &arrays, I vertex, I stride);
	void _init_tex_uv2(const godot::Array &arrays, I vertex, I stride);
	void _init_color(const godot::Array &arrays, I vertex, I stride);
	void _init_custom(const godot::Array &arrays, I vertex, I stride, I offset, ArrayType type, ArrayCustomFormat custom_format);

	static godot::Ref<ManifoldMesh> _primitive(const manifold::Manifold &new_manifold, const godot::Ref<godot::Material> &material, const godot::String &name);
	static godot::Ref<ManifoldMesh> _new_merged_manifold(const manifold::Manifold &new_manifold, const godot::Vector<godot::Ref<ManifoldMesh>> &originals);
	godot::Ref<ManifoldMesh> _new_manifold(const manifold::Manifold &new_manifold) const;

	godot::Ref<ManifoldMesh> _modify_color(const int32_t min_prop, const godot::Callable &p_modify) const;
	godot::Ref<ManifoldMesh> _halfspace(const godot::Plane &p_plane, const godot::Ref<godot::Material> &p_material) const;
};
