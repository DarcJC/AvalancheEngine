#pragma once

#include <concepts>

namespace avalanche {
    template <typename TyFirst, typename TySecond>
    class pair {
    public:
        using first_type = TyFirst;
        using second_type = TySecond;

        first_type first;
        second_type second;

    public:
        pair() : first(), second() {}

        template <typename U, typename V>
        requires std::convertible_to<U, first_type> && std::convertible_to<V, second_type >
        pair(const pair<U, V>& other) : first(other.first), second(other.second) {}

        template <typename T = TyFirst, typename U = TySecond>
        requires std::convertible_to<T, TyFirst> && std::convertible_to<U, TySecond>
        pair(const T& val1, const U& val2)
            : first(static_cast<const TyFirst&>(val1))
            , second(static_cast<const TySecond&>(val2))
        {}

        pair& operator=(const pair& other) {
            if (this != &other) {
                first = other.first();
                second = other.second();
            }
            return *this;
        }

    };

    template <typename TyFirst, typename TySecond>
    pair<TyFirst, TySecond> make_pair(TyFirst&& first, TySecond& second) {
        return pair<TyFirst, TySecond>(std::forward<TyFirst>(first), std::forward<TySecond>(second));
    }
}

