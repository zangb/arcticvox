#ifndef SHK_FMT_FORMATTERS_HPP
#define SHK_FMT_FORMATTERS_HPP

#include <string_view>
#include <type_traits>

#include "spdlog/fmt/bundled/core.h"
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <spdlog/fmt/bundled/format.h>
#include <spdlog/fmt/ostr.h>

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<1, T, Q>> : fmt::formatter<std::string_view> {
    static_assert(std::is_arithmetic_v<T>, "Non-arithmetic type for glm::vec formatter");
    auto format(glm::vec<1, T, Q> vec, fmt::format_context& ctx) -> fmt::format_context::iterator {
        return fmt::format_to(ctx.out(), "[{:+}]", vec.x);
    }
};
template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<2, T, Q>> : fmt::formatter<std::string_view> {
    static_assert(std::is_arithmetic_v<T>, "Non-arithmetic type for glm::vec formatter");
    auto format(glm::vec<2, T, Q> vec, fmt::format_context& ctx) -> fmt::format_context::iterator {
        return fmt::format_to(ctx.out(), "[{:+}, {:+}]", vec.x, vec.y);
    }
};
template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<3, T, Q>> : fmt::formatter<std::string_view> {
    static_assert(std::is_arithmetic_v<T>, "Non-arithmetic type for glm::vec formatter");
    auto format(glm::vec<3, T, Q> vec, fmt::format_context& ctx) -> fmt::format_context::iterator {
        return fmt::format_to(ctx.out(), "[{:+}, {:+}, {:+}]", vec.x, vec.y, vec.z);
    }
};
template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::vec<4, T, Q>> : fmt::formatter<std::string_view> {
    static_assert(std::is_arithmetic_v<T>, "Non-arithmetic type for glm::vec formatter");
    auto format(glm::vec<4, T, Q> vec, fmt::format_context& ctx) -> fmt::format_context::iterator {
        return fmt::format_to(ctx.out(), "[{:+}, {:+}, {:+}, {:+}]", vec.x, vec.y, vec.z, vec.w);
    }
};

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::mat<4, 4, T, Q>> : fmt::formatter<std::string_view> {
    static_assert(std::is_arithmetic_v<T>, "Non-arithmetic type for glm::mat4 formatter");
    auto format(glm::mat<4, 4, T, Q> mat, fmt::format_context& ctx)
        -> fmt::format_context::iterator {
        return fmt::format_to(
            ctx.out(),
            "\n[{:+}, {:+}, {:+}, {:+}\n{:+}, {:+}, {:+}, {:+}\n{:+}, {:+}, {:+}, "
            "{:+}\n{:+}, {:+}, {:+}, {:+}]",
            mat[0][0],
            mat[1][0],
            mat[2][0],
            mat[3][0],
            mat[0][1],
            mat[1][1],
            mat[2][1],
            mat[3][1],
            mat[0][2],
            mat[1][2],
            mat[2][2],
            mat[3][2],
            mat[0][3],
            mat[1][3],
            mat[2][3],
            mat[3][3]);
    }
};

template<typename T, glm::qualifier Q>
struct fmt::formatter<glm::qua<T, Q>> : fmt::formatter<std::string_view> {
    static_assert(std::is_arithmetic_v<T>, "Non-arithmetic type for glm::quat formatter");
    auto format(glm::qua<T, Q> quat, fmt::format_context& ctx) -> fmt::format_context::iterator {
        return fmt::format_to(
            ctx.out(), "[{:+}, {:+}, {:+}, {:+}]", quat.x, quat.y, quat.z, quat.w);
    }
};

#endif
