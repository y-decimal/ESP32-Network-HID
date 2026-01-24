#ifndef ISERIALIZABLESTRUCT_H
#define ISERIALIZABLESTRUCT_H

#include <cstring>
#include <stdint.h>

/**
 * @brief Interface for serializable classes.
 *
 * The Serializable interface provides methods for packing and unpacking
 * serialized data, as well as retrieving the size of the serialized data.
 */
class ISerializable
{
public:
  /**
   * @brief Pack the structure into a serialized byte array.
   * @param output Pointer to the output byte array.
   * @param size Size of the output byte array.
   * @return Number of bytes written to the output array.
   */
  virtual size_t packSerialized(uint8_t *output, size_t size) const = 0;

  /**
   * @brief Unpack the structure from a serialized byte array.
   * @param input Pointer to the input byte array.
   * @param size Size of the input byte array.
   * @return Number of bytes read from the input array.
   */
  virtual size_t unpackSerialized(const uint8_t *input, size_t size) = 0;

  /**
   * @brief Get the size of the serialized structure.
   * @return Size of the serialized structure in bytes.
   */
  virtual size_t getSerializedSize() const = 0;

  // Virtual destructor.
  virtual ~ISerializable() = default;
};

#endif