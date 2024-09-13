#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <concepts>
#include "class.h"
#include "metamixin.h"
#include "avalanche_meta_export.h"


namespace avalanche {

    class Object;

    namespace detail {
        template <typename>
        struct is_pointer_reference : std::false_type {};

        template <typename T>
        struct is_pointer_reference<T*&> : std::true_type {};
    } // namespace detail

    struct TypeQualifiers {
        uint32_t reference: 1 = false;
        uint32_t pointer: 1 = false;
        uint32_t lvalue: 1 = false;
        uint32_t rvalue : 1 = false;

        bool operator==(const TypeQualifiers &) const = default;
        bool operator!=(const TypeQualifiers &) const = default;
    };

    /// @brief Providing type info for struct
    /// @note As we don't want a struct to inherit Object, so we should provide a wrapper that has the type info.
    class AVALANCHE_META_API ScopedStruct : public ManageMemoryMixin, public CanGetClassMixin {
    public:
        /// @brief Get type qualifiers of contained type
        [[nodiscard]] virtual TypeQualifiers qualifiers() const = 0;

        /// @brief Check if target type can cast to this type safely.
        /// Reference is safe to cast to pointer with same type.
        /// @note There is no way to get info from a @code typename@endcode at runtime.
        /// @note So we only have the limited and generated knowledge in type casting.
        [[nodiscard]] virtual bool convertible_from(const ScopedStruct& other) const = 0;
    };

    /// @brief A tag used to allow creating uninitiated value inside container
    struct no_explicit_init_t {};

    template <has_class_name T>
    class ScopedStructContainer : public ScopedStruct {
    public:
        /// @code
        /// T** -> T* (reject by class_name<T*>::value not found)
        /// T* -> T
        /// T*& -> T
        /// T*&& -> T
        /// T& -> T
        /// @endcode
        using decayed_value_t = std::remove_pointer<std::decay_t<T>>;

        using storage_value_t = std::conditional_t<detail::is_pointer_reference<T>::value, std::decay_t<T>, T>;

        template <std::default_initializable = T>
        explicit ScopedStructContainer(no_explicit_init_t) {}

        template <typename U = T>
        requires std::constructible_from<T, U>
        explicit ScopedStructContainer(U&& value) : m_value(std::forward<U>(value)) {}

        template <typename... Args>
        requires std::constructible_from<T, Args...>
        explicit ScopedStructContainer(Args&&... args) : m_value(std::forward<Args>(args)...) {}

        template <typename U = T>
        requires std::constructible_from<T, const U&>
        explicit ScopedStructContainer(const ScopedStructContainer<U>& other) : m_value(other.m_value) {}

        template <typename U = T>
        requires std::constructible_from<T, U&&>
        explicit ScopedStructContainer(ScopedStructContainer<U>&& other) : m_value(std::move(other.m_value)) {}

        template <typename U = T>
        requires std::constructible_from<T, const U&>
        ScopedStructContainer& operator=(const ScopedStructContainer<U>& other) {
            if (&other != this) {
                m_value = other.m_value;
            }
            return *this;
        }

        template <typename U = T>
        requires std::constructible_from<T, U&&>
        ScopedStructContainer& operator=(ScopedStructContainer<U>&& other) {
            if (&other != this) {
                m_value = std::move(other.m_value);
            }
            return *this;
        }

        [[nodiscard]] Class* get_class() const override {
            return Class::for_name(class_name_v<decayed_value_t>);
        }

        void* memory() override {
            if constexpr (std::is_pointer_v<storage_value_t>) {
                return static_cast<void*>(m_value);
            } else {
                return static_cast<void*>(&m_value);
            }
        }

        [[nodiscard]] void const* memory() const override {
            if constexpr (std::is_pointer_v<storage_value_t>) {
                return static_cast<const void*>(m_value);
            } else {
                return static_cast<const void*>(&m_value);
            }
        }

        [[nodiscard]] TypeQualifiers qualifiers() const override {
            static TypeQualifiers qualifiers {
                .pointer = std::is_pointer_v<std::decay_t<T>>, // Ignore reference here to get real type
                .reference = std::is_reference_v<T>,
                .lvalue = std::is_lvalue_reference_v<T>,
                .rvalue = std::is_rvalue_reference_v<T>,
            };
            return qualifiers;
        }

        [[nodiscard]] bool convertible_from(const ScopedStruct& other) const override {
            // Return false if not the same type
            if (*get_class() != *other.get_class()) return false;

            const TypeQualifiers self_qualifiers = qualifiers();
            const TypeQualifiers other_qualifiers = other.qualifiers();

            // Return true if qualifiers is same
            if (self_qualifiers == other_qualifiers) return true;

            // T& -> T*
            if (other_qualifiers.reference && self_qualifiers.pointer) return true;

            return false;
        }

        storage_value_t m_value;
    };

    /// @brief Merging struct and object to a single type
    class AVALANCHE_META_API Chimera : public CanGetClassMixin, public ManageMemoryMixin {
    public:
        Chimera();
        explicit Chimera(ScopedStruct* scoped_struct_);
        explicit Chimera(Object* object_);
        Chimera(const Chimera& other);

        [[nodiscard]] Class* get_class() const override;

        void* memory() override;
        [[nodiscard]] void const* memory() const override;

        [[nodiscard]] bool is_valid() const;

    private:
        union {
            Object* object;
            ScopedStruct* scoped_struct;
        } m_value{};
        uint8_t m_is_object: 1 = false;
        uint8_t m_is_struct: 1 = false;
        uint16_t padding_0 = 0;
        uint32_t padding_1 = 0;
    };

} // namespace avalanche
