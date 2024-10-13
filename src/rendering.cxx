#include "rendering.hxx"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkFont.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/ports/SkFontMgr_directory.h"
#include "layout/layout.hxx"
#include "layout/layout_tree.hxx"
#include <utility>

Clef::LayoutBound __calculate_node_position(Clef::RenderingContext &ctx,
											Clef::LayoutTree::Node *node) {
	auto bounds = node->measure();
	if (!node->parent) {
		return {0, 0, bounds.width, bounds.height};
	}

	if (node->prev_sibiling != nullptr) {
		const auto entry = ctx.layout_map.find(node->prev_sibiling->id);
		if (entry == ctx.layout_map.end()) {
			return {0, 0, bounds.width, bounds.height};
		}

		const auto parent = node->parent;
		const auto prev_bound = entry->second;
		if (parent->type == Clef::LayoutTree::NodeType::Box) {
			const auto parent_box = static_cast<Clef::BoxNode *>(parent);
			switch (parent_box->orientation) {
			case Clef::Orientation::Vertical:
				return {prev_bound.x, prev_bound.y + prev_bound.height,
						bounds.width, bounds.height};
			case Clef::Orientation::Horizontal:
				return {prev_bound.x + prev_bound.width, prev_bound.y,
						bounds.width, bounds.height};
			}
		}

		return {prev_bound.x + prev_bound.width, prev_bound.y, bounds.width,
				bounds.height};
	}

	const auto entry = ctx.layout_map.find(node->parent->id);
	if (entry == ctx.layout_map.end()) {
		return {0, 0, bounds.width, bounds.height};
	}

	const auto parent_bound = entry->second;
	return {parent_bound.x, parent_bound.y, bounds.width, bounds.height};
}

void clef_render_node(Clef::RenderingContext &ctx,
					  Clef::LayoutTree::Node *node) {

	switch (node->type) {
	case Clef::LayoutTree::NodeType::Text: {
		const auto pos = __calculate_node_position(ctx, node);
		auto tn = static_cast<Clef::TextNode *>(node);
		SkPaint paint;
		paint.setColor(SK_ColorBLACK);
		ctx.canvas->drawString(tn->content.c_str(), pos.x, pos.y + pos.height,
							   tn->font, paint);
		ctx.layout_map.insert({node->id, pos});
		break;
	}

	case Clef::LayoutTree::NodeType::Box: {
		const auto box = static_cast<Clef::BoxNode *>(node);
		const auto pos = __calculate_node_position(ctx, node);
		SkRect rect{pos.x, pos.y, pos.x + pos.width, pos.y + pos.height};
		SkPaint paint;
		paint.setColor(SK_ColorCYAN);
		ctx.canvas->drawRect(rect, paint);
		ctx.layout_map.insert({node->id, pos});

		for (const auto &child : box->children) {
			clef_render_node(ctx, child);
		}
		break;
	}
	}
}

void Clef::render_tree(Clef::RenderingContext &ctx, const LayoutTree &tree) {
	SkPaint paint;
	paint.setColor(SK_ColorWHITE);
	ctx.canvas->drawPaint(paint);

	paint.setColor(SK_ColorBLACK);
	auto tf = ctx.font_mgr->matchFamilyStyle("Inter", SkFontStyle::Normal());
	SkFont f(tf, 20);
	ctx.canvas->drawString("hello", 0, 10, f, paint);

	clef_render_node(ctx, tree.root);
}
