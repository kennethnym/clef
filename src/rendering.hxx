#ifndef __CLEF__RENDERING_HXX__
#define __CLEF__RENDERING_HXX__

#include "layout/layout_tree.hxx"
#include "rapidxml.hpp"

namespace Clef {

void render_document(RenderingContext &ctx,
					 const rapidxml::xml_document<> &doc);

void render_tree(RenderingContext &ctx, const LayoutTree &tree);

}; // namespace Clef

#endif
