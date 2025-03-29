#include <array>
#include <filesystem>
#include <string>
#include <unistd.h>

#include "arcticvox/io/filesystem.hpp"

#include <linux/limits.h>

namespace arcticvox::io {
std::filesystem::path get_current_exe_path() {
    std::array<char, PATH_MAX> path_buf;

    ssize_t read_length = readlink("/proc/self/exe", path_buf.data(), PATH_MAX);
    auto exe_path =
        std::filesystem::path {std::string(path_buf.data(), (read_length > 0U) ? read_length : 0U)};
    return exe_path.parent_path();
}
}
