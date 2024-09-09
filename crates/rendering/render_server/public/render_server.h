#pragma once
#include "avalanche_render_server_export.h"
#include "manager/server_manager.h"
#include "manager/tick_manager.h"
#include "resource.h"
#include "annotate.h"

namespace avalanche::rendering {
    struct CanRenderOnMixin;
    using core::handle_t;

    /// @brief RenderServer provide a high level interface of the render hardware
    /// @avalanche::begin
    /// [server]
    /// engine = true
    /// @avalanche::end
    class AVALANCHE_RENDER_SERVER_API IRenderServer : public core::ServerCRTPTickable<IRenderServer, core::TickGroup::CollectRenderingResource> {
    public:
        AVALANCHE_NO_DISCARD static IRenderServer* get();
        AVALANCHE_NO_DISCARD static unique_ptr<IRenderServer> create();

        virtual void queue_frame_to_render(uint64_t unique_invokee_id, CanRenderOnMixin& object_been_render_on) = 0;
    };

    struct QueueFrameToRenderServerEventData {
        uint64_t unique_invokee_id;
        CanRenderOnMixin& object_been_render_on;
    };

} // namespace avalanche::server