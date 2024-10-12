#ifndef __CLEF__RENDERING_HXX__
#define __CLEF__RENDERING_HXX__

#include "include/core/SkCanvas.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkScalar.h"
#include "layout/layout_tree.hxx"
#include "rapidxml.hpp"
#include <cstdint>
#include <random>
#include <unordered_map>

namespace Clef {

struct RenderingContext {
	SkCanvas *canvas;
	std::unordered_map<uint64_t, LayoutBound> layout_map;
	sk_sp<SkFontMgr> font_mgr;
	std::uniform_int_distribution<std::mt19937::result_type> random_id;
};

RenderingContext new_rendering_context(SkCanvas *canvas,
									   sk_sp<SkFontMgr> font_mgr);

void render_document(RenderingContext &ctx,
					 const rapidxml::xml_document<> &doc);

void render_tree(RenderingContext &ctx, const LayoutTree &tree);

}; // namespace Clef

#endif
