#pragma once


#include "avalanche_meta_export.h"
#include <string_view>
#include <string>
#include "metamixin.h"


namespace avalanche {

    class Field;

    struct metadata_tag : CanGetDeclaringClassMixin {};

    class AVALANCHE_META_API Object : public CanGetClassMixin {};

    /// @brief Runtime information of a type
    class AVALANCHE_META_API Class : public HasMetadataMixin {
    public:
        /// @brief Find class by name
        /// @param name full qualified class name
        /// @return nullptr if not found.
        static Class* for_name(std::string_view name);

        /// @brief Fully qualified type name of this class
        /// @return e.g. @code "avalanche::Object"@endcode
        [[nodiscard]] virtual std::string_view full_name() const = 0;
        /// @brief Value is same as @code full_name()@endcode but returning as const string&
        /// @return e.g. @code "avalanche::Object"@endcode
        [[nodiscard]] virtual const std::string& full_name_str() const = 0;

        /// @brief Type hash of this class.
        /// @note The probability of a collision is rare, but not impossible.
        /// @return Value of type hash
        [[nodiscard]] virtual size_t hash() const = 0;

        /// @brief Get the name of its base classes
        /// @note You can use @code Class::for_name()@endcode to get class instance from name. But not all base classes is registered.
        /// @param num_result Base classes count
        /// @param out_data Name of base classes
        virtual void base_classes(int32_t& num_result, const char* const*& out_data) const;
        /// @brief Check if this class derived from the other class
        /// @param name Fully qualified name of base class
        [[nodiscard]] virtual bool is_derived_from(std::string_view name) const;
        /// @brief Check if this class derived from 'avalanche::Object'
        [[nodiscard]] virtual bool is_derived_from_object() const;

        /// @brief Is fundamental types of C/C++. Like @code int8_t, int16_t, ...@endcode.
        [[nodiscard]] virtual bool is_primitive_type() const;

        /// @brief Get fields
        /// @note Only reflected fields will be listed here.
        /// @param num_result Field count
        /// @param out_data
        virtual void fields(int32_t& num_result, const Field* const*& out_data) const;

        /// @brief Comparing if two @code Class@endcode is same
        /// @return Currently, @code true@endcode if memory address is same or @code full_name()@endcode is same.
        /// @note Comparing hash might be fast, but I don't want to deal with the collisions.
        [[nodiscard]] virtual bool equals_to(const Class& other) const;
        bool operator==(const Class& other) const;
        bool operator!=(const Class &) const;
    };

    template <typename>
    struct class_name {
#if DURING_BUILD_TOOL_PROCESS
        static constexpr const char* value = "<default_for_generator>";
        static constexpr bool primitive = false;
#endif
    };

    template <typename T>
    constexpr const char* class_name_v = class_name<T>::value;

    template <typename T>
    constexpr bool class_name_p = class_name<T>::primitive;

    template <typename T>
    class ObjectCRTP : public Object {
    public:
        static Class* static_class() {
            return Class::for_name(class_name_v<T>);
        }

        [[nodiscard]] Class* get_class() const override {
            return static_class();
        }
    };

    template <typename T>
    concept has_class_name = requires {
        class_name_v<T>;
    };


    /// Declaring class name of fundamental types
    template <> struct class_name<int8_t> { static constexpr const char* value = "int8"; static constexpr bool primitive = true; };
    template <> struct class_name<int16_t> { static constexpr const char* value = "int16"; static constexpr bool primitive = true; };
    template <> struct class_name<int32_t> { static constexpr const char* value = "int32"; static constexpr bool primitive = true; };
    template <> struct class_name<int64_t> { static constexpr const char* value = "int64"; static constexpr bool primitive = true; };
    template <> struct class_name<uint8_t> { static constexpr const char* value = "uint8"; static constexpr bool primitive = true; };
    template <> struct class_name<uint16_t> { static constexpr const char* value = "uint16"; static constexpr bool primitive = true; };
    template <> struct class_name<uint32_t> { static constexpr const char* value = "uint32"; static constexpr bool primitive = true; };
    template <> struct class_name<uint64_t> { static constexpr const char* value = "uint64"; static constexpr bool primitive = true; };
    template <> struct class_name<bool> { static constexpr const char* value = "bool"; static constexpr bool primitive = true; };
    template <> struct class_name<char*> { static constexpr const char* value = "char*"; static constexpr bool primitive = true; };
    template <> struct class_name<const char*> { static constexpr const char* value = "const char*"; static constexpr bool primitive = true; };
    template <> struct class_name<float> { static constexpr const char* value = "float"; static constexpr bool primitive = true; };
    template <> struct class_name<double> { static constexpr const char* value = "double"; static constexpr bool primitive = true; };
    template <> struct class_name<long double> { static constexpr const char* value = "long double"; static constexpr bool primitive = true; };
    template <> struct class_name<void*> { static constexpr const char* value = "void*"; static constexpr bool primitive = true; };

    struct fundamental_type_names {
        static constexpr const char* value[] = {
            class_name_v<int8_t>,
            class_name_v<int16_t>,
            class_name_v<int32_t>,
            class_name_v<int64_t>,
            class_name_v<uint8_t>,
            class_name_v<uint16_t>,
            class_name_v<uint32_t>,
            class_name_v<uint64_t>,
            class_name_v<bool>,
            class_name_v<char*>,
            class_name_v<const char*>,
            class_name_v<float>,
            class_name_v<double>,
            class_name_v<long double>,
            class_name_v<void*>,
        };
        static constexpr size_t count = sizeof(value) / sizeof(const char*);
    };

    constexpr const char* const* fundamental_type_names_v = fundamental_type_names::value;

    class AVALANCHE_META_INTERNAL PrimitiveClass : public Class {
    public:
        explicit PrimitiveClass(const char* name);

        [[nodiscard]] std::string_view full_name() const override;
        [[nodiscard]] const std::string& full_name_str() const override;
        [[nodiscard]] size_t hash() const override;
        [[nodiscard]] bool is_primitive_type() const override;

    private:
        const char* m_name = nullptr;
        std::string m_name_str;
    };

} // namespace avalanche
