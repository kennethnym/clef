#include "rendering_context.hxx"
#include "GLFW/glfw3.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/gl/GrGLAssembleInterface.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/gpu/ganesh/gl/GrGLDirectContext.h"
#include "include/ports/SkFontMgr_directory.h"
#include <iostream>

Clef::RenderingContext Clef::new_rendering_context(int width, int height) {
	Clef::RenderingContext ctx{
		.framebuffer_info{0, GL_RGBA8},
		.font_mgr = SkFontMgr_New_Custom_Directory("../resources/font/Inter"),
	};

	sk_sp<const GrGLInterface> interface = GrGLMakeNativeInterface();
	if (!interface) {
		interface = GrGLMakeAssembledInterface(
			nullptr, (GrGLGetProc) * [](void *, const char *p) -> void * {
				return (void *)glfwGetProcAddress(p);
			});
		if (!interface) {
			std::cout << "failed to make native interface" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	ctx.gl_interface = interface;
	ctx.gr_direct_context = GrDirectContexts::MakeGL(interface);

	Clef::update_skia_surface(ctx, width, height);

	return ctx;
}

void Clef::update_skia_surface(Clef::RenderingContext &ctx, int width,
							   int height) {
	ctx.render_target = GrBackendRenderTargets::MakeGL(width, height, 0, 0,
													   ctx.framebuffer_info);
	ctx.sk_surface = SkSurfaces::WrapBackendRenderTarget(
		ctx.gr_direct_context.get(), ctx.render_target,
		kBottomLeft_GrSurfaceOrigin, kRGBA_8888_SkColorType,
		SkColorSpace::MakeSRGB(), nullptr);
	std::cout << "sk surface" << ctx.sk_surface << std::endl;
	ctx.canvas = ctx.sk_surface->getCanvas();
	std::cout << " kasdjkjsak" << ctx.sk_surface->getCanvas() << std::endl;
	std::cout << "new canvas address " << ctx.canvas << std::endl;
}
