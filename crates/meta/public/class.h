#pragma once


#include "avalanche_meta_export.h"
#include <string_view>
#include <string>
#include "metamixin.h"


namespace avalanche {

    struct metadata_tag : CanGetDeclaringClassMixin {};

    class AVALANCHE_META_API Object : public CanGetClassMixin {};

    class AVALANCHE_META_API Class : public HasMetadataMixin {
    public:
        /**
         * @brief Find class by name
         * @param name full qualified class name
         * @return nullptr if not found.
         */
        static Class* for_name(std::string_view name);

        [[nodiscard]] virtual std::string_view full_name() const = 0;
        [[nodiscard]] virtual const std::string& full_name_str() const = 0;

        [[nodiscard]] virtual size_t hash() const = 0;

        virtual void base_classes(int32_t& num_result, const char* const*& out_data) const = 0;
        [[nodiscard]] virtual bool is_derived_from(std::string_view name) const;
        [[nodiscard]] virtual bool is_derived_from_object() const;
    };

    template <typename>
    struct class_name {
#if DURING_BUILD_TOOL_PROCESS
        static constexpr const char* value = "<default_for_generator>";
#else
        static constexpr const char* value = nullptr;
#endif
    };

    template <typename T>
    constexpr const char* class_name_v = class_name<T>::value;

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
    concept has_class_name = class_name_v<T> != nullptr;

} // namespace avalanche
