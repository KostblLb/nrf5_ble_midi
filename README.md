# BLE-MIDI Demonstration Application for NRF52832

1. Get an NRF52 SDK v17 from https://www.nordicsemi.com/Software-and-tools/Software/nRF5-SDK/Download
2. Clone this repository into `%nrf_path%/examples/ble_peripheral`
3. Copy the contents of `ble_app_blinky` folder into the cloned repo (this is a temporary solution as i don't know yet how to initialize an nrf52 project)
4. Open a project file for your IDE and add ble_midi folder to the compilation 
5. Build and flash 

## Testing on iOS GarageBand
 1. Connect your nrf52 board as a MIDI peripheral from the app's menu
 2. Open a piano
 3. Try pushing some buttons on the devkit, GarageBand should receive input and play sounds
