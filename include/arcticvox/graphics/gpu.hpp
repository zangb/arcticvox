#ifndef ARCTICVOX_GRAPHICS_DEVICE_HPP
#define ARCTICVOX_GRAPHICS_DEVICE_HPP

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

#include <vulkan/vulkan_raii.hpp>

#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_enums.hpp>
#include <vulkan/vulkan_handles.hpp>
#include <vulkan/vulkan_structs.hpp>

#include "arcticvox/common/engine_configuration.hpp"
#include "arcticvox/graphics/window.hpp"

namespace arcticvox::graphics {

struct swapchain_support_details {
    vk::SurfaceCapabilitiesKHR surface_capabilities;      //!< The available surface capabilities in
                                                          //!< the swapchain
    std::vector<vk::SurfaceFormatKHR> surface_formats;    //!< The available surface formats in the
                                                          //!< swapchain
    std::vector<vk::PresentModeKHR> present_modes;        //!< The available present modes in the
                                                          //!< swapchain
};

struct queue_family_indices {
    std::optional<uint32_t> graphics_family;    //!< The index to a queue family that supports
                                                //!< graphics operations
    std::optional<uint32_t> present_family;     //!< The index to a queue family supporting
                                                //!< presentation

    /**
     * @brief Returns if queues were found that support both presentation and graphics operations
     *
     * @return True if the queue supports both
     */
    [[nodiscard]] bool is_complete() const {
        return graphics_family && present_family;
    }
};

class gpu {
  public:
    /**
     * @brief Constructs the graphics device
     *
     * @param window    The window wrapping the GLFW instance
     */
    gpu(engine_configuration& config, window& window);

    gpu(const gpu& other) = delete;
    gpu(gpu&& other) = delete;

    gpu& operator=(const gpu& other) = delete;
    gpu& operator=(gpu&& other) = delete;

    ~gpu() = default;

    /**
     * @brief Gets the swapchain information for a provided surface and device
     *
     * @return The swapchain support details belonging to the device and surface
     */
    [[nodiscard]] swapchain_support_details query_swapchain_support();

    /**
     * @brief Checks if the provided extensions are supported by vulkan
     *
     * @param extensions The extensions for which to check the support
     * @param available_extensions The extensions provided by the vulkan instance
     * @return  True if all extensions are supported
     */
    [[nodiscard]] static bool check_extension_support(
        const std::vector<const char*>& extensions,
        const std::vector<vk::ExtensionProperties>& available_extensions);

    /**
     * @brief Checks whether the physical device supports the required extensions, features,
     * swapchain capabilities and queues
     *
     * @param device The device to check for suitability
     * @param extensions The extensions to check for
     * @return True if the device is suitable
     */
    [[nodiscard]] bool check_gpu_suitability(vk::raii::PhysicalDevice device,
                                             const std::vector<const char*>& extensions);

    /**
     * @brief Checks whether the provided layers are supported by the available layer properties of
     * the instance
     *
     * @param layers_to_check The validation layers that need to be checked
     * @param available_layer_props The available layer properties of the current vulkan instance
     * @return True if all provided layers are supported
     */
    [[nodiscard]] static bool check_validation_layer_support(
        const std::vector<const char*>& layers_to_check,
        const std::vector<vk::LayerProperties>& available_layer_props);

    [[nodiscard]] queue_family_indices find_queue_families();

    [[nodiscard]] uint32_t find_memory_type(const uint32_t type_filter,
                                            const vk::MemoryPropertyFlags properties);

    [[nodiscard]] vk::Format find_supported_format(const std::vector<vk::Format>& candidates,
                                                   vk::ImageTiling tiling,
                                                   vk::FormatFeatureFlags features) const;

    [[nodiscard]] auto physical_device() -> vk::raii::PhysicalDevice& {
        return physical_device_;
    }

    [[nodiscard]] auto surface() -> vk::raii::SurfaceKHR& {
        return surface_;
    }

    [[nodiscard]] auto get_engine_configuration() -> engine_configuration& {
        return config_;
    }

  private:
    /**
     * @brief Creates a debug messenger for the vulkan validation layers
     */
    vk::raii::DebugUtilsMessengerEXT create_debug_utils_messenger();

    /**
     * @brief Sets up the Vulkan instance
     */
    vk::raii::Instance create_instance();

    /**
     * @brief Creates the physical vulkan device
     */
    vk::raii::PhysicalDevice create_physical_device();

    /**
     * @brief Sets up the vulkan surface
     */
    vk::raii::SurfaceKHR create_surface();

    /**
     * @brief Tries to find a set of queues that support presentation and graphics operations
     *
     * @param device The physical device on which to query the queues
     * @param surface The surface of the physical device to use for querying
     * @return The set of indices
     */
    [[nodiscard]] queue_family_indices find_queue_families(vk::raii::PhysicalDevice& device,
                                                           vk::raii::SurfaceKHR& surface);

    [[nodiscard]] swapchain_support_details query_swapchain_support(
        vk::raii::PhysicalDevice& device);

    engine_configuration& config_;

    window& window_;                  //!< Handle to the window class wrapping the GLFW window

    vk::raii::Context ctx_;           //!< The vulkan context
    vk::ApplicationInfo app_info_;    //!< The vulkan application information
    vk::raii::Instance instance_;     //!< The vulkan instance object

    vk::raii::DebugUtilsMessengerEXT debug_messenger_;    //!< Handles debug information, e.g. from
                                                          //!< the validation layers
    vk::raii::SurfaceKHR surface_;                //!< The vulkan surface object for rendering to

    vk::raii::PhysicalDevice physical_device_;    //!< Representation of the physical capabalities
                                                  //!< of the GPU

    // vk::raii::Queue graphics_queue_;              //!< The queue in which to push render
    // workloads vk::raii::Queue presentation_queue;    //!< The queue in which to push rendered
    // workloads for
    //!< presentation on screen
};
}
#endif
