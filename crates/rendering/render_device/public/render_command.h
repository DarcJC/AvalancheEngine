#pragma once

#include <atomic>
#include <cstddef>
#include "avalanche_render_device_export.h"
#include "container/color.hpp"
#include "render_enums.h"
#include "render_resource.h"


namespace avalanche::rendering {
    struct RenderPassDesc;
    struct ComputePassDesc;
    struct RenderBundleDesc;

    /// @brief A mixin that keeping encoder state.
    /// @note The operations provided here is thread safe.
    class AVALANCHE_RENDER_DEVICE_API CommandEncoderMixin {
    public:
        CommandEncoderMixin();
        virtual ~CommandEncoderMixin();

        /// @brief Get current state of the encoder
        AVALANCHE_NO_DISCARD CommandEncoderState get_encoder_state() const;

        /// @brief Check if encoder can be written to now
        AVALANCHE_NO_DISCARD bool can_write() const;

    protected:
        /// @brief Set current state of the encoder
        void set_encoder_state(CommandEncoderState new_state);

    private:
        AVALANCHE_NO_DISCARD const std::atomic<uint8_t>* get_atomic_value() const;
        std::atomic<uint8_t>* get_atomic_value();

        alignas(std::atomic<uint8_t>) std::byte m_storage_[sizeof(std::atomic<uint8_t>)]{};
    };

    class AVALANCHE_RENDER_DEVICE_API BindingCommandsMixin {
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
    class AVALANCHE_RENDER_DEVICE_API RenderCommandsMixin {
    public:
        virtual ~RenderCommandsMixin() = default;

        /// @brief Set pipeline object to use in the next operations
        /// @param render_pipeline The handle of pipeline state object of GAPI currently using.
        virtual void set_pipeline(handle_t render_pipeline) = 0;

        /// @brief  Sets the current index buffer.
        /// @param buffer Buffer containing index data to use for subsequent drawing commands.
        /// @param index_format Format of the index data contained in buffer.
        /// @param offset Offset in bytes into buffer where the index data begins. Defaults to 0.
        /// @param size Size in bytes of the index data in buffer. Defaults to the size of the buffer minus the offset.
        virtual void set_index_buffer(handle_t buffer, IndexFormat index_format, size_t offset, size_t size) = 0;

        /// @brief Sets the current vertex buffer for the given slot.
        /// @param slot The vertex buffer slot to set the vertex buffer for.
        /// @param buffer Buffer containing vertex data to use for subsequent drawing commands.
        /// @param offset Offset in bytes into buffer where the vertex data begins. Defaults to 0.
        /// @param size Size in bytes of the vertex data in buffer. Defaults to the size of the buffer minus the offset.
        virtual void set_vertex_buffer(uint32_t slot, handle_t buffer, size_t offset, size_t size) = 0;

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

    class AVALANCHE_RENDER_DEVICE_API IRenderBundle {
    public:
        virtual ~IRenderBundle() = default;
    };

    class AVALANCHE_RENDER_DEVICE_API IRenderBundleEncoder : public CommandEncoderMixin, public BindingCommandsMixin, public RenderCommandsMixin {
    public:
        virtual unique_ptr<IRenderBundle> finish(const RenderBundleDesc& desc) = 0;
    };

    class AVALANCHE_RENDER_DEVICE_API IRenderPassEncoder : public CommandEncoderMixin, public RenderCommandsMixin, public BindingCommandsMixin {
    public:
        /// @brief Sets the viewport used during the rasterization stage to linearly map from normalized device coordinates to viewport coordinates.
        /// @param x Minimum X value of the viewport in pixels.
        /// @param y Minimum Y value of the viewport in pixels.
        /// @param width Width of the viewport in pixels.
        /// @param height Height of the viewport in pixels.
        /// @param min_depth Minimum depth value of the viewport.
        /// @param max_depth Maximum depth value of the viewport.
        virtual void set_viewport(float x, float y, float width, float height, float min_depth, float max_depth) = 0;

        /// @brief Sets the scissor rectangle used during the rasterization stage. After transformation into viewport coordinates any fragments which fall outside the scissor rectangle will be discarded.
        /// @param x Minimum X value of the scissor rectangle in pixels.
        /// @param y Minimum Y value of the scissor rectangle in pixels.
        /// @param width Width of the scissor rectangle in pixels.
        /// @param height Height of the scissor rectangle in pixels.
        virtual void set_scissor_rect(size_t x, size_t y, size_t width, size_t height) = 0;

        /// @brief Sets the constant blend color and alpha values used with "constant" and "one-minus-constant" GPUBlendFactors.
        /// @param color New blend constant
        virtual void set_blend_constant(const Color& color) = 0;

        /// @brief Set stencil reference value dynamically.
        /// @param reference_value New stencil reference value
        virtual void set_stencil_reference(size_t reference_value) = 0;

        /// @brief Appending commands in bundles to current encoder.
        /// @param bundles A sequence of render command bundles.
        virtual void execute_bundles(const vector<IRenderBundle>& bundles) = 0;

        /// @brief Completes recording of the render pass commands sequence.
        virtual void end() = 0;
    };

    class AVALANCHE_RENDER_DEVICE_API IComputePassEncoder : public CommandEncoderMixin, public BindingCommandsMixin {
    public:
        /// @brief Set current compute pipeline state.
        virtual void set_pipeline(handle_t compute_pipeline) = 0;

        /// @brief Completes recording of the compute pass commands sequence.
        virtual void end() = 0;
    };

    class AVALANCHE_RENDER_DEVICE_API IRayTracingPassEncoder : public CommandEncoderMixin, public BindingCommandsMixin{
    public:
        /// @brief Set current ray tracing pipeline state.
        virtual void set_pipeline(handle_t ray_tracing_pipeline) = 0;

        /// @brief Completes recording of the ray tracing pass commands sequence.
        virtual void end() = 0;
    };

    class AVALANCHE_RENDER_DEVICE_API ICommandEncoder : public CommandEncoderMixin {
    public:
        virtual ~ICommandEncoder() = default;

        virtual unique_ptr<IRenderPassEncoder> begin_render_pass(const RenderPassDesc&) = 0;
        virtual unique_ptr<IComputePassEncoder> begin_compute_pass(const ComputePassDesc&) = 0;
        virtual unique_ptr<IComputePassEncoder> begin_ray_tracing_pass(const ComputePassDesc&);

        virtual handle_t finish() = 0;
    };

} // namespace avalanche::rendering
