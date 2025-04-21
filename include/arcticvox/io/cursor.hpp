#ifndef ARCTICVOX_CURSOR_HPP
#define ARCTICVOX_CURSOR_HPP

#include <GLFW/glfw3.h>

namespace arcticvox::io {
enum class cursor_mode {
    normal = GLFW_CURSOR_NORMAL,             //!< cursor is visible and works as expected
    hidden = GLFW_CURSOR_HIDDEN,             //!< cursor is hidden when it is over a window
    captured = GLFW_CURSOR_CAPTURED,         //!< cursor is visible but confined to content area of
                                             //!< the window
    disabled = GLFW_CURSOR_DISABLED,         //!< cursor is hidden and unlimited, useful for camera
                                             //!< controls
    unavailable = GLFW_CURSOR_UNAVAILABLE    //!< cursor is not available
};

/**
 * @class cursor_delta
 * @brief Describes the delta of the cursor on the screen
 *
 */
struct cursor_delta {
    double x;    //!< the delta in the x direction
    double y;    //!< the delta in the y direction
};

class cursor {
  public:
    /**
     * @brief Constructs the cursor object
     *
     * @param glfw_window Handle to the glfw window this cursor is attached to
     */
    cursor(GLFWwindow* glfw_window);

    ~cursor() = default;

    /**
     * @brief Returns the current cursor mode
     *
     * @return The current cursor mode
     */
    [[nodiscard]] cursor_mode get_current_cursor_mode() const;

    /**
     * @brief Returns the cursor delta from the current position to the previous one
     *
     * @return The cursor delta from the current position to the previous one
     */
    [[nodiscard]] cursor_delta get_cursor_delta() const;
    /**
     * @brief Sets the new cursor mode
     *
     * @param mode The cursor mode to use
     */
    void set_cursor_mode(cursor_mode mode);

    /**
     * @brief Updates the internally stored cursor position
     *
     * @param x The cursor's new x position
     * @param y The cursor's new y position
     */
    void update_position(double x, double y);

  private:
    /**
     * @brief GLFW callback function called when the cursor position changes
     *
     * @param window The window instance
     * @param x_pos The cursor's new x position
     * @param y_pos The cursor's new y position
     */
    static void cursor_position_cb(GLFWwindow* window, double x_pos, double y_pos);

    GLFWwindow* glfw_;           //!< Handle to the glfw window the cursor is attached to

    double prev_x_pos_ = 0.0;    //!< The cursor's last x position
    double prev_y_pos_ = 0.0;    //!< The cursor's last y position

    double x_pos_ = 0.0;         //!< The cursor's most recent x position
    double y_pos_ = 0.0;         //!< The cursor's most recent y position
};

}

#endif
