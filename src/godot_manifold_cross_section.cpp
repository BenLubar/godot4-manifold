#include "godot_manifold_defs.h"
#include "godot_manifold_converters.h"

#include <godot_cpp/core/class_db.hpp>

#include <godot_cpp/classes/geometry2d.hpp>

#include <manifold/cross_section.h>

#include <polypartition.h>

using namespace godot;

void CrossSection::_bind_methods() {
	static_assert(EVEN_ODD == int(manifold::CrossSection::FillRule::EvenOdd));
	BIND_ENUM_CONSTANT(EVEN_ODD);
	static_assert(NON_ZERO == int(manifold::CrossSection::FillRule::NonZero));
	BIND_ENUM_CONSTANT(NON_ZERO);
	static_assert(POSITIVE == int(manifold::CrossSection::FillRule::Positive));
	BIND_ENUM_CONSTANT(POSITIVE);
	static_assert(NEGATIVE == int(manifold::CrossSection::FillRule::Negative));
	BIND_ENUM_CONSTANT(NEGATIVE);

	static_assert(SQUARE == int(manifold::CrossSection::JoinType::Square));
	BIND_ENUM_CONSTANT(SQUARE);
	static_assert(ROUND == int(manifold::CrossSection::JoinType::Round));
	BIND_ENUM_CONSTANT(ROUND);
	static_assert(MITER == int(manifold::CrossSection::JoinType::Miter));
	BIND_ENUM_CONSTANT(MITER);

	ClassDB::bind_static_method(get_class_static(), D_METHOD("from_simple_polygon", "simple_polygon", "fill_rule"), &CrossSection::from_simple_polygon, DEFVAL(POSITIVE));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("from_polygons", "polygons", "fill_rule"), &CrossSection::from_polygons, DEFVAL(POSITIVE));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("from_rect", "rect"), &CrossSection::from_rect);
	ClassDB::bind_method(D_METHOD("to_polygons"), &CrossSection::to_polygons);
	ClassDB::bind_method(D_METHOD("to_convex_polygons"), &CrossSection::to_convex_polygons);

	ClassDB::bind_method(D_METHOD("decompose"), &CrossSection::decompose);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("compose", "cross_sections"), &CrossSection::compose);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("square", "dimensions", "center"), &CrossSection::square, DEFVAL(false));
	ClassDB::bind_static_method(get_class_static(), D_METHOD("circle", "radius", "circular_segments"), &CrossSection::circle, DEFVAL(0));

	ClassDB::bind_method(D_METHOD("is_empty"), &CrossSection::is_empty);
	ClassDB::bind_method(D_METHOD("get_vertex_count"), &CrossSection::get_vertex_count);
	ClassDB::bind_method(D_METHOD("get_contour_count"), &CrossSection::get_contour_count);
	ClassDB::bind_method(D_METHOD("get_bounds"), &CrossSection::get_bounds);
	ClassDB::bind_method(D_METHOD("get_area"), &CrossSection::get_area);

	ClassDB::bind_method(D_METHOD("translate", "offset"), &CrossSection::translate);
	ClassDB::bind_method(D_METHOD("rotate", "degrees"), &CrossSection::rotate);
	ClassDB::bind_method(D_METHOD("scale", "scale"), &CrossSection::scale);
	ClassDB::bind_method(D_METHOD("mirror", "axis"), &CrossSection::mirror);
	ClassDB::bind_method(D_METHOD("transform", "transform"), &CrossSection::transform);
	ClassDB::bind_method(D_METHOD("warp", "warp_vertex"), &CrossSection::warp);
	ClassDB::bind_method(D_METHOD("warp_batch", "warp_vertices"), &CrossSection::warp_batch);
	ClassDB::bind_method(D_METHOD("simplify", "epsilon"), &CrossSection::simplify, DEFVAL(1e-6));
	ClassDB::bind_method(D_METHOD("offset", "delta", "join_type", "miter_limit", "circular_segments"), &CrossSection::offset, DEFVAL(2.0), DEFVAL(0));

	ClassDB::bind_method(D_METHOD("union", "with"), &CrossSection::union_with);
	ClassDB::bind_method(D_METHOD("intersection", "with"), &CrossSection::intersection_with);
	ClassDB::bind_method(D_METHOD("difference", "with"), &CrossSection::difference_with);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("batch_union", "cross_sections"), &CrossSection::batch_union);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("batch_intersection", "cross_sections"), &CrossSection::batch_intersection);
	ClassDB::bind_static_method(get_class_static(), D_METHOD("batch_difference", "cross_sections"), &CrossSection::batch_difference);

	ClassDB::bind_method(D_METHOD("hull"), &CrossSection::hull);
}

struct CrossSection::Inner {
	manifold::CrossSection _cross_section;
};

CrossSection::CrossSection() {
	_inner = memnew(Inner);
}
CrossSection::CrossSection(const manifold::CrossSection &p_cross_section) {
	_inner = memnew(Inner);
	_inner->_cross_section = p_cross_section;
}
CrossSection::~CrossSection() {
	memdelete(_inner);
	_inner = nullptr;
}

Ref<CrossSection> CrossSection::from_simple_polygon(const PackedVector2Array &p_simple_polygon, FillRule p_fill_rule) {
	return memnew(CrossSection(manifold::CrossSection(to_simple_polygon(p_simple_polygon), manifold::CrossSection::FillRule(p_fill_rule))));
}
Ref<CrossSection> CrossSection::from_polygons(const TypedArray<PackedVector2Array> &p_polygons, FillRule p_fill_rule) {
	return memnew(CrossSection(manifold::CrossSection(::to_polygons(p_polygons), manifold::CrossSection::FillRule(p_fill_rule))));
}
Ref<CrossSection> CrossSection::from_rect(const Rect2 &p_rect) {
	return memnew(CrossSection(manifold::CrossSection(to_rect(p_rect))));
}
TypedArray<PackedVector2Array> CrossSection::to_polygons() const {
	return ::from_polygons(_inner->_cross_section.ToPolygons());
}

TypedArray<CrossSection> CrossSection::decompose() const {
	const std::vector<manifold::CrossSection> sections = _inner->_cross_section.Decompose();
	TypedArray<CrossSection> wrapped_sections;
	wrapped_sections.resize(sections.size());
	for (size_t i = 0; i < sections.size(); i++) {
		wrapped_sections[i] = Ref<CrossSection>(memnew(CrossSection(sections[i])));
	}
	return wrapped_sections;
}

// This will eventually be available in Godot directly once https://github.com/godotengine/godot/pull/104407 is merged, but why wait?
TypedArray<PackedVector2Array> CrossSection::to_convex_polygons() const {
	const TypedArray<PackedVector2Array> polygons_and_holes = to_polygons();

	TypedArray<PackedVector2Array> decomp;
	TPPLPolyList in_poly, out_poly;

	for (int64_t i = 0; i < polygons_and_holes.size(); i++) {
		const PackedVector2Array polygon_or_hole = polygons_and_holes[i];

		TPPLPoly inp;
		inp.Init(polygon_or_hole.size());
		for (int64_t j = 0; j < polygon_or_hole.size(); j++) {
			inp[j].x = polygon_or_hole[j].x;
			inp[j].y = polygon_or_hole[j].y;
		}

		if (Geometry2D::get_singleton()->is_polygon_clockwise(polygon_or_hole)) {
			inp.SetOrientation(TPPL_ORIENTATION_CW);
			inp.SetHole(true);
		} else {
			inp.SetOrientation(TPPL_ORIENTATION_CCW);
		}

		DEV_ASSERT(inp.Valid());

		in_poly.push_back(inp);
	}

	TPPLPartition tpart;
	if (tpart.ConvexPartition_HM(&in_poly, &out_poly) == 0) { // Failed.
		ERR_PRINT("Convex decomposing failed!");
		return decomp;
	}

	decomp.resize(out_poly.size());
	int idx = 0;
	for (const TPPLPoly &tp : out_poly) {
		PackedVector2Array dp;
		dp.resize(tp.GetNumPoints());

		for (int64_t i = 0; i < tp.GetNumPoints(); i++) {
			dp[i].x = static_cast<real_t>(tp.GetPoint(i).x);
			dp[i].y = static_cast<real_t>(tp.GetPoint(i).y);
		}

		decomp[idx] = dp;

		idx++;
	}

	return decomp;
}

Ref<CrossSection> CrossSection::compose(const TypedArray<CrossSection> &p_cross_sections) {
	std::vector<manifold::CrossSection> sections;
	sections.resize(p_cross_sections.size());
	for (size_t i = 0; i < sections.size(); i++) {
		const Ref<CrossSection> section = p_cross_sections[i];
		ERR_FAIL_COND_V(section.is_null(), nullptr);
		sections[i] = section->_inner->_cross_section;
	}
	return memnew(CrossSection(manifold::CrossSection::Compose(sections)));
}
Ref<CrossSection> CrossSection::square(const Vector2 &p_dimensions, bool p_center) {
	return memnew(CrossSection(manifold::CrossSection::Square(to_vec2(p_dimensions), p_center)));
}
Ref<CrossSection> CrossSection::circle(double p_radius, int32_t p_circular_segments) {
	return memnew(CrossSection(manifold::CrossSection::Circle(p_radius, p_circular_segments)));
}

bool CrossSection::is_empty() const {
	return _inner->_cross_section.IsEmpty();
}
size_t CrossSection::get_vertex_count() const {
	return _inner->_cross_section.NumVert();
}
size_t CrossSection::get_contour_count() const {
	return _inner->_cross_section.NumContour();
}
Rect2 CrossSection::get_bounds() const {
	return ::from_rect(_inner->_cross_section.Bounds());
}
double CrossSection::get_area() const {
	return _inner->_cross_section.Area();
}

Ref<CrossSection> CrossSection::translate(const Vector2 &p_offset) const {
	return memnew(CrossSection(_inner->_cross_section.Translate(to_vec2(p_offset))));
}
Ref<CrossSection> CrossSection::rotate(double p_degrees) const {
	return memnew(CrossSection(_inner->_cross_section.Rotate(p_degrees)));
}
Ref<CrossSection> CrossSection::scale(const Vector2 &p_scale) const {
	return memnew(CrossSection(_inner->_cross_section.Scale(to_vec2(p_scale))));
}
Ref<CrossSection> CrossSection::mirror(const Vector2 &p_axis) const {
	return memnew(CrossSection(_inner->_cross_section.Mirror(to_vec2(p_axis))));
}
Ref<CrossSection> CrossSection::transform(const Transform2D &p_transform) const {
	return memnew(CrossSection(_inner->_cross_section.Transform(to_mat2x3(p_transform))));
}
Ref<CrossSection> CrossSection::warp(const Callable &p_warp_vertex) const {
	const auto warp_func = [p_warp_vertex](manifold::vec2 &p_vec) -> void {
		p_vec = to_vec2(p_warp_vertex.call(from_vec2(p_vec)));
	};
	return memnew(CrossSection(_inner->_cross_section.Warp(warp_func)));
}
Ref<CrossSection> CrossSection::warp_batch(const Callable &p_warp_vertices) const {
	const auto warp_func = [p_warp_vertices](manifold::VecView<manifold::vec2> p_view) -> void {
		PackedVector2Array view;
		view.resize(p_view.size());
		std::transform(p_view.begin(), p_view.end(), view.ptrw(), &from_vec2);

		const PackedVector2Array warped = p_warp_vertices.call(view);
		ERR_FAIL_COND_MSG(warped.size() != int64_t(p_view.size()), "The Callable given to warp_batch should modify the PackedVector2Array argument in-place and return it");

		std::transform(warped.begin(), warped.end(), p_view.begin(), &to_vec2);
	};
	return memnew(CrossSection(_inner->_cross_section.WarpBatch(warp_func)));
}
Ref<CrossSection> CrossSection::simplify(double p_epsilon) const {
	return memnew(CrossSection(_inner->_cross_section.Simplify(p_epsilon)));
}
Ref<CrossSection> CrossSection::offset(double p_delta, JoinType p_join_type, double p_miter_limit, int32_t p_circular_segments) const {
	return memnew(CrossSection(_inner->_cross_section.Offset(p_delta, manifold::CrossSection::JoinType(p_join_type), p_miter_limit, p_circular_segments)));
}

Ref<CrossSection> CrossSection::union_with(const Ref<CrossSection> &p_with) const {
	ERR_FAIL_COND_V(p_with.is_null(), nullptr);
	return memnew(CrossSection(_inner->_cross_section + p_with->_inner->_cross_section));
}
Ref<CrossSection> CrossSection::intersection_with(const Ref<CrossSection> &p_with) const {
	ERR_FAIL_COND_V(p_with.is_null(), nullptr);
	return memnew(CrossSection(_inner->_cross_section ^ p_with->_inner->_cross_section));
}
Ref<CrossSection> CrossSection::difference_with(const Ref<CrossSection> &p_with) const {
	ERR_FAIL_COND_V(p_with.is_null(), nullptr);
	return memnew(CrossSection(_inner->_cross_section - p_with->_inner->_cross_section));
}
Ref<CrossSection> CrossSection::batch_union(const TypedArray<CrossSection> &p_cross_sections) {
	std::vector<manifold::CrossSection> sections;
	sections.resize(p_cross_sections.size());
	for (size_t i = 0; i < sections.size(); i++) {
		const Ref<CrossSection> section = p_cross_sections[i];
		ERR_FAIL_COND_V(section.is_null(), nullptr);
		sections[i] = section->_inner->_cross_section;
	}
	return memnew(CrossSection(manifold::CrossSection::BatchBoolean(sections, manifold::OpType::Add)));
}
Ref<CrossSection> CrossSection::batch_intersection(const TypedArray<CrossSection> &p_cross_sections) {
	std::vector<manifold::CrossSection> sections;
	sections.resize(p_cross_sections.size());
	for (size_t i = 0; i < sections.size(); i++) {
		const Ref<CrossSection> section = p_cross_sections[i];
		ERR_FAIL_COND_V(section.is_null(), nullptr);
		sections[i] = section->_inner->_cross_section;
	}
	return memnew(CrossSection(manifold::CrossSection::BatchBoolean(sections, manifold::OpType::Intersect)));
}
Ref<CrossSection> CrossSection::batch_difference(const TypedArray<CrossSection> &p_cross_sections) {
	std::vector<manifold::CrossSection> sections;
	sections.resize(p_cross_sections.size());
	for (size_t i = 0; i < sections.size(); i++) {
		const Ref<CrossSection> section = p_cross_sections[i];
		ERR_FAIL_COND_V(section.is_null(), nullptr);
		sections[i] = section->_inner->_cross_section;
	}
	return memnew(CrossSection(manifold::CrossSection::BatchBoolean(sections, manifold::OpType::Subtract)));
}

Ref<CrossSection> CrossSection::hull() const {
	return memnew(CrossSection(_inner->_cross_section.Hull()));
}
