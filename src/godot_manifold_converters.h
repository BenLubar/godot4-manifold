#pragma once

#include <gdextension_interface.h>
#include <manifold/common.h>

#include <algorithm>

static _FORCE_INLINE_ godot::Vector2 from_vec2(const manifold::vec2 &p_vec) {
	return {static_cast<godot::real_t>(p_vec.x), static_cast<godot::real_t>(p_vec.y)};
}
static _FORCE_INLINE_ godot::Vector3 from_vec3(const manifold::vec3 &p_vec) {
	return {static_cast<godot::real_t>(p_vec.x), static_cast<godot::real_t>(p_vec.y), static_cast<godot::real_t>(p_vec.z)};
}
static _FORCE_INLINE_ manifold::vec2 to_vec2(const godot::Vector2 &p_vec) {
	return {p_vec.x, p_vec.y};
}
static _FORCE_INLINE_ manifold::vec3 to_vec3(const godot::Vector3 &p_vec) {
	return {p_vec.x, p_vec.y, p_vec.z};
}
static _FORCE_INLINE_ godot::Rect2 from_rect(const manifold::Rect &p_rect) {
	return {from_vec2(p_rect.min), from_vec2(p_rect.Size())};
}
static _FORCE_INLINE_ manifold::Rect to_rect(const godot::Rect2 &p_rect) {
	return {to_vec2(p_rect.position), to_vec2(p_rect.get_end())};
}
static _FORCE_INLINE_ godot::AABB from_box(const manifold::Box &p_box) {
	return {from_vec3(p_box.min), from_vec3(p_box.Size())};
}
static _FORCE_INLINE_ manifold::Box to_box(const godot::AABB &p_box) {
	return {to_vec3(p_box.position), to_vec3(p_box.get_end())};
}
static _FORCE_INLINE_ manifold::mat2x3 to_mat2x3(const godot::Transform2D &p_transform) {
	return {to_vec2(p_transform.columns[0]), to_vec2(p_transform.columns[1]), to_vec2(p_transform.columns[2])};
}
static _FORCE_INLINE_ manifold::mat3 to_mat3(const godot::Basis &p_basis) {
	return {
		{
			static_cast<godot::real_t>(p_basis.rows[0].x),
			static_cast<godot::real_t>(p_basis.rows[1].x),
			static_cast<godot::real_t>(p_basis.rows[2].x),
		},
		{
			static_cast<godot::real_t>(p_basis.rows[0].y),
			static_cast<godot::real_t>(p_basis.rows[1].y),
			static_cast<godot::real_t>(p_basis.rows[2].y),
		},
		{
			static_cast<godot::real_t>(p_basis.rows[0].z),
			static_cast<godot::real_t>(p_basis.rows[1].z),
			static_cast<godot::real_t>(p_basis.rows[2].z),
		},
	};
}
static _FORCE_INLINE_ manifold::mat3x4 to_mat3x4(const godot::Transform3D &p_transform) {
	return {to_mat3(p_transform.basis), to_vec3(p_transform.origin)};
}
static _FORCE_INLINE_ godot::PackedVector2Array from_simple_polygon(const manifold::SimplePolygon &p_polygon) {
	godot::PackedVector2Array polygon;
	polygon.resize(p_polygon.size());
	std::transform(p_polygon.crbegin(), p_polygon.crend(), polygon.begin(), &from_vec2);
	return polygon;
}
static _FORCE_INLINE_ godot::TypedArray<godot::PackedVector2Array> from_polygons(const manifold::Polygons &p_polygons) {
	godot::TypedArray<godot::PackedVector2Array> polygons;
	polygons.resize(p_polygons.size());
	std::transform(p_polygons.cbegin(), p_polygons.cend(), polygons.begin(), &from_simple_polygon);
	return polygons;
}
static _FORCE_INLINE_ manifold::SimplePolygon to_simple_polygon(const godot::PackedVector2Array &p_polygon) {
	manifold::SimplePolygon polygon;
	polygon.resize(p_polygon.size());
	std::transform(p_polygon.begin(), p_polygon.end(), polygon.rbegin(), &to_vec2);
	return polygon;
}
static _FORCE_INLINE_ manifold::Polygons to_polygons(const godot::TypedArray<godot::PackedVector2Array> &p_polygons) {
	manifold::Polygons polygons;
	polygons.resize(p_polygons.size());
	std::transform(p_polygons.begin(), p_polygons.end(), polygons.begin(), &to_simple_polygon);
	return polygons;
}
