#include "render_command.h"


namespace avalanche::rendering {

    CommandEncoderState CommandsMixin::get_encoder_state() const {
        return static_cast<CommandEncoderState>(m_current_state_.load(std::memory_order_acquire));
    }

    void CommandsMixin::set_encoder_state(const CommandEncoderState new_state) {
        m_current_state_.store(static_cast<uint8_t>(new_state), std::memory_order_release);
    }

    bool CommandsMixin::can_write() const {
        const CommandEncoderState current_state = get_encoder_state();
        return current_state == CommandEncoderState::Open;
    }

    unique_ptr<IComputePassEncoder> ICommandEncoder::begin_ray_tracing_pass(const ComputePassDesc &) {
        AVALANCHE_TODO("Ray tracing doesn't implemented");
        return nullptr;
    }
} // namespace avalanche::rendering
