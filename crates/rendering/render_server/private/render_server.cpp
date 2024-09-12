#include "render_server_internal.h"

#include <event/named_event.h>
#include <render_device.h>
#include <render_frame.h>
#include <unordered_set>

#include "vulkan_render_device.h"

namespace avalanche::rendering {

    void RenderServerImpl::tick(duration_type delta_time) {}

    void RenderServerImpl::on_startup() {
        m_primary_render_device = pick_and_initialize_render_device();
        IRenderServer::on_startup();
        // m_queue_frame_event_canceller = core::INamedEvent::get().subscribe("render_server_queue_frame", [this] (void* data) {
        //     AVALANCHE_CHECK(data != nullptr, "Invalid event data");
        //     const auto * event_data = static_cast<QueueFrameToRenderServerEventData*>(data);
        //     this->queue_frame_to_render(event_data->unique_invokee_id, event_data->object_been_render_on);
        // });
    }

    void RenderServerImpl::on_shutdown() {
        IRenderServer::on_shutdown();
        m_primary_render_device.reset();
    }

    void RenderServerImpl::queue_frame_to_render(uint64_t unique_invokee_id, CanRenderOnMixin &object_been_render_on) {
        FrameInFlight frame_in_flight = object_been_render_on.prepare_frame_context();
    }

    unique_ptr<IRenderDevice> RenderServerImpl::pick_and_initialize_render_device() const {
        // TODO: pick device based on settings
#if AVALANCHE_ENABLE_VULKAN
        return unique_ptr<IRenderDevice>(vulkan::RenderDevice::create_instance({}));
#endif

        AVALANCHE_CHECK_RUNTIME(false, "Can't not determine which GAPI to use");
    }

    IRenderServer* IRenderServer::get() { return core::ServerManager::get().get_server_checked<IRenderServer>(); }

    IRenderServer* IRenderServer::create() {
        return new RenderServerImpl();
    }

} // namespace avalanche::server
