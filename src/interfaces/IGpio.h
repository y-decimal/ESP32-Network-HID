#ifndef IGPIO_H
#define IGPIO_H
#include <cstdint>

/**
 * @brief Enumeration for GPIO pin modes.
 */
enum class PinMode : uint8_t { Input, InputPullup, Output };

/**
 * @brief Enumeration for GPIO pin states.
 */
enum class PinState : uint8_t { Low = 0, High = 1 };

/**
 * @brief Interface for GPIO operations.
 *
 * The IGpio interface provides methods for configuring pin modes and
 * reading/writing digital values to GPIO pins.
 */
class IGpio {
public:
  /**
   * @brief Set the mode of a GPIO pin.
   * @param pin The GPIO pin number.
   * @param mode The mode to set for the pin (Input, InputPullup, Output).
   */
  virtual void pinMode(uint8_t pin, PinMode mode) = 0;

  /**
   * @brief Write a digital value to a GPIO pin.
   * @param pin The GPIO pin number.
   * @param value The value to write to the pin (High or Low).
   */
  virtual void digitalWrite(uint8_t pin, PinState value) = 0;

  /**
   * @brief Read a digital value from a GPIO pin.
   * @param pin The GPIO pin number.
   * @return The value read from the pin (High or Low).
   */
  virtual PinState digitalRead(uint8_t pin) = 0;

  // Virtual destructor
  virtual ~IGpio() = default;
};
#endif