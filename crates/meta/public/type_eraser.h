#pragma once
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <expected>
#include "avalanche_meta_export.h"
#include "class.h"
#include "metamixin.h"

#include <cassert>


namespace avalanche {

    class Object;

    namespace detail {
        template <typename>
        struct is_pointer_reference : std::false_type {};

        template <typename T>
        struct is_pointer_reference<T*&> : std::true_type {};
    } // namespace detail

    /// @brief Providing type info for struct
    /// @note As we don't want a struct to inherit Object, so we should provide a wrapper that has the type info.
    class AVALANCHE_META_API ScopedStruct : public ManageMemoryMixin, public CanGetClassMixin, public HasQualifiersMixin {
    public:
        /// @brief Check if target type can cast to this type safely.
        /// Reference is safe to cast to pointer with same type.
        /// @note There is no way to get info from a @code typename@endcode at runtime.
        /// @note So we only have the limited and generated knowledge in type casting.
        [[nodiscard]] virtual bool convertible_from(const ScopedStruct& other) const;
    };

    /// @brief A tag used to allow creating uninitiated value inside container
    struct no_explicit_init_t {};

    template <typename T>
    requires has_class_name<std::remove_pointer_t<std::decay_t<T>>>
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

        template <typename = T>
        requires std::default_initializable
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
                .default_initialized = false,
                .pointer = std::is_pointer_v<std::decay_t<T>>, // Ignore reference here to get real type
                .reference = std::is_reference_v<T>,
                .lvalue = std::is_lvalue_reference_v<T>,
                .rvalue = std::is_rvalue_reference_v<T>,
            };
            return qualifiers;
        }

        storage_value_t m_value;
    };

    class FieldProxyStruct : public ScopedStruct {
    public:
        FieldProxyStruct(void* proxied_memory_, Class* clazz_);

        [[nodiscard]] Class* get_class() const override;

        void* memory() override {
            return m_proxied_memory;
        }

        [[nodiscard]] void const* memory() const override {
            return m_proxied_memory;
        }

        [[nodiscard]] TypeQualifiers qualifiers() const override {
            return {
                .pointer = true,
            };
        }

    private:
        void* m_proxied_memory = nullptr;
        Class* m_class = nullptr;
    };

    /// @brief Merging struct and object to a single type
    class AVALANCHE_META_API Chimera : public CanGetClassMixin, public ManageMemoryMixin, public HasQualifiersMixin {
    public:
        Chimera();
        explicit Chimera(ScopedStruct* scoped_struct_, bool managed_ = false);
        explicit Chimera(Object* object_, bool managed_ = false);
        Chimera(Chimera&& other) noexcept;

        Chimera& operator=(Chimera&& other) noexcept;

        ~Chimera() override;

        [[nodiscard]] Class* get_class() const override;

        void* memory() override;
        [[nodiscard]] void const* memory() const override;

        [[nodiscard]] TypeQualifiers qualifiers() const override;

        [[nodiscard]] bool is_valid() const;
        [[nodiscard]] bool is_managed() const;

        /// @brief Release memory if @code is_managed()@endcode is true
        /// @note Do nothing if @code is_managed() == false@endcode
        void release();
        void swap(Chimera& other) noexcept;

        [[nodiscard]] bool is_signed_integer() const;
        [[nodiscard]] bool is_unsigned_integer() const;
        [[nodiscard]] int64_t as_integer() const;
        [[nodiscard]] double as_float_point() const;

        enum class cast_error {
            null_class,
            unexpected_type,
            invalid_memory,
        };

        template <typename T>
        requires has_class_name<std::remove_pointer_t<std::decay_t<T>>>
        [[nodiscard]] auto as() -> std::expected<T, cast_error> {
            const Class* clazz = get_class();

            if (clazz == nullptr) {
                return std::unexpected(cast_error::null_class);
            }

            if (nullptr == memory()) {
                return std::unexpected(cast_error::invalid_memory);
            }

            constexpr std::string_view target_class_name = class_name_sv<std::remove_pointer_t<std::decay_t<T>>>;
            const TypeQualifiers modifiers = qualifiers();
            if (clazz->full_name() == target_class_name) {
                if constexpr (std::is_pointer_v<T>) {
                    return static_cast<T>(memory());
                } else {
                    return static_cast<T>(*static_cast<T*>(memory()));
                }
            }
            return std::unexpected(cast_error::unexpected_type);
        }

    private:
        union {
            Object* object;
            ScopedStruct* scoped_struct;
        } m_value{};
        struct {
            uint64_t m_is_object: 1 = false;
            uint64_t m_is_struct: 1 = false;
            uint64_t m_managed_memory: 1 = false;
        } m_flags;
    };

} // namespace avalanche
