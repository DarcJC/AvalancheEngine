#include "render_resource.h"


namespace avalanche::rendering {

    IResource::IResource(IRenderDevice &render_device)
        : m_render_device(render_device)
    {}

    IResource::~IResource() = default;

    bool IResource::is_committed() const { return true; }

    bool IResource::is_valid() const { return false; }

    bool IResource::is_external() const {
        return false;
    }

    IRenderDevice & IResource::get_render_device() {
        return m_render_device;
    }

    detail::ResourceFlags& IResource::flags() {
        return m_flag;
    }

    int32_t detail::ResourceFlags::increase_rc(const std::memory_order memory_order) {
        const uint32_t old_value = m_value.fetch_add(1, memory_order);
        AVALANCHE_CHECK((old_value & Bits::Deleting) == 0, "Attempting to add refernce for a deleting resource");
        const int32_t ref_count = static_cast<int32_t>(old_value & Bits::RefCounterMask) + 1;
        AVALANCHE_CHECK(ref_count <= Bits::RefCounterMask, "Reference counter is overflowed");
        return ref_count;
    }

    int32_t detail::ResourceFlags::decrease_rc(const std::memory_order memory_order) {
        const uint32_t old_value = m_value.fetch_sub(1, memory_order);
        AVALANCHE_CHECK((old_value & Bits::Deleting) == 0, "Attempting to add refernce for a deleting resource");
        const int32_t ref_count = static_cast<int32_t>(old_value & Bits::RefCounterMask) - 1;
        AVALANCHE_CHECK((old_value & Bits::RefCounterMask) > 0, "Reference counter is underflowed");
        return ref_count;
    }

    bool detail::ResourceFlags::mark_for_delete(const std::memory_order memory_order) {
        const uint32_t old_value = m_value.fetch_or(Bits::MarkForDelete, memory_order);
        AVALANCHE_CHECK((old_value & Bits::Deleting) == 0, "Attempting to mark a deleting resource for delete");
        return (old_value & Bits::MarkForDelete) != 0;
    }

    bool detail::ResourceFlags::unmark_for_delete(const std::memory_order memory_order) {
        const uint32_t old_value = m_value.fetch_xor(Bits::MarkForDelete, memory_order);
        AVALANCHE_CHECK((old_value & Bits::Deleting) == 0, "Attempting to unmark a deleting resource for delete");
        const bool old_marked_for_delete = (old_value & Bits::MarkForDelete) != 0;
        AVALANCHE_CHECK(old_marked_for_delete, "");
        return old_marked_for_delete;
    }

    bool detail::ResourceFlags::set_deleting() {
        const uint32_t local_value = m_value.load(std::memory_order_acquire);
        AVALANCHE_CHECK((local_value & Bits::MarkForDelete) != 0, "Must mark resource for delete before set deleting");
        AVALANCHE_CHECK((local_value & Bits::Deleting) == 0, "Resource is already deleting");
        const uint32_t ref_count = local_value & Bits::RefCounterMask;

        if (ref_count == 0) {
            m_value.fetch_or(Bits::Deleting, std::memory_order_acquire);
            return true;
        } else {
            unmark_for_delete(std::memory_order_release);
            return false;
        }
    }

    bool detail::ResourceFlags::is_valid(const std::memory_order memory_order) const {
        const uint32_t local_value = m_value.load(memory_order);
        return (local_value & Bits::MarkForDelete) == 0 && (local_value & Bits::RefCounterMask) != 0;
    }

    bool detail::ResourceFlags::is_marked_for_delete(const std::memory_order memory_order) const {
        return (m_value.load(memory_order) & Bits::MarkForDelete) != 0;
    }

    int32_t detail::ResourceFlags::ref_count(const std::memory_order memory_order) const {
        return static_cast<int32_t>(m_value.load(memory_order) & Bits::RefCounterMask);
    }

} // namespace avalanche::rendering
