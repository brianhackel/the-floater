# The Floater
A tilt floating hydrometer built on Adafruit Feather Huzzah ESP8266 and MPU-6050

# Operation
<p align="center">
<img src="https://github.com/brianhackel/the-floater/blob/main/docs/diagram.jpg" height="500" />
</p>

## Modes
The Floater has a few differnt modes indicated by the lights.
### 1. Access Point
  The Access Point mode is indicated by both the Blue and Red lights flashing together at 1 second intervals. This is basically the "factory reset" state. At this point, The Floater should be broadcasting its own WiFi signal with an SSID of `The-Floater`. Connecting to this network should direct your device to open a Captive Portal asking for your WiFi name and password. If the Captive Portal does not open up by itself, try opening a web browser and going to any website (like google.com); it should prompt you at that point. Once you have provided your WiFi credentials and clicked The Floater should restart and attempt to enter Configuration mode
### 2. Configuration
  The Configuration mode is indicated by the Blue light flashing slowly at 1 second intervals. At this point, if your device (phone, tablet, computer) is on the same WiFi network as the one configured via the Access Point in (1) above, then you should able to visit `the-floater.local` in your web browser and see the configuration page. Here is where you can check The Floater's status like battery, tilt and temperature and configure the details for how you'd like it log its data.
### 3. Operation / Boot
  During normal operation, The Floater will begin by attempting to connect to the configured WiFi network. During this time the Blue light will flash quickly at 1/2 second intervals. This typically lasts no more than 20 seconds. At the end of which, it will either go into Configuration mode (slow Blue light blinks) or perform a data logging step (solid Blue light for a short time) and then go into sleep mode.
### 4. Sleep
  In between logging operations, The Floater will be in sleep mode. The two green lights for the temperature sensor and tilt sensor (accelerometer) will still be lit solid green, but no other lights will be on. During this time, the device is mostly powered down to save battery, but the internal clock is waiting to wake up for the next logging interval based on teh configuration from (2).
### 5. Standby
  Standby is a special case of Sleep mode that can be achieved from the Configuration page. It basically puts The Floater into a state where it will not wake up and log, but the switch can be left in the ON state for battery charging. Note that the battery WILL NOT CHARGE with the switch in the OFF state, so Standby is needed to allow the device to charge optimally (without drawing much power) without also still logging data.

## Changing Modes
  * Changing from Access Point to Configuration is straightforward - use the Submit button
  * Changing from Configuration to Operation is straightforward - submit a valid configuration via the "Apply and Restart" button
  * Changing from Configuration to Standby is straightforward - use the Enter Standby Mode button on the Configuration page
  * Changing from Configuration to Access Point can be done in two ways:
    1. Use the Reset to Factory button on the Configuration page
    2. Press the PHYSICAL Reset Button (3 in the device diagram) while the Blue light is blinking to indicate Configuration mode
  * Changing from Operation to Configuration:
    1. Press the PHYSICAL Reset Button (3 in the device diagram)
    2. WAIT for the Blue light to begin blinking
    3. AS SOON AS you see the Blue light blinking after pressing the RESET button, PRESS IT AGAIN
    4. WAIT. You should now notice that the device goes from Boot to Configuration mode (Blue Light blinking slowly at once a second)
    5. From here, you should be able to access the Configuration page via `the-floater.local`.
