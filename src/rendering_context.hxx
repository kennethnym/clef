#ifndef __CLEF_RENDERING_CONTEXT_HXX__
#define __CLEF_RENDERING_CONTEXT_HXX__

#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkFontMgr.h"
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/gl/GrGLTypes.h"
#include "layout/layout_bound.hxx"
#include <random>

namespace Clef {

struct RenderingContext {
	GrGLFramebufferInfo framebuffer_info;
	GrBackendRenderTarget render_target;
	sk_sp<const GrGLInterface> gl_interface;
	sk_sp<GrDirectContext> gr_direct_context;
	sk_sp<SkSurface> sk_surface;
	SkCanvas *canvas;
	std::unordered_map<uint64_t, LayoutBound> layout_map;
	sk_sp<SkFontMgr> font_mgr;
	std::uniform_int_distribution<std::mt19937::result_type> random_id;
};

RenderingContext new_rendering_context(int width, int height);

void update_skia_surface(RenderingContext &ctx, int width, int height);

} // namespace Clef

#endif
