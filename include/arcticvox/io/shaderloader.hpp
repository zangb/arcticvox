#ifndef ARCTICVOX_SHADERLOADER_HPP
#define ARCTICVOX_SHADERLOADER_HPP

#include <cstdint>
#include <filesystem>
#include <vector>

namespace arcticvox::io {
class shader_loader final {
  public:
    shader_loader() = default;
    ~shader_loader() = default;

    /**
     * @brief Converts the provided byte shader code to a vector of uint32_t words.
     *
     * @param shader_code The binary shader code
     * @return Returns the shader code converted to a vector of uint32_t words
     *
     * @details The function groups four adjacent bytes into one word. The lowest index corresponds
     * to the lowest byte in the word. If the vector size is not a multiple of sizeof(uint32_t) the
     * remaining upper bits in the uint32_t word will be zero bytes.
     */
    [[nodiscard]] static std::vector<uint32_t>
        shader_byte_to_u32(const std::vector<char>& shader_code);

    /**
     * @brief Tries to load the shader code from the provided file path.
     *
     * @param path The path to the file containing the compiled binary shader data
     * @return Returns the loaded shader binary data
     *
     * @details Tries to load the shader code from the provided file path as binary data.
     * If the file could not be opened a std::runtime_error is thrown.
     */
    [[nodiscard]] static std::vector<char> load_from_file(const std::filesystem::path& path);
};

}
#endif
