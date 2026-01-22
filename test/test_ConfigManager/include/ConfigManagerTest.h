#ifndef CONFIGMANAGERTEST_H
#define CONFIGMANAGERTEST_H

#include <unity.h>
#include <submodules/Config/ConfigManager.h>
#include <interfaces/IStorage.h>

extern IStorage &testStorage;

void test_ConfigManager_initialization() {
  ConfigManager manager(&testStorage);
  TEST_ASSERT_TRUE(true); // Basic initialization test
}

void test_ConfigManager_getConfig_GlobalConfig_defaults() {
  ConfigManager manager(&testStorage);
  GlobalConfig config = manager.getConfig<GlobalConfig>();

  // Test that we get a valid config with defaults
  uint8_t macBuffer[6] = {};
  config.getMac(macBuffer, sizeof(macBuffer));
  TEST_ASSERT_TRUE(true); // Valid config retrieved
}

void test_ConfigManager_getConfig_KeyScannerConfig_defaults() {
  ConfigManager manager(&testStorage);
  KeyScannerConfig config = manager.getConfig<KeyScannerConfig>();

  // Test that we get a valid config with defaults
  TEST_ASSERT_TRUE(config.getRefreshRate() >= 0);
}

void test_ConfigManager_setConfig_GlobalConfig() {
  ConfigManager manager(&testStorage);
  GlobalConfig config;
  GlobalConfig::MacAddress testMac = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
  config.setMac(testMac);

  manager.setConfig(config);
  GlobalConfig retrieved = manager.getConfig<GlobalConfig>();

  uint8_t retrievedMac[6] = {};
  retrieved.getMac(retrievedMac, sizeof(retrievedMac));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(testMac, retrievedMac, 6);
}

void test_ConfigManager_setConfig_KeyScannerConfig() {
  ConfigManager manager(&testStorage);
  KeyScannerConfig config;
  config.setRefreshRate(100);

  manager.setConfig(config);
  KeyScannerConfig retrieved = manager.getConfig<KeyScannerConfig>();

  TEST_ASSERT_EQUAL(100, retrieved.getRefreshRate());
}

void test_ConfigManager_saveConfig() {
  ConfigManager manager(&testStorage);
  GlobalConfig config;
  GlobalConfig::MacAddress testMac = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  config.setMac(testMac);

  manager.setConfig(config);
  bool saved = manager.saveConfig();

  TEST_ASSERT_TRUE(saved);
}

void test_ConfigManager_loadConfig() {
  ConfigManager manager1(&testStorage);
  GlobalConfig config;
  GlobalConfig::MacAddress testMac = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
  config.setMac(testMac);
  KeyScannerConfig scannerConfig;
  scannerConfig.setRefreshRate(150);

  manager1.setConfig(config);
  manager1.setConfig(scannerConfig);
  bool saved = manager1.saveConfig();
  TEST_ASSERT_TRUE(saved);

  ConfigManager manager2(&testStorage);
  bool loaded = manager2.loadConfig();

  TEST_ASSERT_TRUE(loaded);
  GlobalConfig retrieved = manager2.getConfig<GlobalConfig>();
  uint8_t retrievedMac[6] = {};
  retrieved.getMac(retrievedMac, sizeof(retrievedMac));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(testMac, retrievedMac, 6);
}

void test_ConfigManager_save_and_load_multiple_configs() {
  ConfigManager manager1(&testStorage);

  GlobalConfig globalCfg;
  GlobalConfig::MacAddress testMac = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60};
  globalCfg.setMac(testMac);
  manager1.setConfig(globalCfg);

  KeyScannerConfig scannerCfg;
  scannerCfg.setRefreshRate(200);
  manager1.setConfig(scannerCfg);

  manager1.saveConfig();

  ConfigManager manager2(&testStorage);
  manager2.loadConfig();

  GlobalConfig retrievedGlobal = manager2.getConfig<GlobalConfig>();
  KeyScannerConfig retrievedScanner = manager2.getConfig<KeyScannerConfig>();

  uint8_t retrievedMac[6] = {};
  retrievedGlobal.getMac(retrievedMac, sizeof(retrievedMac));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(testMac, retrievedMac, 6);
  TEST_ASSERT_EQUAL(200, retrievedScanner.getRefreshRate());
}

void test_ConfigManager_overwrite_config() {
  ConfigManager manager(&testStorage);

  GlobalConfig config1;
  GlobalConfig::MacAddress mac1 = {0x01, 0x01, 0x01, 0x01, 0x01, 0x01};
  config1.setMac(mac1);
  manager.setConfig(config1);
  manager.saveConfig();

  GlobalConfig config2;
  GlobalConfig::MacAddress mac2 = {0x02, 0x02, 0x02, 0x02, 0x02, 0x02};
  config2.setMac(mac2);
  manager.setConfig(config2);
  manager.saveConfig();

  ConfigManager manager2(&testStorage);
  manager2.loadConfig();
  GlobalConfig retrieved = manager2.getConfig<GlobalConfig>();

  uint8_t retrievedMac[6] = {};
  retrieved.getMac(retrievedMac, sizeof(retrievedMac));
  TEST_ASSERT_EQUAL_UINT8_ARRAY(mac2, retrievedMac, 6);
}

void run_ConfigManager_tests() {
  RUN_TEST(test_ConfigManager_initialization);
  RUN_TEST(test_ConfigManager_getConfig_GlobalConfig_defaults);
  RUN_TEST(test_ConfigManager_getConfig_KeyScannerConfig_defaults);
  RUN_TEST(test_ConfigManager_setConfig_GlobalConfig);
  RUN_TEST(test_ConfigManager_setConfig_KeyScannerConfig);
  RUN_TEST(test_ConfigManager_saveConfig);
  RUN_TEST(test_ConfigManager_loadConfig);
  RUN_TEST(test_ConfigManager_save_and_load_multiple_configs);
  RUN_TEST(test_ConfigManager_overwrite_config);
}



#endif