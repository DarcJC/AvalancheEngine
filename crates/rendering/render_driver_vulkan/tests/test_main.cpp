//
// Created by DarcJC on 8/3/2024.
//
#include <vulkan_render_device.h>


#include "container/shared_ptr.hpp"
#include "container/unique_ptr.hpp"
#include "execution/graph.h"
#include "logger.h"
#include "render_device.h"
#include "vulkan_context.h"

struct TestNode : avalanche::core::execution::Node<TestNode> {
    explicit TestNode(const node_id_type id) : Node(id) {}

    int usage = 0;
};

int main(int argc, char* argv[]) {
    {
        using namespace avalanche::core::execution;

        Graph<TestNode> graph;
        auto u = graph.new_node();
        auto v = graph.new_node();
        auto root = graph.default_root_node();
        graph.add_edge(root, u);
        graph.add_edge(root, v);
        graph.add_edge(u, v);
    }

    {
        using namespace avalanche::rendering;

        DeviceSettings settings{};
        settings.required_features.debug = true;
        settings.required_features.display = true;

        auto render_device = avalanche::unique_ptr<IRenderDevice>(vulkan::RenderDevice::create_instance(settings));
    }

    return 0;
}
