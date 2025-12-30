#include <submodules/ConfigManager/KeyScannerConfig.h>

void KeyScannerConfig::setPins(uint8_t *rowPinData, uint8_t rowSize,
                               uint8_t *colPinData, uint8_t colSize) {
  rowPins.assign(rowPinData, rowPinData + rowSize);
  rows = rowSize;
  colPins.assign(colPinData, colPinData + colSize);
  cols = colSize;
  bitMapSize = (rows * cols + 7) / 8;
}

void KeyScannerConfig::setRefreshRate(uint16_t rate) {
  if (rate < MIN_REFRESH_RATE || rate > MAX_REFRESH_RATE)
    return;
  refreshRate = rate;
}

void KeyScannerConfig::setBitMapSendInterval(uint16_t rateDivisor) {
  if (MIN_BITMAP_INTERVAL < 2 || rateDivisor > MAX_BITMAP_INTERVAL)
    // Limited to a range of 2-5000, to ensure bitmaps aren't sent every
    // single loop (as this would most likely block the ESP communication),
    // and 5000 to ensure we don't exceed the uint16_t limit and also stay
    // within reasonable bounds for regular bitMap refreshes
    return;
  bitMapSendInterval = rateDivisor;
}

void KeyScannerConfig::setConfig(KeyCfgParams config) {
  setPins(config.rowPins, config.rows, config.colPins, config.cols);
  setRefreshRate(config.refreshRate);
  setBitMapSendInterval(config.bitMapSendInterval);
}
