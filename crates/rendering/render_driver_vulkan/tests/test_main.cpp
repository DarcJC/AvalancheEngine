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
#include <sstream>
#include <thread>

using namespace avalanche::core::execution;

struct TestNode : public Node<TestNode> {
public:
    explicit TestNode(const node_id_type id) : Node(id) {}
};

coroutine_context<> foo(int i, const int n = 10) {
    if (i < n) {
        co_await foo(i + 2, n);
    }
    std::stringstream ss;
    ss << "[thread-" << std::this_thread::get_id() << "] " << "foo-" << i << std::endl;
    std::cout << ss.str();
    co_return;
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

    sync_coroutine_context::start(foo(0, 10));

    return 0;
}
