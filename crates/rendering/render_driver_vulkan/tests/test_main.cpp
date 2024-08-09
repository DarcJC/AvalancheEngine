//
// Created by DarcJC on 8/3/2024.
//
#include "render_device.h"
#include "vulkan_context.h"
#include "logger.h"
#include "execution/graph.h"
#include "execution/generator.h"
#include "execution/sync_coroutine.h"
#include "execution/async_coroutine.h"
#include "execution/executor.h"
#include <iostream>
#include <sstream>
#include <format>
#include <thread>
#include <chrono>

using namespace avalanche::core::execution;

struct TestNode : public Node<TestNode> {
public:
    explicit TestNode(const node_id_type id) : Node(id) {}
};

inline async_bool bar() {
    co_return true;
}

inline async_void foo(int i, const int n = 10) {
    if (i < n) {
        co_await foo(i + 2, n).set_executor(sync_coroutine_executor::get_global_executor());
    }
    AVALANCHE_LOGGER.critical("[thread-{}] foo-{}",std::this_thread::get_id(), i);
    co_return;
}

inline async_void noop() {
    co_return;
}

inline async_void loop_noop(int count) {
    std::cout << "loop_synchronously(" << count << ")" << std::endl;
    auto current_time = std::chrono::high_resolution_clock::now();
    for (auto i : range<int>(0, count)) {
        co_await noop();
    }
    std::cout << "loop_synchronously(" << count << ") returning" << std::endl;
    std::cout << (std::chrono::high_resolution_clock::now() - current_time) << std::endl;
}

int main(int argc, char* argv[]) {

    using namespace avalanche::rendering;

    DeviceSettings settings{};
    settings.required_features.debug = true;

    vulkan::Context context(settings);

    // TODO: Figure out why Graph deconstruction leads to crash in Release Mode
    // Graph<TestNode> graph{};
    // avalanche::shared_ptr<TestNode> root = graph.default_root_node();
    // avalanche::shared_ptr<TestNode> u = graph.new_node();
    // avalanche::shared_ptr<TestNode> v = graph.new_node();
    // graph.add_edge(root, u);
    // graph.add_edge(root, v);
    // AVALANCHE_LOGGER.log(avalanche::core::LogLevel::Info, "{}", graph.is_node_exist(u->node_id()));
    // graph.add_edge(u, v);

    decltype(foo(5, 10).launch()) state;
    bar().sync().launch()->then([&state] (bool b) mutable {
        if (b) {
            state = foo(5, 100).async().launch();
        }
    });

    auto qwq = loop_noop(1000000)
        .set_executor(sync_coroutine_executor::get_global_executor())
        .launch();

    std::this_thread::sleep_for(std::chrono::milliseconds(9999000));
    // threaded_coroutine_executor::get_global_executor().wait_for_all_jobs(0);
    // for (int i = 0; i < 10; i++) {
    //     foo(0, 10).launch();
    // }

    return 0;
}
