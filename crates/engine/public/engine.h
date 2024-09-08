#pragma once
#include <container/unique_ptr.hpp>
#include <core.h>


namespace avalanche {

    namespace window {
        struct WindowSettings;
        class IWindow;
    }

    using namespace avalanche::core;

    struct EngineDesc {
    };

    class IEngine {
    public:
        AVALANCHE_NO_DISCARD static unique_ptr<IEngine> create_instance(const EngineDesc& desc);

        virtual ~IEngine();

        virtual void poll() = 0;

        virtual window::IWindow* create_window(const window::WindowSettings& settings) = 0;
    };

} // namespace avalanche
