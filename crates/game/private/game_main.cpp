
#include <engine.h>
#include <window_server.h>

using namespace avalanche;

int main(int argc, char* argv[]) {
    auto* value = Class::for_name("avalanche::rendering::IRenderServer");

    auto engine = IEngine::create_instance({});
    auto* window = engine->create_window(window::WindowSettings {});

    ITickManager& ticker = ITickManager::get();
    while (ticker.tick_frame())
        ;

    return 0;
}
