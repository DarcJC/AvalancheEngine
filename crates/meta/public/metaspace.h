#pragma once

#include "avalanche_meta_export.h"
#include <cstdint>
#include <string_view>

namespace avalanche {

    namespace detail {
        template <typename T>
        struct FNV1aInternal {
            static constexpr uint32_t val = 0x811c9dc5U;
            static constexpr uint32_t prime = 0x1000193U;
        };

        template <>
        struct FNV1aInternal<uint64_t> {
            static constexpr uint64_t val = 0xcbf29ce484222325ULL;
            static constexpr uint64_t prime = 0x100000001b3ULL;
        };
    }

    struct AVALANCHE_META_API FNV1aHash {

        [[nodiscard]] static uint32_t hash_32_fnv1a(std::string_view str) noexcept;
        [[nodiscard]] static uint64_t hash_64_fnv1a(std::string_view str) noexcept;

        size_t operator()(std::string_view str) const noexcept;
    };

    class Class;

    /// @brief MetaSpace is a binary-module level storage class to provide Class information for reflection discovery.
    class AVALANCHE_META_API MetaSpace {
    public:
        /// @brief Get the global MetaSpace managed by meta module
        static MetaSpace& get();

        virtual ~MetaSpace();

        virtual class MetaSpaceProxy create() = 0;

        virtual void register_class(Class* new_class) = 0;
        virtual void unregister_class(Class* new_class) = 0;

        virtual Class* find_class(std::string_view name) = 0;

    protected:
        virtual MetaSpace* allocate_meta_space() = 0;
        virtual void deallocate_meta_space(MetaSpace* space) = 0;

        friend class MetaSpaceProxy;
    };

    class AVALANCHE_META_API MetaSpaceProxy final {
    public:
        explicit MetaSpaceProxy(MetaSpace& parent_space);
        ~MetaSpaceProxy();

        MetaSpace* operator->() const noexcept;

    private:
        MetaSpace& m_parent_space;
        MetaSpace* m_value;

        friend class MetaSpace;
    };

} // namespace avalanche

#define EXTERN_MODULE_METASPACE(MODULE_NAME) extern avalanche::MetaSpaceProxy G_##MODULE_NAME##_METASPACE_;
#define MODULE_METASPACE_STORAGE(MODULE_NAME) avalanche::MetaSpaceProxy G_##MODULE_NAME##_METASPACE_ = avalanche::MetaSpace::get().create();
#define GET_MODULE_METASPACE(MODULE_NAME) G_##MODULE_NAME##_METASPACE_
