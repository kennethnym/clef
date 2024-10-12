#include "GL/gl.h"
#include "GLFW/glfw3.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "layout/layout_tree.hxx"
#include "rapidxml.hpp"
#include "rendering.hxx"
#include "rendering_context.hxx"
#include <GLFW/glfw3.h>
#include <cstdlib>
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

const auto INITIAL_WINDOW_WIDTH = 800;
const auto INITIAL_WINDOW_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
	auto ctx =
		static_cast<Clef::RenderingContext *>(glfwGetWindowUserPointer(window));

	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	float xscale, yscale;
	glfwGetMonitorContentScale(mon, &xscale, &yscale);
	glViewport(0, 0, width, height);

	Clef::update_skia_surface(*ctx, width, height);
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

	window = glfwCreateWindow(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT,
							  "Clef", nullptr, nullptr);
	if (!window) {
		std::cout << "window cannot be created" << std::endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);
	glfwWindowHint(GLFW_SCALE_FRAMEBUFFER, GLFW_TRUE);
	glfwSwapInterval(1);
	glfwMakeContextCurrent(window);

	float xscale, yscale;
	GLFWmonitor *mon = glfwGetPrimaryMonitor();
	glfwGetMonitorContentScale(mon, &xscale, &yscale);

	Clef::RenderingContext rendering_ctx = Clef::new_rendering_context(
		INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);

	glfwSetWindowUserPointer(window, (void *)&rendering_ctx);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	rapidxml::file<> input_file(argv[1]);
	rapidxml::xml_document<> doc;
	doc.parse<rapidxml::parse_default>(input_file.data());
	Clef::LayoutTree tree = Clef::LayoutTree::from_xml(rendering_ctx, doc);

	while (!glfwWindowShouldClose(window)) {
		Clef::render_tree(rendering_ctx, tree);

		rendering_ctx.gr_direct_context->flush();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
