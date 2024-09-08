#pragma once
#include <manager/server_manager.h>
#include <manager/tick_manager.h>
#include <resource.h>

namespace avalanche::rendering {
    struct CanRenderOnMixin;
    using core::handle_t;

    class IRenderServer : public core::ServerCRTPTickable<IRenderServer, core::TickGroup::CollectRenderingResource> {
    public:
        AVALANCHE_NO_DISCARD static IRenderServer* get();

        virtual void queue_frame_to_render(uint64_t unique_invokee_id, CanRenderOnMixin& object_been_render_on) = 0;
    };

    struct QueueFrameToRenderServerEventData {
        uint64_t unique_invokee_id;
        CanRenderOnMixin& object_been_render_on;
    };

} // namespace avalanche::server

EXTERN_REGISTER_SERVER(RenderServer);
