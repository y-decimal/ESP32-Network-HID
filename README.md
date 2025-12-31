# WIP - This project is still in early developement and is as of yet unfinished and not in a working state!

## ESP32 Based Modular Wireless HID Input Framework

This project aims to create a modular and expandable framework for creating small "networks" of human input devices.
The entire network's state is relayed through an orchestrator device as HID inputs, either through USB or BLE. 
Any Device can act as an orchestrator, while simultaneously being part of a hardware input itself. Think two wireless keyboards, and one of the keyboards also relays their combined key inputs via BLE to the end device.
Or a keyboard and mouse, with a third device working as a wired dongle for low latency, with both the keyboard and the mouse optionally being able to run as independent BLE devices.
