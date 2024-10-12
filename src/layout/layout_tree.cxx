#include "layout_tree.hxx"
#include "include/core/SkFont.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkFontTypes.h"
#include "include/core/SkRect.h"
#include "include/core/SkRefCnt.h"
#include "include/core/SkTypeface.h"
#include "rapidxml.hpp"
#include "rendering.hxx"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>

static std::uniform_int_distribution<std::mt19937::result_type>
	random_id(0, std::numeric_limits<std::mt19937::result_type>::max());
static std::random_device dev;
static std::mt19937 rng(dev());

uint64_t __generate_random_node_id() { return random_id(rng); };

Clef::ContainerNode::ContainerNode() {
	prev_sibiling = nullptr;
	next_sibiling = nullptr;
	parent = nullptr;
	type = Clef::LayoutTree::NodeType::Container;
}

Clef::LayoutBound Clef::ContainerNode::measure() {
	float max_child_width = -1;
	float total_height = 0;
	for (const auto &child : children) {
		const auto bound = child->measure();
		if (bound.width > max_child_width) {
			max_child_width = bound.width;
		}
		total_height += bound.height;
	}
	return {0, 0, max_child_width, total_height};
}

Clef::TextNode::TextNode(const char *content, SkFont font)
	: content(content), font(font) {
	prev_sibiling = nullptr;
	next_sibiling = nullptr;
	parent = nullptr;
	type = Clef::LayoutTree::NodeType::Text;
}

Clef::LayoutBound Clef::TextNode::measure() {
	SkRect rect;
	font.measureText(content.c_str(), content.size(), SkTextEncoding::kUTF8,
					 &rect);
	return {0, 0, rect.width(), rect.height()};
}

Clef::LayoutTree::LayoutTree(Clef::LayoutTree::Node *root) : root(root) {}

Clef::LayoutTree::Node *
Clef::LayoutTree::Node::from_xml_node(const RenderingContext &ctx,
									  rapidxml::xml_node<char> *node) {
	std::cout << "encountered node:" << node->name() << std::endl;

	if (std::strncmp("box", node->name(), 3) == 0) {
		auto container_node = new ContainerNode();
		container_node->id = __generate_random_node_id();

		std::cout << "assigned id " << container_node->id << std::endl;
		auto current_node = node->first_node();
		Node *last_node = nullptr;

		while (current_node) {
			auto n = Node::from_xml_node(ctx, current_node);
			if (!n) {
				return nullptr;
			}

			if (last_node) {
				n->prev_sibiling = last_node;
				last_node->next_sibiling = n;
				std::cout << n << std::endl;
				std::cout << last_node << std::endl;
			}

			n->parent = container_node;
			container_node->children.emplace_back(n);

			last_node = n;
			current_node = current_node->next_sibling();
		}

		return container_node;
	}

	if (std::strncmp("text", node->name(), 4) == 0) {
		const auto content = node->value();
		if (!content) {
			return nullptr;
		}

		auto typeface =
			ctx.font_mgr->matchFamilyStyle("Inter", SkFontStyle::Normal());
		SkFont f(typeface, 20);

		auto n = new TextNode(content, f);
		n->id = __generate_random_node_id();

		std::cout << "assigned id " << n->id << std::endl;

		return n;
	}

	return nullptr;
}

Clef::LayoutTree
Clef::LayoutTree::from_xml(const RenderingContext &ctx,
						   const rapidxml::xml_document<> &doc) {
	Clef::LayoutTree::Node *root;

	auto xml_root = doc.first_node("box");
	if (!xml_root) {
		throw std::runtime_error(
			"cml layout must start with a container node.");
	}

	root = Node::from_xml_node(ctx, xml_root);
	if (!root) {
		throw std::runtime_error("failed to parse cml");
	}

	std::cout << "cml parsed successfully" << std::endl;

	return root;
}
