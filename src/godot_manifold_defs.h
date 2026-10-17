#pragma once

#include <gdextension_interface.h>

#include <functional>

#include <godot_cpp/classes/material.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/templates/local_vector.hpp>
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
	enum JoinType {
		SQUARE,
		ROUND,
		MITER,
		BEVEL,
	};

	CrossSection();
	CrossSection(const manifold::CrossSection &p_cross_section);
	~CrossSection() override;

	[[nodiscard]] static godot::Ref<CrossSection> from_simple_polygon(const godot::PackedVector2Array &p_simple_polygon);
	[[nodiscard]] static godot::Ref<CrossSection> from_polygons(const godot::TypedArray<godot::PackedVector2Array> &p_polygons);
	[[nodiscard]] static godot::Ref<CrossSection> from_even_odd(const godot::TypedArray<godot::PackedVector2Array> &p_polygons);
	[[nodiscard]] static godot::Ref<CrossSection> from_rect(const godot::Rect2 &p_rect);
	[[nodiscard]] godot::TypedArray<godot::PackedVector2Array> to_polygons() const;
	[[nodiscard]] godot::TypedArray<godot::PackedVector2Array> to_convex_polygons() const;
	[[nodiscard]] godot::PackedVector2Array to_triangles() const;
	[[nodiscard]] godot::PackedVector2Array to_triangles_with_vertices_from(const godot::TypedArray<CrossSection> &p_others) const;

	[[nodiscard]] godot::TypedArray<CrossSection> decompose() const;
	[[nodiscard]] static godot::Ref<CrossSection> square(const godot::Vector2 &p_dimensions, bool p_center = false);
	[[nodiscard]] static godot::Ref<CrossSection> circle(double p_radius, int32_t p_circular_segments = 0);

	[[nodiscard]] bool is_empty() const;
	[[nodiscard]] uint64_t get_vertex_count() const;
	[[nodiscard]] uint64_t get_contour_count() const;
	[[nodiscard]] godot::Rect2 get_bounds() const;
	[[nodiscard]] double get_area() const;

	[[nodiscard]] godot::Ref<CrossSection> translate(const godot::Vector2 &p_offset) const;
	[[nodiscard]] godot::Ref<CrossSection> rotate(double p_degrees) const;
	[[nodiscard]] godot::Ref<CrossSection> scale(const godot::Vector2 &p_scale) const;
	[[nodiscard]] godot::Ref<CrossSection> mirror(const godot::Vector2 &p_axis) const;
	[[nodiscard]] godot::Ref<CrossSection> transform(const godot::Transform2D &p_transform) const;
	[[nodiscard]] godot::Ref<CrossSection> warp(const godot::Callable &p_warp_vertex) const;
	[[nodiscard]] godot::Ref<CrossSection> warp_batch(const godot::Callable &p_warp_vertices) const;
	[[nodiscard]] godot::Ref<CrossSection> simplify(double p_epsilon = 1e-6) const;
	[[nodiscard]] godot::Ref<CrossSection> offset(double p_delta, JoinType p_join_type, double p_miter_limit = 2.0, int32_t p_circular_segments = 0) const;

	[[nodiscard]] godot::Ref<CrossSection> union_with(const godot::Ref<CrossSection> &p_with) const;
	[[nodiscard]] godot::Ref<CrossSection> intersection_with(const godot::Ref<CrossSection> &p_with) const;
	[[nodiscard]] godot::Ref<CrossSection> difference_with(const godot::Ref<CrossSection> &p_with) const;
	[[nodiscard]] static godot::Ref<CrossSection> batch_union(const godot::TypedArray<CrossSection> &p_cross_sections);
	[[nodiscard]] static godot::Ref<CrossSection> batch_intersection(const godot::TypedArray<CrossSection> &p_cross_sections);
	[[nodiscard]] static godot::Ref<CrossSection> batch_difference(const godot::TypedArray<CrossSection> &p_cross_sections);

	[[nodiscard]] godot::Ref<CrossSection> hull() const;
	[[nodiscard]] static godot::Ref<CrossSection> from_simple_hull(const godot::PackedVector2Array &p_simple_polygon);
	[[nodiscard]] static godot::Ref<CrossSection> from_complex_hull(const godot::TypedArray<godot::PackedVector2Array> &p_polygons);
	[[nodiscard]] static godot::Ref<CrossSection> from_combined_hull(const godot::TypedArray<CrossSection> &p_cross_sections);

private:
	struct Inner;
	Inner *_inner;
};
VARIANT_ENUM_CAST(CrossSection::JoinType);

class ManifoldMesh32 : public godot::Resource {
	GDCLASS(ManifoldMesh32, godot::Resource);

protected:
	static void _bind_methods();

public:
	ManifoldMesh32();
	ManifoldMesh32(const manifold::MeshGLP<float, uint32_t> &p_meshgl);
	~ManifoldMesh32() override;

	[[nodiscard]] uint32_t num_vert() const;
	[[nodiscard]] uint32_t num_tri() const;
	[[nodiscard]] uint32_t get_num_prop() const;
	void set_num_prop(uint32_t p_num_prop);
	[[nodiscard]] godot::PackedFloat32Array get_vert_properties() const;
	void set_vert_properties(const godot::PackedFloat32Array &p_vert_properties);
	[[nodiscard]] godot::PackedInt32Array get_tri_verts() const;
	void set_tri_verts(const godot::PackedInt32Array &p_tri_verts);
	[[nodiscard]] godot::PackedInt32Array get_merge_from_vert() const;
	void set_merge_from_vert(const godot::PackedInt32Array &p_merge_from_vert);
	[[nodiscard]] godot::PackedInt32Array get_merge_to_vert() const;
	void set_merge_to_vert(const godot::PackedInt32Array &p_merge_to_vert);
	[[nodiscard]] godot::PackedInt32Array get_run_index() const;
	void set_run_index(const godot::PackedInt32Array &p_run_index);
	[[nodiscard]] godot::PackedInt32Array get_run_original_id() const;
	void set_run_original_id(const godot::PackedInt32Array &p_run_original_id);
	[[nodiscard]] godot::PackedFloat32Array get_run_transform() const;
	void set_run_transform(const godot::PackedFloat32Array &p_run_transform);
	[[nodiscard]] godot::PackedInt32Array get_face_id() const;
	void set_face_id(const godot::PackedInt32Array &p_face_id);
	[[nodiscard]] godot::PackedFloat32Array get_halfedge_tangent() const;
	void set_halfedge_tangent(const godot::PackedFloat32Array &p_halfedge_tangent);
	[[nodiscard]] float get_tolerance() const;
	void set_tolerance(float p_tolerance);

	bool merge();
	[[nodiscard]] godot::Ref<Manifold> to_manifold() const;
	[[nodiscard]] godot::Ref<Manifold> to_manifold_with_original_id(uint32_t p_original_id) const;

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
	~ManifoldMesh64() override;

	[[nodiscard]] uint64_t num_vert() const;
	[[nodiscard]] uint64_t num_tri() const;
	[[nodiscard]] uint64_t get_num_prop() const;
	void set_num_prop(uint64_t p_num_prop);
	[[nodiscard]] godot::PackedFloat64Array get_vert_properties() const;
	void set_vert_properties(const godot::PackedFloat64Array &p_vert_properties);
	[[nodiscard]] godot::PackedInt64Array get_tri_verts() const;
	void set_tri_verts(const godot::PackedInt64Array &p_tri_verts);
	[[nodiscard]] godot::PackedInt64Array get_merge_from_vert() const;
	void set_merge_from_vert(const godot::PackedInt64Array &p_merge_from_vert);
	[[nodiscard]] godot::PackedInt64Array get_merge_to_vert() const;
	void set_merge_to_vert(const godot::PackedInt64Array &p_merge_to_vert);
	[[nodiscard]] godot::PackedInt64Array get_run_index() const;
	void set_run_index(const godot::PackedInt64Array &p_run_index);
	[[nodiscard]] godot::PackedInt32Array get_run_original_id() const;
	void set_run_original_id(const godot::PackedInt32Array &p_run_original_id);
	[[nodiscard]] godot::PackedFloat64Array get_run_transform() const;
	void set_run_transform(const godot::PackedFloat64Array &p_run_transform);
	[[nodiscard]] godot::PackedInt64Array get_face_id() const;
	void set_face_id(const godot::PackedInt64Array &p_face_id);
	[[nodiscard]] godot::PackedFloat64Array get_halfedge_tangent() const;
	void set_halfedge_tangent(const godot::PackedFloat64Array &p_halfedge_tangent);
	[[nodiscard]] double get_tolerance() const;
	void set_tolerance(double p_tolerance);

	bool merge();
	[[nodiscard]] godot::Ref<Manifold> to_manifold() const;
	[[nodiscard]] godot::Ref<Manifold> to_manifold_with_original_id(uint32_t p_original_id) const;

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
	~Manifold() override;

	[[nodiscard]] godot::Ref<ManifoldMesh32> to_mesh32(int p_normal_idx = -1) const;
	[[nodiscard]] godot::Ref<ManifoldMesh64> to_mesh64(int p_normal_idx = -1) const;

	[[nodiscard]] godot::TypedArray<Manifold> decompose() const;
	[[nodiscard]] static godot::Ref<Manifold> tetrahedron();
	[[nodiscard]] static godot::Ref<Manifold> cube(godot::Vector3 p_size = godot::Vector3(1.0f, 1.0f, 1.0f), bool p_center = false);
	[[nodiscard]] static godot::Ref<Manifold> cylinder(double p_height, double p_radius_low, double p_radius_high = -1.0, int p_circular_segments = 0, bool p_center = false);
	[[nodiscard]] static godot::Ref<Manifold> sphere(double p_radius, int p_circular_segments = 0);
	[[nodiscard]] static godot::Ref<Manifold> level_set_bind(const godot::Callable &p_sdf, godot::AABB p_bounds, double p_edge_length, double p_level = 0, double p_tolerance = -1);
	[[nodiscard]] static godot::Ref<Manifold> level_set(const std::function<double(godot::Vector3)> &p_sdf, godot::AABB p_bounds, double p_edge_length, double p_level = 0, double p_tolerance = -1);

	[[nodiscard]] godot::TypedArray<godot::PackedVector2Array> slice(double p_height = 0) const;
	[[nodiscard]] godot::TypedArray<godot::PackedVector2Array> project() const;
	[[nodiscard]] static godot::Ref<Manifold> extrude(const godot::TypedArray<godot::PackedVector2Array> &p_cross_section, double p_height, int p_num_divisions = 0, double p_twist_degrees = 0.0, godot::Vector2 p_scale_top = godot::Vector2(1.0f, 1.0f));
	[[nodiscard]] static godot::Ref<Manifold> revolve(const godot::TypedArray<godot::PackedVector2Array> &p_cross_section, int p_circular_segments = 0, double p_revolve_degrees = 360.0f);

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
		ERROR_INVALID_TANGENTS = 13,
		ERROR_CANCELLED = 14,
	};

	[[nodiscard]] Error status() const;
	[[nodiscard]] bool is_empty() const;
	[[nodiscard]] int64_t num_vert() const;
	[[nodiscard]] int64_t num_edge() const;
	[[nodiscard]] int64_t num_tri() const;
	[[nodiscard]] int64_t num_prop() const;
	[[nodiscard]] int64_t num_prop_vert() const;
	[[nodiscard]] godot::AABB bounding_box() const;
	[[nodiscard]] int genus() const;
	[[nodiscard]] double get_tolerance() const;

	[[nodiscard]] double surface_area() const;
	[[nodiscard]] double volume() const;
	[[nodiscard]] double min_gap(const godot::Ref<Manifold> &p_other, double p_search_length) const;

	struct RayHit {
		/// The triangle index that was hit.
		uint64_t face{};
		/// The parametric distance along the ray segment in the closed interval
		/// [0, 1], where 0 is the origin and 1 is the endpoint. Hits exactly at
		/// the origin or endpoint are included.
		godot::real_t distance{};
		/// The 3D position of the hit point.
		godot::Vector3 position{};
		/// The geometric face normal at the hit.
		godot::Vector3 normal{};
	};
	void ray_cast(godot::LocalVector<RayHit> &r_hits, godot::Vector3 p_origin, godot::Vector3 p_endpoint) const;
	[[nodiscard]] godot::TypedArray<godot::Dictionary> ray_cast_bind(godot::Vector3 p_origin, godot::Vector3 p_endpoint) const;
	[[nodiscard]] godot::PackedInt32Array winding_number(const godot::PackedVector3Array &p_points) const;

	[[nodiscard]] int original_id() const;
	[[nodiscard]] godot::Ref<Manifold> as_original(int p_original_id = -1) const;
	[[nodiscard]] godot::Ref<Manifold> merge_runs() const;
	[[nodiscard]] static uint32_t reserve_ids(uint32_t p_count);

	[[nodiscard]] godot::Ref<Manifold> translate(godot::Vector3 p_offset) const;
	[[nodiscard]] godot::Ref<Manifold> scale(godot::Vector3 p_scale) const;
	[[nodiscard]] godot::Ref<Manifold> rotate(double p_x_degrees, double p_y_degrees = 0.0, double p_z_degrees = 0.0) const;
	[[nodiscard]] godot::Ref<Manifold> mirror(godot::Vector3 p_axis) const;
	[[nodiscard]] godot::Ref<Manifold> transform(const godot::Transform3D &p_transform) const;
	[[nodiscard]] godot::Ref<Manifold> warp(const std::function<godot::Vector3(godot::Vector3)> &p_func) const;
	[[nodiscard]] godot::Ref<Manifold> warp_bind(const godot::Callable &p_func) const;
	[[nodiscard]] godot::Ref<Manifold> set_tolerance(double p_tolerance) const;
	[[nodiscard]] godot::Ref<Manifold> simplify(double p_tolerance = 0) const;
	[[nodiscard]] godot::Ref<Manifold> remove_degenerates() const;

	[[nodiscard]] godot::Ref<Manifold> union_with(const godot::Ref<Manifold> &p_second) const;
	[[nodiscard]] static godot::Ref<Manifold> union_batch(const godot::TypedArray<Manifold> &p_manifolds);
	[[nodiscard]] godot::Ref<Manifold> intersection_with(const godot::Ref<Manifold> &p_second) const;
	[[nodiscard]] static godot::Ref<Manifold> intersection_batch(const godot::TypedArray<Manifold> &p_manifolds);
	[[nodiscard]] godot::Ref<Manifold> difference_with(const godot::Ref<Manifold> &p_second) const;
	[[nodiscard]] static godot::Ref<Manifold> difference_batch(const godot::TypedArray<Manifold> &p_manifolds);
	[[nodiscard]] godot::Pair<godot::Ref<Manifold>, godot::Ref<Manifold>> split(const godot::Ref<Manifold> &p_manifold) const;
	[[nodiscard]] godot::TypedArray<Manifold> split_bind(const godot::Ref<Manifold> &p_manifold) const;
	[[nodiscard]] godot::Pair<godot::Ref<Manifold>, godot::Ref<Manifold>> split_by_plane(godot::Plane p_plane) const;
	[[nodiscard]] godot::TypedArray<Manifold> split_by_plane_bind(godot::Plane p_plane) const;
	[[nodiscard]] godot::Ref<Manifold> trim_by_plane(godot::Plane p_plane) const;
	[[nodiscard]] godot::Ref<Manifold> minkowski_sum(const godot::Ref<Manifold> &p_second) const;
	[[nodiscard]] godot::Ref<Manifold> minkowski_difference(const godot::Ref<Manifold> &p_second) const;

	[[nodiscard]] godot::Ref<Manifold> set_properties(int p_num_prop, const std::function<godot::PackedFloat64Array(godot::Vector3, const godot::PackedFloat64Array &)> &p_prop_func) const;
	[[nodiscard]] godot::Ref<Manifold> set_properties_bind(int p_num_prop, const godot::Callable &p_prop_func) const;
	[[nodiscard]] godot::Ref<Manifold> calculate_curvature(int p_gaussian_idx, int p_mean_idx) const;
	[[nodiscard]] godot::Ref<Manifold> calculate_normals(int p_normal_idx = 0, double p_min_sharp_angle = 52.5) const;

	[[nodiscard]] godot::Ref<Manifold> refine(int p_splits) const;
	[[nodiscard]] godot::Ref<Manifold> refine_to_length(double p_length) const;
	[[nodiscard]] godot::Ref<Manifold> refine_to_tolerance(double p_tolerance) const;
	[[nodiscard]] godot::Ref<Manifold> smooth_by_normals(int p_normal_idx) const;

	[[nodiscard]] godot::Ref<Manifold> hull() const;
	[[nodiscard]] static godot::Ref<Manifold> hull_batch(const godot::TypedArray<Manifold> &p_manifolds);
	[[nodiscard]] static godot::Ref<Manifold> hull_points(const godot::PackedVector3Array &p_points);

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

	[[nodiscard]] static uint32_t get_material_original_id(const godot::Ref<godot::Material> &p_material);

private:
	godot::PackedInt32Array _surface_formats;
	godot::PackedInt32Array _surface_original_ids;
	godot::TypedArray<godot::Material> _surface_materials;
	godot::PackedStringArray _surface_names;

public:
	ManifoldMesh();
	~ManifoldMesh() override;

	void set_surface_formats(const godot::PackedInt32Array &p_surface_formats);
	[[nodiscard]] godot::PackedInt32Array get_surface_formats() const;
	void set_surface_original_ids(const godot::PackedInt32Array &p_surface_original_ids);
	[[nodiscard]] godot::PackedInt32Array get_surface_original_ids() const;
	void set_surface_materials(const godot::TypedArray<godot::Material> &p_surface_materials);
	[[nodiscard]] godot::TypedArray<godot::Material> get_surface_materials() const;
	void set_surface_names(const godot::PackedStringArray &p_surface_names);
	[[nodiscard]] godot::PackedStringArray get_surface_names() const;

	[[nodiscard]] bool is_valid() const;
	[[nodiscard]] bool is_empty() const;
	void remove_unused_materials();

	void set_num_prop(I p_num_prop);
	[[nodiscard]] I get_num_prop() const;

	void set_vert_properties(const godot::PackedRealArray &p_vert_properties);
	[[nodiscard]] godot::PackedRealArray get_vert_properties() const;

	void set_tri_verts(const PackedIArray &p_tri_verts);
	[[nodiscard]] PackedIArray get_tri_verts() const;

	void set_merge_from_vert(const PackedIArray &p_merge_from_vert);
	[[nodiscard]] PackedIArray get_merge_from_vert() const;

	void set_merge_to_vert(const PackedIArray &p_merge_to_vert);
	[[nodiscard]] PackedIArray get_merge_to_vert() const;

	void set_run_index(const PackedIArray &p_run_index);
	[[nodiscard]] PackedIArray get_run_index() const;

	void set_run_original_id(const godot::PackedInt32Array &p_run_original_id);
	[[nodiscard]] godot::PackedInt32Array get_run_original_id() const;

	void set_run_transform(const godot::PackedRealArray &p_run_transform);
	[[nodiscard]] godot::PackedRealArray get_run_transform() const;

	void set_face_id(const PackedIArray &p_face_id);
	[[nodiscard]] PackedIArray get_face_id() const;

	void set_halfedge_tangent(const godot::PackedRealArray &p_halfedge_tangent);
	[[nodiscard]] godot::PackedRealArray get_halfedge_tangent() const;

	void set_tolerance(Precision p_tolerance);
	[[nodiscard]] Precision get_tolerance() const;

	[[nodiscard]] int32_t _get_surface_count() const override;
	[[nodiscard]] int32_t _surface_get_array_len(int32_t p_index) const override;
	[[nodiscard]] int32_t _surface_get_array_index_len(int32_t p_index) const override;
	[[nodiscard]] godot::Array _surface_get_arrays(int32_t p_index) const override;
	[[nodiscard]] godot::TypedArray<godot::Array> _surface_get_blend_shape_arrays(int32_t p_index) const override;
	[[nodiscard]] godot::Dictionary _surface_get_lods(int32_t p_index) const override;
	[[nodiscard]] uint32_t _surface_get_format(int32_t p_index) const override;
	[[nodiscard]] uint32_t _surface_get_primitive_type(int32_t p_index) const override;
	void _surface_set_material(int32_t p_index, const godot::Ref<godot::Material> &p_material) override;
	[[nodiscard]] godot::Ref<godot::Material> _surface_get_material(int32_t p_index) const override;
	[[nodiscard]] int32_t _get_blend_shape_count() const override;
	[[nodiscard]] godot::StringName _get_blend_shape_name(int32_t p_index) const override;
	void _set_blend_shape_name(int32_t p_index, const godot::StringName &p_name) override;
	[[nodiscard]] godot::AABB _get_aabb() const override;
	[[nodiscard]] godot::RID _get_rid() const override;

	[[nodiscard]] static godot::Ref<ManifoldMesh> from_mesh(const godot::Ref<godot::Mesh> &p_mesh);
	[[nodiscard]] godot::Ref<godot::ArrayMesh> to_mesh(bool p_generate_lods = true, bool p_create_shadow_mesh = true, const godot::TypedArray<godot::Material> &p_skip_material = {}) const;

	[[nodiscard]] godot::TypedArray<ManifoldMesh> decompose() const;
	[[nodiscard]] static godot::Ref<ManifoldMesh> tetrahedron(const godot::Ref<godot::Material> &p_material = nullptr);
	[[nodiscard]] static godot::Ref<ManifoldMesh> cube(const godot::Vector3 &p_size = godot::Vector3(1.0f, 1.0f, 1.0f), bool p_center = false, const godot::Ref<godot::Material> &p_material = nullptr);
	[[nodiscard]] static godot::Ref<ManifoldMesh> cylinder(double p_height, double p_radius_low, double p_radius_high = -1.0, int32_t p_circular_segments = 0, bool p_center = false, const godot::Ref<godot::Material> &p_material = nullptr);
	[[nodiscard]] static godot::Ref<ManifoldMesh> sphere(double p_radius, int32_t p_circular_segments = 0, const godot::Ref<godot::Material> &p_material = nullptr);
	[[nodiscard]] static godot::Ref<ManifoldMesh> level_set(const godot::Callable &p_sdf, const godot::AABB &p_bounds, double p_edge_length, double p_level = 0.0, double p_tolerance = -1.0, const godot::Ref<godot::Material> &p_material = nullptr);

	[[nodiscard]] godot::TypedArray<godot::PackedVector2Array> slice(double p_height = 0.0) const;
	[[nodiscard]] godot::TypedArray<godot::PackedVector2Array> project() const;
	[[nodiscard]] static godot::Ref<ManifoldMesh> extrude(const godot::TypedArray<godot::PackedVector2Array> &p_cross_section, double p_height, int32_t p_divisions = 0, double p_twist_degrees = 0.0, const godot::Vector2 &p_scale_top = godot::Vector2(1.0f, 1.0f), const godot::Ref<godot::Material> &p_material = nullptr);
	[[nodiscard]] static godot::Ref<ManifoldMesh> revolve(const godot::TypedArray<godot::PackedVector2Array> &p_cross_section, int32_t p_circular_segments = 0, double p_revolve_degrees = 360.0, const godot::Ref<godot::Material> &p_material = nullptr);

	[[nodiscard]] uint64_t get_vertex_count() const;
	[[nodiscard]] uint64_t get_edge_count() const;
	[[nodiscard]] uint64_t get_triangle_count() const;
	[[nodiscard]] uint64_t get_property_vertex_count() const;

	[[nodiscard]] int32_t get_genus() const;
	[[nodiscard]] double get_surface_area() const;
	[[nodiscard]] double get_volume() const;
	[[nodiscard]] double get_min_gap(const godot::Ref<ManifoldMesh> &p_other, double p_search_length) const;

	[[nodiscard]] godot::Ref<ManifoldMesh> translate(const godot::Vector3 &p_offset) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> scale(const godot::Vector3 &p_scale) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> rotate(const godot::Vector3 &p_rotation_degrees) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> mirror(const godot::Vector3 &p_normal) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> transform(const godot::Transform3D &p_transform) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> warp(const godot::Callable &p_warp_vertex) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> warp_batch(const godot::Callable &p_warp_vertices) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> simplify(double p_tolerance = 0.0) const;

	[[nodiscard]] godot::Ref<ManifoldMesh> hull() const;
	[[nodiscard]] godot::Ref<ManifoldMesh> refine(int32_t p_subdivisions) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> refine_to_length(double p_length) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> refine_to_tolerance(double p_tolerance) const;

	[[nodiscard]] godot::Ref<ManifoldMesh> union_with(const godot::Ref<ManifoldMesh> &p_with) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> intersection_with(const godot::Ref<ManifoldMesh> &p_with) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> difference_with(const godot::Ref<ManifoldMesh> &p_with) const;
	[[nodiscard]] static godot::Ref<ManifoldMesh> batch_union(const godot::TypedArray<ManifoldMesh> &p_manifolds);
	[[nodiscard]] static godot::Ref<ManifoldMesh> batch_intersection(const godot::TypedArray<ManifoldMesh> &p_manifolds);
	[[nodiscard]] static godot::Ref<ManifoldMesh> batch_difference(const godot::TypedArray<ManifoldMesh> &p_manifolds);

	[[nodiscard]] godot::Pair<godot::Ref<ManifoldMesh>, godot::Ref<ManifoldMesh>> split(const godot::Ref<ManifoldMesh> &p_manifold) const;
	[[nodiscard]] godot::TypedArray<ManifoldMesh> split_bind(const godot::Ref<ManifoldMesh> &p_manifold) const;
	[[nodiscard]] godot::Pair<godot::Ref<ManifoldMesh>, godot::Ref<ManifoldMesh>> split_by_plane(const godot::Plane &p_plane, const godot::Ref<godot::Material> &p_material = nullptr) const;
	[[nodiscard]] godot::TypedArray<ManifoldMesh> split_by_plane_bind(const godot::Plane &p_plane, const godot::Ref<godot::Material> &p_material = nullptr) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> trim_by_plane(const godot::Plane &p_plane, const godot::Ref<godot::Material> &p_material = nullptr) const;

	[[nodiscard]] godot::Ref<ManifoldMesh> modify_normal(const godot::Callable &p_modify, bool p_normalize = true) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> modify_tex_uv(const godot::Callable &p_modify) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> modify_tex_uv2(const godot::Callable &p_modify) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> modify_color(const godot::Callable &p_modify) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> modify_custom0(const godot::Callable &p_modify) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> modify_custom1(const godot::Callable &p_modify) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> modify_custom2(const godot::Callable &p_modify) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> modify_custom3(const godot::Callable &p_modify) const;

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

	[[nodiscard]] static godot::Ref<ManifoldMesh> _primitive(const manifold::Manifold &new_manifold, const godot::Ref<godot::Material> &material, const godot::String &name);
	[[nodiscard]] static godot::Ref<ManifoldMesh> _new_merged_manifold(const manifold::Manifold &new_manifold, const godot::Vector<godot::Ref<ManifoldMesh>> &originals);
	[[nodiscard]] godot::Ref<ManifoldMesh> _new_manifold(const manifold::Manifold &new_manifold) const;

	[[nodiscard]] godot::Ref<ManifoldMesh> _modify_color(int32_t min_prop, const godot::Callable &p_modify) const;
	[[nodiscard]] godot::Ref<ManifoldMesh> _halfspace(const godot::Plane &p_plane, const godot::Ref<godot::Material> &p_material) const;
};
