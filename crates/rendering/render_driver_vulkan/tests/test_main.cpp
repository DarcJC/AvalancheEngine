//
// Created by DarcJC on 8/3/2024.
//
#include "render_device.h"
#include "vulkan_context.h"
#include "logger.h"
#include "execution/graph.h"
#include "execution/generator.h"
#include "execution/coroutine.h"
#include "execution/executor.h"
#include <iostream>

using namespace avalanche::core::execution;

struct TestNode : public Node<TestNode> {
public:
    explicit TestNode(const node_id_type id) : Node(id) {}
};

inline async_void foo(const size_t i) {
    if (i < 2000) {
        co_await foo(i + 1);
    }
    std::cout << "foo-" << i << std::endl;
}

int main(int argc, char* argv[]) {

    using namespace avalanche::rendering;

    DeviceSettings settings{};
    settings.required_features.debug = true;

    vulkan::Context context(settings);

    Graph<TestNode> graph{};
    avalanche::shared_ptr<TestNode> root = graph.default_root_node();
    avalanche::shared_ptr<TestNode> u = graph.new_node();
    avalanche::shared_ptr<TestNode> v = graph.new_node();
    graph.add_edge(root, u);
    graph.add_edge(root, v);
    AVALANCHE_LOGGER.log(avalanche::core::LogLevel::Info, "{}", graph.is_node_exist(u->node_id()));
    graph.add_edge(u, v);

    launch(foo(0));

    return 0;
}
