#ifndef __CLEF_LAYOUT_LAYOUT_TREE_HXX__
#define __CLEF_LAYOUT_LAYOUT_TREE_HXX__

#include "include/core/SkColorSpace.h"
#include "include/core/SkFont.h"
#include "layout_bound.hxx"
#include "rapidxml.hpp"
#include "rendering_context.hxx"
#include <cstdint>
#include <string>
#include <vector>

namespace Clef {

struct LayoutTree {
	enum class NodeType { Text, Container };

	struct Node {
		uint64_t id;
		Node *parent;
		Node *prev_sibiling;
		Node *next_sibiling;
		NodeType type;

		static Node *from_xml_node(const RenderingContext &ctx,
								   rapidxml::xml_node<char> *node);

		virtual Clef::LayoutBound measure() = 0;
	};

	Node *root;

	static LayoutTree from_xml(const RenderingContext &ctx,
							   const rapidxml::xml_document<> &doc);

  private:
	LayoutTree(Node *root);
};

struct ContainerNode : LayoutTree::Node {
	std::vector<Node *> children;

	Clef::LayoutBound measure() override;

	ContainerNode();
};

struct TextNode : LayoutTree::Node {
	std::string content;
	SkFont font;

	Clef::LayoutBound measure() override;

	TextNode(const char *content, SkFont font);
};

} // namespace Clef

#endif
