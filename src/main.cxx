#include "GL/gl.h"
#include "GLFW/glfw3.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkColorType.h"
#include "include/core/SkFontStyle.h"
#include "include/core/SkPaint.h"
#include "include/core/SkTypeface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/GrTypes.h"
#include "include/gpu/ganesh/gl/GrGLTypes.h"
#include "layout/layout_tree.hxx"
#include "rapidxml.hpp"
#include "rendering.hxx"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <fstream>
#include <include/core/SkColorSpace.h>
#include <include/core/SkFont.h>
#include <include/core/SkFontMgr.h>
#include <include/core/SkStream.h>
#include <include/gpu/ganesh/GrBackendSurface.h>
#include <include/gpu/ganesh/SkSurfaceGanesh.h>
#include <include/gpu/ganesh/gl/GrGLAssembleInterface.h>
#include <include/gpu/ganesh/gl/GrGLBackendSurface.h>
#include <include/gpu/ganesh/gl/GrGLDirectContext.h>
#include <include/gpu/ganesh/gl/GrGLInterface.h>
#include <include/ports/SkFontMgr_directory.h>
#include <include/utils/SkCustomTypeface.h>
#include <iostream>
#include <rapidxml_utils.hpp>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	float xscale, yscale;
	glfwGetMonitorContentScale(mon, &xscale, &yscale);
	glViewport(0, 0, width, height);
}

int main(int argc, char *argv[]) {
	if (argc <= 0) {
		std::cerr << "expected a markup file" << std::endl;
		exit(EXIT_FAILURE);
	}

	GLFWwindow *window;

	if (!glfwInit()) {
		std::cout << "failed to initialize glfw" << std::endl;
		return -1;
	}

	window = glfwCreateWindow(200, 200, "Clef", nullptr, nullptr);
	if (!window) {
		std::cout << "window cannot be created" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwMakeContextCurrent(window);

	auto interface = GrGLMakeNativeInterface();
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

	auto context = GrDirectContexts::MakeGL(interface);
	if (!context) {
		std::cout << "failed to make DirectContext" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "gl direct context created" << std::endl;

	GrGLFramebufferInfo framebufferInfo;
	framebufferInfo.fFBOID = 0;
	framebufferInfo.fFormat = GL_RGBA8;

	std::cout << "frame buffer info created" << std::endl;

	SkColorType colorType = kRGBA_8888_SkColorType;
	auto target =
		GrBackendRenderTargets::MakeGL(400, 400, 0, 0, framebufferInfo);

	auto skSurface = SkSurfaces::WrapBackendRenderTarget(
		context.get(), target, kBottomLeft_GrSurfaceOrigin, colorType, nullptr,
		nullptr);
	if (!skSurface) {
		std::cout << "failed to create skia surface" << std::endl;
		exit(EXIT_FAILURE);
	}

	std::cout << "skia surface created" << std::endl;

	SkCanvas *canvas = skSurface->getCanvas();
	if (!canvas) {
		std::cout << "failed to obtain skia canvas" << std::endl;
		exit(EXIT_FAILURE);
	}

	glfwSwapInterval(1);

	std::cout << "canvas obtained. entering run loop..." << std::endl;

	auto fontMgr = SkFontMgr_New_Custom_Directory("../resources/font/Inter");
	std::cout << "fonts found: " << fontMgr->countFamilies() << std::endl;

	auto interRegular =
		fontMgr->matchFamilyStyle("Inter", SkFontStyle::Normal());
	if (!interRegular) {
		std::cout << "WARNING: inter regular not found" << std::endl;
	}

	rapidxml::file<> input_file(argv[1]);
	rapidxml::xml_document<> doc;
	doc.parse<rapidxml::parse_default>(input_file.data());

	Clef::RenderingContext ctx{
		.canvas = canvas,
		.font_mgr = fontMgr,
	};

	Clef::LayoutTree tree = Clef::LayoutTree::from_xml(doc, *fontMgr.get());

	while (!glfwWindowShouldClose(window)) {
		Clef::render_tree(ctx, tree);

		context->flush();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
