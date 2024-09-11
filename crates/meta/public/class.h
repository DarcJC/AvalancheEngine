#pragma once


#include "avalanche_meta_export.h"
#include <string_view>
#include <string>


namespace avalanche {

    struct metadata_tag {};

    class AVALANCHE_META_API Object {
    public:
        virtual ~Object() = default;

        virtual class Class* get_class() = 0;
    };

    class AVALANCHE_META_API Class {
    public:
        /**
         * @brief Find class by name
         * @param name full qualified class name
         * @return nullptr if not found.
         */
        static Class* for_name(std::string_view name);

        virtual ~Class() = default;

        virtual std::string_view full_name() = 0;
        virtual const std::string& full_name_str() = 0;

        virtual size_t hash() = 0;
    };

    template <typename>
    struct class_name {
        static constexpr const char* value = nullptr;
    };

    template <typename T>
    constexpr const char* class_name_v = class_name<T>::value;

    template <typename T>
    class ObjectCRTP : public Object {
    public:
        static Class* static_class() {
            return Class::for_name(class_name_v<T>);
        }

        Class* get_class() override {
            return static_class();
        }
    };

} // namespace avalanche
