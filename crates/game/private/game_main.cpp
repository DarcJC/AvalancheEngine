
#include <engine.h>
#include <render_server.h>
#include <window_server.h>
#include <class.h>

using namespace avalanche;

int main(int argc, char* argv[]) {

    auto engine = IEngine::create_instance({
        .primary_render_device_settings = rendering::DeviceSettings {
            .required_features = {
                .display = true,
                .debug = true,
            },
        },
    });

    rendering::TestData data{};
    data.field1 = 123456;
    const Class* clazz = Class::for_name(class_name_v<rendering::TestData>);
    const Field* field = clazz->get_field("field1");
    Chimera value = field->get_value<rendering::TestData&>(data);
    AVALANCHE_LOGGER.info("{}", value.as<uint32_t>().value());

    auto* window = engine->create_window(window::WindowSettings {});

    ITickManager& ticker = ITickManager::get();
    while (ticker.tick_frame())
        ;

    return 0;
}
