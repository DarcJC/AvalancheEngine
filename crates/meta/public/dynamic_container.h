#pragma once

#include <bit>
#include <concepts>
#include <type_traits>
#include "avalanche_meta_export.h"
#include "class.h"

#include <vector>


namespace avalanche {
    class DynamicContainerBase;

    constexpr auto G_DATA_ENCODING_ENDIAN = std::endian::little;

    static_assert(
        std::endian::native == std::endian::little || std::endian::native == std::endian::big,
        "Dynamic container doesn't support mixed-endian arch yet."
    );

    class IMetadataKeyValueStorage : public CanGetDeclaringClassMixin {
    public:
        /// @brief Get all keys in this storage
        virtual void keys(size_t& o_num_keys, std::string_view const* & o_keys) const = 0;

        /// @brief Get value from key
        [[nodiscard]] virtual const DynamicContainerBase* get(std::string_view key) const = 0;

        /// @brief A utility to provide modern C++ interface
        void keys(std::vector<std::string_view>& o_vec) const;
    };

    class AVALANCHE_META_API DynamicContainerBase {
    public:
        virtual ~DynamicContainerBase() = default;

        /// @brief Get class name if type is reflected.
        /// @return <unregistered> if not reflected type.
        [[nodiscard]] virtual auto get_class_name() const -> std::string_view;

        /// @brief Get an erased pointer of value
        [[nodiscard]] virtual void* get_storage_pointer() = 0;
        /// @brief Get an erased pointer of value
        [[nodiscard]] virtual const void* get_storage_pointer() const = 0;

        /// @brief Get aligned memory size of storage
        [[nodiscard]] virtual size_t get_aligned_size() const = 0;

        template <typename T>
        T* reinterpret_to() {
            if (sizeof(T) <= get_aligned_size()) {
                return static_cast<T*>(get_storage_pointer());
            }
            return nullptr;
        }

        template <typename T>
        const T* reinterpret_to() const {
            if (sizeof(T) <= get_aligned_size()) {
                return static_cast<const T*>(get_aligned_size());
            }
            return nullptr;
        }

        /// @brief Type-safe casting relying on reflection data
        /// @note You can use this function to validate storage type
        /// @return nullptr if type miss matched.
        template <typename T>
        requires has_class_name<T>
        T* cast_to() {
            const std::string_view name = get_class_name();
            const Class* clazz = Class::for_name(name);

            // Only reflected type can use cast_to
            if (clazz != nullptr) {
                if (name == class_name_sv<T> || clazz->is_derived_from(class_name_sv<T>)) {
                    return static_cast<T*>(get_storage_pointer());
                }
            }

            return nullptr;
        }

        /// @brief Type-safe casting relying on reflection data
        /// @note You can use this function to validate storage type
        /// @return nullptr if type miss matched.
        template <typename T>
        requires has_class_name<T>
        const T* cast_to() const {
            const std::string_view name = get_class_name();
            const Class* clazz = Class::for_name(name);

            // Only reflected type can use cast_to
            if (clazz != nullptr) {
                if (name == class_name_sv<std::remove_cv_t<T>> || clazz->is_derived_from(class_name_sv<std::remove_cv_t<T>>)) {
                    return static_cast<const T*>(get_storage_pointer());
                }
            }

            return nullptr;
        }
    };

    template <typename T>
    requires std::copyable<std::remove_cv_t<T>>
    class GenericDynamicContainer : public DynamicContainerBase {
    public:
        using Super = DynamicContainerBase;
        using value_type = std::remove_cv_t<T>;

        template <typename U = value_type>
        requires std::constructible_from<T, U&&>
        explicit GenericDynamicContainer(U&& value) : m_value_(std::forward<U>(value)) {}

        template <typename U = value_type>
        requires std::constructible_from<T, const U&>
        explicit GenericDynamicContainer(const U& value) : m_value_(value) {}

        [[nodiscard]] auto get_class_name() const -> std::string_view override {
            if constexpr (is_reflect_type_v<std::remove_cv_t<T>>) {
                return class_name_sv<std::remove_cv_t<T>>;
            } else {
                return Super::get_class_name();
            }
        }

        void* get_storage_pointer() override {
            return &m_value_;
        }

        [[nodiscard]] const void* get_storage_pointer() const override {
            return &m_value_;
        }

        [[nodiscard]] size_t get_aligned_size() const override {
            return sizeof(value_type);
        }

        value_type m_value_;
    };

} // namespace avalanche
