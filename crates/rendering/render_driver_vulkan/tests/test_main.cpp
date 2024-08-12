//
// Created by DarcJC on 8/3/2024.
//
#include "render_device.h"
#include "vulkan_context.h"
#include "logger.h"
#include "execution/graph.h"
#include "container/shared_ptr.hpp"

struct TestNode : avalanche::core::execution::Node<TestNode> {
    explicit TestNode(const node_id_type id) : Node(id) {}

    int usage = 0;
};

int main(int argc, char* argv[]) {
    using namespace avalanche::core::execution;

    Graph<TestNode> graph;
    auto u = graph.new_node();
    auto v = graph.new_node();
    auto root = graph.default_root_node();
    graph.add_edge(root, u);
    graph.add_edge(root, v);
    graph.add_edge(u, v);

    return 0;
}
