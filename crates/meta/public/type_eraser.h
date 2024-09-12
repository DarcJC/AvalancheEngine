#pragma once
#include <cstddef>
#include <cstdint>
#include <concepts>
#include "class.h"
#include "metamixin.h"
#include "avalanche_meta_export.h"


namespace avalanche {

    class Object;

    /// @brief Providing type info for struct
    /// @note As we don't want a struct to inherit Object, so we should provide a wrapper that has the type info.
    class AVALANCHE_META_API ScopedStruct : public ManageMemoryMixin, public CanGetClassMixin {};

    /// @brief A tag used to allow creating uninitiated value inside container
    struct no_explicit_init_t {};

    template <has_class_name T>
    class ScopedStructContainer : public ScopedStruct {
    public:
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
            return Class::for_name(class_name_v<T>);
        }

        void* memory() override {
            return static_cast<void*>(&m_value);
        }

        [[nodiscard]] void const* memory() const override {
            return static_cast<const void*>(&m_value);
        }

        T m_value;
    };

    /// @brief Merging struct and object to a single type
    class AVALANCHE_META_API Chimera : public CanGetClassMixin, public ManageMemoryMixin {
    public:
        Chimera();
        explicit Chimera(ScopedStruct* scoped_struct_);
        explicit Chimera(Object* object_);

        [[nodiscard]] Class* get_class() const override;

        void* memory() override;
        [[nodiscard]] void const* memory() const override;

    private:
        union {
            Object* object;
            ScopedStruct* scoped_struct;
        } m_value;
        uint8_t m_is_object: 1 = false;
        uint8_t m_is_struct: 1 = false;
        uint16_t padding_0 = 0;
        uint32_t padding_1 = 0;
    };

} // namespace avalanche
