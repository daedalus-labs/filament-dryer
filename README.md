# MQTT Filament Dryer

This project is Raspberry Pi Pico-based dryer for Filament for use with 3D Printers that provides feedback and command/control through MQTT.

This project does not have any external dependencies, but does require the [Pico C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html#sdk-setup)
to build. The [picotool](https://github.com/raspberrypi/picotool) project is required for installing configuration data.

## Build

In order to build, the `PICO_SDK_PATH` environment variable must be set. Once that is set, the `build.bash` script provides
a single command build interface, which will build using all default build options:

```bash
./build.bash --build
```

The `build.bash` script will forward CMake args, so build options can be modified using `-D{OPTION}={VALUE}`. For
example, the command below will build this project with such that the temperature sensor will read data from `GP12`.

```bash
./build.bash --build -DDHT_DATA_PIN=12
```

### Available Build Options

| Option              | Default Value | Description                                                                                          |
| ------------------- | ------------- | ---------------------------------------------------------------------------------------------------- |
| MQTT_PORT           | 1883          | The TCP/IP Port to use for MQTT communication                                                        |
| DHT_FEEDBACK_PIN    | 254           | The GPIO pin of the DHT Feedback LED. The LED will be `ON` when reading data, `OFF` otherwise        |
| DHT_DATA_PIN        | 17            | The GPIO pin of the DHT Temperature Sensor.                                                          |
| HEATER_FEEDBACK_PIN | 15            | The GPIO pin of the Heater Feedback LED. It is `ON` when the heater is on, `OFF` otherwise           |
| HEATER_CONTROL_PIN  | 19            | The GPIO pin of the Heater Relay.                                                                    |
| SYSTEM_LED_PIN      | 13            | The GPIO pin of the System LED. It is `ON` when the Pico has booted and is running, `OFF` otherwise  |
| MQTT_FEEDBACK_PIN   | 14            | The GPIO pin of the MQTT Feedback LED. It is `ON` when connected to the MQTT broker, `OFF` otherwise |

The LED behaviors of `DHT_FEEDBACK_PIN`, `SYSTEM_LED_PIN`, `MQTT_FEEDBACK_PIN`, and `HEATER_FEEDBACK_PIN` can all be disabled by setting that value
to a value larger than `NUM_BANK0_GPIOS`. A default value of `254` means that LED is not used by default.

### Cleaning

The `build.bash` script also provides an option to clean out all build artifacts via `--clean`:

```bash
./build.bash --clean
```

## Installation

After building the project, the `filament-dryer.uf2` file can be copied to the Pico as described in the [fine manuals for the Pico](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html#your-first-binaries).

Additional documentation on wiring setup and a pre-designed enclosure is available [here](docs/README.md).

### Configuration

Configuration data is loaded onto the Raspberry Pi at run-time using the `load.py` script, which will encode the data into
a binary blob and load in the Pico's flash. The Pico should be hooked up to the computer before running the script.

```bash
./load.py --ssid MyAwesomeWifi --passphrase DontHackMyNetwork --device daryl --broker wilson
```

| Option                 | Description                                                    |
| ---------------------- | -------------------------------------------------------------- |
| `--ssid` or `-s`       | The SSID of the Wireless Network to use for MQTT communication |
| `--passphrase` or `-p` | The passphrase of the Wireless network                         |
| `--device` or `-d`     | The device name to use for MQTT communication                  |
| `--broker` or `-b`     | The hostname or IP address of the MQTT Broker to use           |

### Monitoring via MQTT

The dryer publishes a collection fo MQTT topics for monitoring the status of the dryer. All topics are relative
to the device name (i.e. if the device is named `daryl`, the container humidity will be available on `daryl/container/humidity`).

| Topic                          | Description                                                                 | Data Type |
| ------------------------------ | --------------------------------------------------------------------------- | --------- |
| `container/humidity`           | The current humidity within the container, as a percentage.                 | Float     |
| `container/temperature`        | The current temperature within the container, in degrees Celsius.           | Float     |
| `container/target_temperature` | The desired temperature within the container, in degrees Celsius.           | Float     |
| `container/heater`             | The current state of the heater. Will be "on" if it is on, "off" otherwise. | String    |

The dryer subscribes to the following MQTT topics for command/control of the dryer:

| Topic                              | Description                                                            | Data Type |
| ---------------------------------- | ---------------------------------------------------------------------- | --------- |
| `container/target_temperature/set` | Sets the desired temperature within the container, in degrees Celsius. | Float     |

Finally, there are some MQTT topics that provide metadata on the device status:

| Topic               | Description                                              | Data Type |
| ------------------- | -------------------------------------------------------- | --------- |
| `board/temperature` | The current temperature of the Pico, in degrees Celsius. | Float     |
| `version`           | The version of software running on the Pico.             | String    |
| `uid`               | The UID of the Pico.                                     | String    |

It should be noted that the MQTT interface will require all data be encoded as a string; the `Data Type` column above


## Credits and Thanks

* [Hermann-SW](https://gist.github.com/Hermann-SW) for sharing guides on how to use the Raspberry Pi Pico's flash memory on the [Raspberry Pi Forums](https://forums.raspberrypi.com//viewtopic.php?f=145&t=300146).
* [Erich Styger](https://github.com/ErichStyger) for a really great breakdown of using [MQTT in the lwIP stack](https://mcuoneclipse.com/2023/02/11/using-mqtt-with-the-raspberry-pi-pico-w-and-homeassistant-for-an-optimized-solar-energy-electrical-vehicle-charger/).
