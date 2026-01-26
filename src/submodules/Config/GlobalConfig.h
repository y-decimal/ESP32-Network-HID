#ifndef GLOBALCONFIG_H
#define GLOBALCONFIG_H

#include <cstring>
#include <interfaces/IConfig.h>
#include <vector>

/**
 * @brief Global configuration class managing device roles and MAC address.
 *
 * The GlobalConfig class provides methods to set and get device roles and
 * MAC address. It also implements serialization and deserialization methods
 * for storing and retrieving configuration data.
 */
class GlobalConfig : public IConfig
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
    HidOutput,
    Count
  };

  /**
   * @brief Set the device modules.
   * @param moduleArray Array of DeviceModule to set.
   * @param arrSize Size of the moduleArray.
   */
  void setDeviceModules(const std::vector<DeviceModule> &modules);

  /**
   * @brief Append a device module.
   * @param module Module to append.
   */
  void appendDeviceModule(DeviceModule module);

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
   * @return A vector containing the device modules.
   */
  std::vector<DeviceModule> getDeviceModules();

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

  static constexpr const char *NAMESPACE = "GlobalCfg";
  const char *getNamespace()override { return NAMESPACE; }
  void setStorage(IStorage *storage) override;
  bool save() override;
  bool load() override;
  bool erase() override;

private:
  IStorage *storage = nullptr;

  // Array to hold device roles
  std::vector<DeviceModule> modules;

  // Stores device mode
  DeviceMode mode = DeviceMode::Count;

  // Variable to hold the device MAC address
  MacAddress deviceMac{0};
};

#endif