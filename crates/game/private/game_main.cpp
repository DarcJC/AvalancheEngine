
#include <engine.h>
#include <render_server.h>
#include <window_server.h>

using namespace avalanche;

int main(int argc, char* argv[]) {

    auto engine = IEngine::create_instance({});

    rendering::TestData data{};
    data.field1 = 123456;
    const Class* clazz = Class::for_name(class_name_v<rendering::TestData>);
    int32_t num;
    const Field *const * fields;
    clazz->fields(num, fields);
    Chimera value = fields[0]->get_value<rendering::TestData&>(data);
    AVALANCHE_LOGGER.info("{}", *static_cast<uint32_t*>(value.memory()));

    auto* window = engine->create_window(window::WindowSettings {});

    ITickManager& ticker = ITickManager::get();
    while (ticker.tick_frame())
        ;

    return 0;
}
