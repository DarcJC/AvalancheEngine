#include "render_server.h"

#include <render_frame.h>

namespace avalanche::rendering {

    RenderServer *RenderServer::get() { return core::ServerManager::get().get_server_checked<RenderServer>(); }

    void RenderServer::tick(duration_type delta_time) { AVALANCHE_LOGGER.info("delta_time: {}", delta_time); }

    void RenderServer::queue_frame_to_render(uint64_t unique_invokee_id, CanRenderOnMixin& object_been_render_on) {
        FrameInFlight frame_in_flight = object_been_render_on.prepare_frame_context();
    }

} // namespace avalanche::server

REGISTER_SERVER(avalanche::rendering::RenderServer);
