
#include <engine.h>
#include <window_server.h>

#include "../../rendering/render_server/public/render_server.h"

using namespace avalanche;

int main(int argc, char* argv[]) {

    auto engine = IEngine::create_instance({});
    AVALANCHE_LOGGER.info("{}", ServerManager::get().get_server_checked<rendering::IRenderServer>()->get_server_id());
    auto* window = engine->create_window(window::WindowSettings {});

    ITickManager& ticker = ITickManager::get();
    while (ticker.tick_frame())
        ;

    return 0;
}
