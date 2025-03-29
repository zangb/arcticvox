#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <vector>

#include "arcticvox/io/filesystem.hpp"
#include "arcticvox/io/shaderloader.hpp"

namespace arcticvox::io {

std::vector<uint32_t> shader_loader::shader_byte_to_u32(const std::vector<char>& shader_code) {
    const size_t words_u32 = shader_code.size() / sizeof(uint32_t);
    const size_t dangling_bytes = shader_code.size() % sizeof(uint32_t);

    std::vector<uint32_t> converted_shader_code(words_u32);

    for(size_t i = 0U; i < words_u32; ++i) {
        uint32_t converted_word = 0U;
        std::memcpy(&converted_word, &shader_code[i * sizeof(uint32_t)], sizeof(uint32_t));
        converted_shader_code.at(i) = converted_word;
    }
    if(dangling_bytes > 0U) {
        uint32_t converted_word = 0U;
        std::memcpy(
            &converted_word, &shader_code.at(shader_code.size() - dangling_bytes), dangling_bytes);
        converted_shader_code.push_back(converted_word);
    }
    return converted_shader_code;
}

std::vector<char> shader_loader::load_from_file(const std::filesystem::path& path) {
    std::filesystem::path cw_path;
    if(path.is_absolute())
        cw_path = path;
    else
        cw_path = io::get_current_exe_path() / path;

    std::ifstream ifs {cw_path, std::ios::binary};
    if(!ifs.good())
        throw std::runtime_error("Invalid shader path " + cw_path.string());

    const uintmax_t file_sz = std::filesystem::file_size(cw_path);

    // create vector with needed length and default constructed elements so we can use read,
    // as read does not use methods that make the vector aware of elements being added
    std::vector<char> buffer(file_sz);
    ifs.read(reinterpret_cast<char*>(buffer.data()), file_sz);

    return buffer;
}
}
