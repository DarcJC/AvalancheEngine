#pragma once

#include "render_resource.h"
#include "render_enums.h"
#include <atomic>


namespace avalanche::rendering {
    struct RenderPassDesc;
    struct ComputePassDesc;

    /// @brief A mixin that keeping encoder state.
    /// @note The operations provided here is thread safe.
    class CommandsMixin {
    public:
        /// @brief Get current state of the encoder
        AVALANCHE_NO_DISCARD CommandEncoderState get_encoder_state() const;

        /// @brief Check if encoder can be written to now
        AVALANCHE_NO_DISCARD bool can_write() const;

    protected:
        /// @brief Set current state of the encoder
        void set_encoder_state(CommandEncoderState new_state);

    private:
        std::atomic<uint8_t> m_current_state_ = static_cast<uint8_t>(CommandEncoderState::Open);
    };

    class BindingCommandsMixin {
    public:
        virtual ~BindingCommandsMixin() = default;

        /// @brief Set the bind group at the given index of a root signature (pipeline layout in Vulkan).
        /// @note Bind group also known as DescriptorSet in Vulkan.
        /// @param index The index of bind group to set in the root signature.
        /// @param bind_group_to_use Bind group that set to.
        /// @param dynamic_offsets Sizes of dynamic buffer offsets
        virtual void set_bind_group(uint32_t index, handle_t bind_group_to_use, const vector<size_t>& dynamic_offsets) = 0;
    };

    /// @brief Providing an operation set of rendering
    class RenderCommandsMixin {
    public:
        virtual ~RenderCommandsMixin() = default;

        /// @brief Set pipeline object to use in the next operations
        /// @param render_pipeline The handle of pipeline state object of GAPI currently using.
        virtual void set_pipeline(handle_t render_pipeline) = 0;

        /// TODO
        virtual void set_index_buffer(handle_t buffer) = 0;

        /// @brief Perform a draw with vertex buffer and current pipeline state
        /// @param vertex_count Vertex count to draw
        /// @param instance_count Instance count to draw. Use @code 1@endcode as non-instanced draw.
        /// @param first_vertex Start index of the vertex buffer. Bind it in bind group.
        /// @param first_instance Start index of the instance data buffer. Bind it in bind group.
        virtual void draw(size_t vertex_count, size_t instance_count, size_t first_vertex, size_t first_instance) = 0;

        /// @brief Perform a draw with an index buffer.
        /// @param index_count Index count to draw
        /// @param instance_count Instance data count to draw. Use @code 1@endcode as non-instanced draw.
        /// @param first_index Start index of index buffer.
        /// @param base_vertex Index global offset. Indexing like @code vertex_buffer[base_vertex + index]@endcode.
        /// @param first_instance Start index of the instance data buffer.
        virtual void draw_indexed(size_t index_count, size_t instance_count, size_t first_index, size_t base_vertex, size_t first_instance) = 0;

        /// @brief Draws primitives using parameters read from a Buffer.
        /// @note The indirect draw parameters encoded in the buffer must be a tightly packed block of four 32-bit unsigned integer values (16 bytes total), given in the same order as the arguments for @code draw()@endcode .
        /// @param indirect_buffer Buffer containing the indirect draw parameters.
        /// @param indirect_offset Offset in bytes into indirectBuffer where the drawing data begins.
        virtual void draw_indirect(handle_t indirect_buffer, size_t indirect_offset) = 0;

        /// @brief Draws indexed primitives using parameters read from a Buffer.
        /// @note The indirect drawIndexed parameters encoded in the buffer must be a tightly packed block of five 32-bit values (20 bytes total), given in the same order as the arguments for @code drawIndexed()@endcode. The value corresponding to @code baseVertex@endcode is a signed 32-bit integer, and all others are unsigned 32-bit integers. For example:
        /// @param indirect_buffer Buffer containing the indirect draw parameters.
        /// @param indirect_offset Offset in bytes into indirectBuffer where the drawing data begins.
        virtual void draw_indexed_indirect(handle_t indirect_buffer, size_t indirect_offset) = 0;
    };

    class IRenderPassEncoder {
    public:
        virtual ~IRenderPassEncoder() = default;
    };

    class IComputePassEncoder {
    public:
        virtual ~IComputePassEncoder() = default;
    };

    class IRayTracingPassEncoder {
    public:
        virtual ~IRayTracingPassEncoder() = default;
    };

    class ICommandEncoder {
    public:
        virtual ~ICommandEncoder() = default;

        virtual unique_ptr<IRenderPassEncoder> begin_render_pass(const RenderPassDesc&) = 0;
        virtual unique_ptr<IComputePassEncoder> begin_compute_pass(const ComputePassDesc&) = 0;
        virtual unique_ptr<IComputePassEncoder> begin_ray_tracing_pass(const ComputePassDesc&);

        virtual handle_t finish() = 0;
    };

} // namespace avalanche::rendering
