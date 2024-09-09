#pragma once

#include "render_server.h"
#include "event/named_event.h"
#include "render_device.h"

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
        AVALANCHE_NO_DISCARD unique_ptr<IRenderDevice> pick_and_initialize_render_device() const;

    private:
        unique_ptr<IRenderDevice> m_primary_render_device;
        core::INamedEvent::CancellerType m_queue_frame_event_canceller;
    };

} // namespace avalanche::rendering
