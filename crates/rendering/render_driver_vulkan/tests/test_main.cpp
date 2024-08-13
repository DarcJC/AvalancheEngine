//
// Created by DarcJC on 8/3/2024.
//
#include <vulkan_render_device.h>

#include "container/shared_ptr.hpp"
#include "container/unique_ptr.hpp"
#include "execution/async_coroutine.h"
#include "execution/graph.h"
#include "logger.h"
#include "manager/server_manager.h"
#include "manager/tick_manager.h"
#include "render_device.h"
#include "vulkan_context.h"
#include "window_server.h"

struct TestNode : avalanche::core::execution::Node<TestNode> {
    explicit TestNode(const node_id_type id) : Node(id) {}

    int usage = 0;
};

using namespace avalanche::core;
using namespace avalanche::core::execution;

inline async<void> foo() {
    // std::this_thread::sleep_for(std::chrono::milliseconds(500));
    AVALANCHE_LOGGER.info("foo()");
    co_return;
}

struct Bar : public TickableCRTPBase<Bar> {

    void tick(ITickable::duration_type delta_time) override {
        AVALANCHE_LOGGER.info("Delta Time: {}ms", delta_time);
    }

};

int main(int argc, char* argv[]) {
    {
        launch(foo());
    }

    {
        using namespace avalanche::core::execution;
//
//        Graph<TestNode> graph;
//        auto u = graph.new_node();
//        auto v = graph.new_node();
//        auto root = graph.default_root_node();
//        graph.add_edge(root, u);
//        graph.add_edge(root, v);
//        graph.add_edge(u, v);
    }

    {
        using namespace avalanche::rendering;

        DeviceSettings settings{};
        settings.required_features.debug = true;
        settings.required_features.display = true;

        auto render_device = avalanche::unique_ptr<IRenderDevice>(vulkan::RenderDevice::create_instance(settings));
        auto* window_server = avalanche::core::ServerManager::get().get_server_checked<avalanche::window::IWindowServer>();
        avalanche::window::IWindow* window = window_server->create_window({});
    }

    Bar bar{};

    ITickManager& ticker = ITickManager::get();
    while (ticker.tick_frame())
        ;

    return 0;
}
