#include "render_server.h"

#include <event/named_event.h>
#include <render_device.h>
#include <render_frame.h>
#include <unordered_set>

#include "vulkan_render_device.h"

namespace avalanche::rendering {

    class RenderServerImpl : public IRenderServer {
    public:
        //  Begin ITickable interface
        void tick(duration_type delta_time) override;
        void on_startup() override;
        void on_shutdown() override;
        //  End ITickable interface

        // Begin IRenderServer interface
        void queue_frame_to_render(uint64_t unique_invokee_id, CanRenderOnMixin& object_been_render_on) override;
        // End IRenderServer interface

    protected:
        AVALANCHE_NO_DISCARD unique_ptr<IRenderDevice> pick_and_initialize_render_device() const {
            // TODO: pick device based on settings
#if AVALANCHE_ENABLE_VULKAN
            return unique_ptr<IRenderDevice>(vulkan::RenderDevice::create_instance({}));
#endif

            AVALANCHE_CHECK_RUNTIME(false, "Can't not determine which GAPI to use");
        }

    private:
        unique_ptr<IRenderDevice> m_primary_render_device;
        core::INamedEvent::CancellerType m_queue_frame_event_canceller;
    };

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

    void RenderServerImpl::queue_frame_to_render(uint64_t unique_invokee_id, CanRenderOnMixin& object_been_render_on) {
        FrameInFlight frame_in_flight = object_been_render_on.prepare_frame_context();
    }

    IRenderServer *IRenderServer::get() { return core::ServerManager::get().get_server_checked<IRenderServer>(); }

    unique_ptr<IRenderServer> IRenderServer::create() {
        return { make_unique<RenderServerImpl>() };
    }

} // namespace avalanche::server
