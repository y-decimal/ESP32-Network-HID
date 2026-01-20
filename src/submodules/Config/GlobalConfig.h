#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <cstring>
#include <interfaces/ISerializableStructs.h>

/**
 * @brief Global configuration class managing device roles and MAC address.
 *
 * The GlobalConfig class provides methods to set and get device roles and
 * MAC address. It also implements serialization and deserialization methods
 * for storing and retrieving configuration data.
 */
class GlobalConfig : public Serializable
{
public:
  // Definition of the serialized configuration structure
  typedef uint8_t MacAddress[6];

  enum class DeviceRole : uint8_t
  {
    Master,
    Keyboard,
    Count
  };

  // Size of the serialized configuration
  static const size_t SERIALIZED_SIZE = sizeof(DeviceRole) * (size_t)DeviceRole::Count + sizeof(MacAddress);

  struct SerializedConfig
  {
    uint8_t data[SERIALIZED_SIZE];
    size_t size = SERIALIZED_SIZE;
  };

  /**
   * @brief Set the device roles.
   * @param roleArray Array of DeviceRole to set.
   * @param arrSize Size of the roleArray.
   */
  void setRoles(DeviceRole *roleArray, size_t arrSize);

  /**
   * @brief Set the device MAC address.
   * @param mac MAC address to set.
   */
  void setMac(MacAddress mac);

  /**
   * @brief Get the device roles.
   * @param out Output array to store the roles.
   * @param size Size of the output array.
   */
  void getRoles(DeviceRole *out, size_t size);

  /**
   * @brief Get the device MAC address.
   * @param out Output array to store the MAC address.
   * @param size Size of the output array.
   */
  void getMac(uint8_t *out, size_t size);

  size_t packSerialized(uint8_t *output, size_t size) const override;
  size_t unpackSerialized(const uint8_t *input, size_t size) override;
  size_t getSerializedSize() const override;

private:
  // Array to hold device roles
  DeviceRole roles[(size_t)DeviceRole::Count]{DeviceRole::Count};

  // Variable to hold the device MAC address
  MacAddress deviceMac{};
};

#endif