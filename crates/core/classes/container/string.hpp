#pragma once

#include "polyfill.h"
#include "container/allocator.hpp"
#include <string_view>
#include <format>

namespace avalanche
{
    // Small String Optimization on stack(also can be data segment and so on) memory size
    // Notice it memory usage will be SSO_BUFFER_SIZE + 1 because we need space to store '\0'
    constexpr size_t SSO_BUFFER_SIZE = 15;

    class AVALANCHE_CORE_API simple_string {
    public:
        using allocator_type = default_allocator<char>;

        simple_string();
        simple_string(const char* s);
        simple_string(std::string_view s);

        simple_string(const simple_string& other);
        simple_string(simple_string&& other) AVALANCHE_NOEXCEPT;

        simple_string& operator=(const simple_string& other) AVALANCHE_NOEXCEPT;
        simple_string& operator=(simple_string&& other) AVALANCHE_NOEXCEPT;

        bool operator==(const simple_string& other) const AVALANCHE_NOEXCEPT;
        bool operator!=(const simple_string& other) const AVALANCHE_NOEXCEPT;

        bool operator==(const char* other) const AVALANCHE_NOEXCEPT;
        bool operator!=(const char* other) const AVALANCHE_NOEXCEPT;

        virtual ~simple_string();

        AVALANCHE_NO_DISCARD bool is_empty() const AVALANCHE_NOEXCEPT;
        void swap(simple_string& other) AVALANCHE_NOEXCEPT;

        operator std::string_view() const AVALANCHE_NOEXCEPT;
        operator const char*() const AVALANCHE_NOEXCEPT;
        operator char*() noexcept;

        char* data();
        AVALANCHE_NO_DISCARD const char* data() const;
        AVALANCHE_NO_DISCARD size_t length() const;
        void ensure_capacity(size_t new_length);

        simple_string& append(const char* s);
        simple_string& append(std::string_view s);

        simple_string& operator+=(const char* s);
        simple_string& operator+=(std::string_view s);
        simple_string& operator+=(const simple_string& other);

    private:
        union {
            char m_ss_buffer[SSO_BUFFER_SIZE + 1]{}; // internal buffer
            char* m_heap_data; // allocated by memory allocator
        };
        size_t m_length = 0;
        size_t m_capacity = SSO_BUFFER_SIZE;
        bool m_is_heap = false;

        void set_data(const char* s, size_t len);

    };

    using string = simple_string;
}

template <>
struct std::formatter<avalanche::simple_string> : std::formatter<string_view> {
    auto format(const avalanche::simple_string& s, std::format_context& ctx) const {
        return formatter<string_view>::format(s, ctx);
    }
};

template <>
struct std::hash<avalanche::simple_string> {
    size_t operator()(const avalanche::simple_string& s) const AVALANCHE_NOEXCEPT {
        return std::hash<const char*>()(s.data());
    }
};


