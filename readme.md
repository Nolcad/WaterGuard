# 💧 Arduino Leak Detection and Flow Monitoring System

This project implements a leak detection and abnormal flow monitoring system using an Arduino-compatible microcontroller. It's designed for long-term monitoring of water or fluid flow via a pulse-generating flow sensor (e.g., a water meter). Visual indicators and serial logs provide feedback on flow status and alarm conditions.

## 📋 Features

- **Leak Detection**: Triggers an alarm if no extended inactivity is detected during a monitoring period.
- **Abnormal Flow Alarm**: Triggers if flow persists continuously beyond a defined threshold.
- **Reset Button**: Manually resets the monitoring window and clears alarms.
- **LED Indicators**:
  - `LED_PULSE`: On when flow is active (pulse detected).
  - `LED_ALARM`:
    - **Blinks** for abnormal flow
    - **Stays solid** for leaks
  - `LED_BUILTIN`: Heartbeat indicator (blinks periodically).
- **Serial Debugging**: Prints real-time status and timer values.

## ⚙️ Hardware Requirements

- 1× Arduino-compatible board (e.g. ESP8266/ESP32)
- 1× Flow sensor with pulse output (e.g. Hall-effect water meter)
- 2× LEDs with resistors
- 1× Push button (active LOW)
- Jumper wires, breadboard or PCB

## 🧠 Default Configuration

| Parameter                    | Value            | Description                                 |
|-----------------------------|------------------|---------------------------------------------|
| `MONITORING_PERIOD`         | 24 hours         | Time window to detect leak behavior         |
| `REQUIRED_INACTIVITY_TIME`  | 2 hours          | Minimum flow pause to avoid leak alarm      |
| `ABNORMAL_ACTIVITY_TIME`    | 2 hours          | Max continuous flow duration before alarm   |
| `MINIMAL_ACTIVITY_TIME`     | 1 minute         | Timeout to consider flow as inactive        |
| `LED_BLINK_PERIOD`          | 250 ms           | Blink interval for built-in and alarm LEDs  |

*Testing values are available in comments for quicker iterations.*

## 🔌 Pin Configuration

| Signal         | Pin  |
|----------------|------|
| `LED_PULSE`    | D1   |
| `LED_ALARM`    | D2   |
| `BUTTON_N`     | D6   |
| `PULSE_N`      | D7   |
| `LED_BUILTIN`  | Built-in LED (typically GPIO2 or 13) |

> All inputs use `INPUT_PULLUP` configuration. Active state is LOW.

## 🚀 Getting Started

1. **Clone the repo**:
   ```bash
   git clone https://github.com/Nolcad/WaterGuard.git
   cd WaterGuard

2. **Open in Arduino IDE** or your preferred environment (e.g. PlatformIO).

3. **Upload** the sketch to your board.

4. **Monitor** the serial output at 115200 baud for debugging info.

## 📷 Example Use Case

Place this system on a household or industrial water pipe with a flow sensor. Use it to detect:

- Undetected leaks when nobody is using water.
- Abnormal long-duration water use (e.g., a stuck valve or malfunction).

## 🛠️ Customization

You can change monitoring durations by editing the #define values at the top of the file.

For example:

```cpp
#define MONITORING_PERIOD (6 * 60 * 60) // 6 hours
```

## 🧪 Testing Tips

- Temporarily reduce constants (e.g. to 30 seconds or 1 minute) during development.
- Use a pushbutton to simulate pulses or test the reset behavior.

## 📄 License

This project is licensed under the MIT License. See the LICENSE file for details.

## 🙋‍♂️ Contributions

Pull requests are welcome! Feel free to open issues for improvements, suggestions, or bug reports.

---

**Version**: `v1.0`  
**Author**: *Nolcad*
