#include "render_command.h"


namespace avalanche::rendering {

    CommandEncoderMixin::CommandEncoderMixin() {
        new (&m_storage_) std::atomic<uint8_t>{static_cast<uint8_t>(CommandEncoderState::Open)};
    }

    CommandEncoderMixin::~CommandEncoderMixin() {
        get_atomic_value()->~atomic();
    }

    CommandEncoderState CommandEncoderMixin::get_encoder_state() const {
        return static_cast<CommandEncoderState>(get_atomic_value()->load(std::memory_order_acquire));
    }

    void CommandEncoderMixin::set_encoder_state(const CommandEncoderState new_state) {
        get_atomic_value()->store(static_cast<uint8_t>(new_state), std::memory_order_release);
    }

    const std::atomic<uint8_t> *CommandEncoderMixin::get_atomic_value() const {
        return reinterpret_cast<const std::atomic<uint8_t> *>(&m_storage_);
    }

    std::atomic<uint8_t> *CommandEncoderMixin::get_atomic_value() {
        return reinterpret_cast<std::atomic<uint8_t> *>(&m_storage_);
    }

    bool CommandEncoderMixin::can_write() const {
        const CommandEncoderState current_state = get_encoder_state();
        return current_state == CommandEncoderState::Open;
    }

    unique_ptr<IComputePassEncoder> ICommandEncoder::begin_ray_tracing_pass(const ComputePassDesc &) {
        AVALANCHE_TODO("Ray tracing doesn't implemented");
        return nullptr;
    }
} // namespace avalanche::rendering
