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

  enum class DeviceMode : uint8_t
  {
    Master,
    Slave,
    Count
  };

  enum class DeviceModule : uint8_t
  {
    Keyscanner, // Expand later with new modules
    Count
  };

  // Size of the serialized configuration
  static const size_t SERIALIZED_SIZE =
      sizeof(DeviceModule) * (size_t)DeviceModule::Count +
      sizeof(DeviceMode) +
      sizeof(MacAddress);

  struct SerializedConfig
  {
    uint8_t data[SERIALIZED_SIZE]{0};
    size_t size = SERIALIZED_SIZE;
  };

  /**
   * @brief Set the device modules.
   * @param moduleArray Array of DeviceModule to set.
   * @param arrSize Size of the moduleArray.
   */
  void setDeviceModules(DeviceModule *moduleArray, size_t arrSize);

  /**
   * @brief Set the device mode
   * @param mode Mode to set, e.g. DeviceMode::Slave
   */
  void setDeviceMode(DeviceMode mode);

  /**
   * @brief Set the device MAC address.
   * @param mac MAC address to set.
   */
  void setMac(MacAddress mac);

  /**
   * @brief Get the device modules.
   * @param out Output array to store the modules.
   * @param size Size of the output array.
   */
  void getDeviceModules(DeviceModule *out, size_t size);

  /**
   * @brief Get the device mode.
   * @return Returns the device mode, e.g. DeviceMode::Master
   */
  DeviceMode getDeviceMode();

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
  DeviceModule modules[(size_t)DeviceModule::Count]{DeviceModule::Count};
  DeviceMode mode = DeviceMode::Count;

  // Variable to hold the device MAC address
  MacAddress deviceMac{0};
};

#endif