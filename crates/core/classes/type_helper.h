#pragma once

#include <array> // std::array
#include <string>
#include <string_view>
#include <utility> // std::index_sequence

namespace avalanche::core {

    // By Matt Rodusek
    // https://rodusek.com/posts/2021/03/09/getting-an-unmangled-type-name-at-compile-time/
    template<std::size_t... Idxs>
    constexpr auto substring_as_array(std::string_view str, std::index_sequence<Idxs...>) {
        return std::array{str[Idxs]..., '\n'};
    }

    template<typename T>
    constexpr auto type_name_array() {
#if defined(__clang__)
        constexpr auto prefix = std::string_view{"[T = "};
        constexpr auto suffix = std::string_view{"]"};
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(__GNUC__)
        constexpr auto prefix = std::string_view{"with T = "};
        constexpr auto suffix = std::string_view{"]"};
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#elif defined(_MSC_VER)
        constexpr auto prefix = std::string_view{"type_name_array<"};
        constexpr auto suffix = std::string_view{">(void)"};
        constexpr auto function = std::string_view{__FUNCSIG__};
#else
#   warning Unsupported compiler
        constexpr auto prefix = std::string_view{"[T = "};
        constexpr auto suffix = std::string_view{"]"};
        constexpr auto function = std::string_view{__PRETTY_FUNCTION__};
#endif

        constexpr auto start = function.find(prefix) + prefix.size();
        constexpr auto end = function.rfind(suffix);

        static_assert(start < end);

        constexpr auto name = function.substr(start, (end - start));
        return substring_as_array(name, std::make_index_sequence<name.size()>{});
    }

    template<typename T>
    struct type_name_holder {
        static inline constexpr auto value = type_name_array<T>();
    };

    template<typename T>
    constexpr auto type_name() -> std::string_view {
        constexpr auto &value = type_name_holder<T>::value;
        return std::string_view{value.data(), value.size()};
    }
    // End By Matt Rodusek

    inline constexpr std::uint64_t fnv1a_hash_64(const char *str, std::size_t length) {
        constexpr std::uint64_t fnv_offset_basis = 0xCBF29CE484222325ULL;
        constexpr std::uint64_t fnv_prime = 0x100000001B3ULL;

        std::uint64_t hash = fnv_offset_basis;
        for (std::size_t i = 0; i < length; ++i) {
            hash ^= static_cast<std::uint64_t>(str[i]);
            hash *= fnv_prime;
        }
        return hash;
    }

    template <typename T>
    constexpr auto id_of_type() -> size_t {
        constexpr auto name = type_name<T>();
        constexpr auto hash = fnv1a_hash_64(name.data(), name.size());
        return hash;
    }

} // namespace avalanche::core
