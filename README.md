# Transportation Band – ESP32 + Blynk

Control system for a conveyor belt using **ESP32** and **Blynk**.  
It manages two conveyor belts (Band 1 and Band 2), a transfer mechanism ("Palanca"), and multiple position sensors.  
Includes **automatic mode**, **calibration mode**, and **fast mode**, all controlled from the Blynk mobile app.

---

## 1. Features

- ESP32-based control system
- Remote control using Blynk
- Two independent conveyor belts
- Transfer mechanism for object movement
- Real-time sensor visualization using virtual LEDs
- Automatic operation mode
- Calibration mode for system testing
- Fast and slow operation modes
- Integrated event logging using Blynk Terminal

---

## 2. Hardware Requirements

- ESP32 development board  
- 3 DC motors:
  - Conveyor Band 1
  - Conveyor Band 2
  - Transfer mechanism (Palanca)
- 3 H-Bridge motor drivers
- 4 digital sensors:
  - `SIND_Cubo`
  - `SIND_A0`
  - `SIND_A1`
  - `SIND_Cubo2`
- WiFi connection
- Power supply for motors and ESP32

---

## 3. Pin Configuration

### Motors
| Component | Enable | IN1 | IN2 |
|----------|--------|-----|-----|
| Band 1 | E12 (27) | I1 (17) | I2 (18) |
| Band 2 | E34 (19) | I3 (21) | I4 (22) |
| Palanca | E56 (23) | I5 (25) | I6 (26) |

### Sensors
| Sensor | GPIO |
|--------|------|
| SIND_Cubo | 33 |
| SIND_A0 | 32 |
| SIND_A1 | 34 |
| SIND_Cubo2 | 35 |

---

## 4. Blynk Virtual Pins

| Virtual Pin | Function |
|-------------|----------|
| V0 | Start / Pause System |
| V1 | LED Band 1 |
| V2 | LED Band 2 |
| V3 | LED Cubo 2 |
| V4 | LED Cubo |
| V5 | LED A0 |
| V6 | LED A1 |
| V7 | Calibration Mode |
| V8 | Fast Mode |
| V10 | Terminal Log |

---

## 5. Operating Modes

### Automatic Mode
- Detects objects using sensors
- Activates conveyors automatically
- Moves objects using the transfer system
- Sends them to the destination

### Calibration Mode
- Sequentially tests:
  - Band 1
  - Band 2
  - Transfer system forward
  - Transfer system backward
- Automatically exits when completed

### Fast Mode
- Skips waiting state after returning the transfer system
- Restarts cycle immediately

---

## 6. System Logic Flow

1. Object detected on Band 1
2. Band 2 starts
3. Transfer system moves object forward
4. Transfer system retracts
5. Object is transported to destination
6. Cycle restarts automatically

---

## 7. Software Requirements

- Arduino IDE
- ESP32 Board Package
- Blynk Library
- WiFi Library

---

## 8. Installation and Setup

1. Install **Arduino IDE**
2. Install **ESP32 Board Manager**
3. Install required libraries:
   - `Blynk`
   - `WiFi`
4. Configure your WiFi credentials:
   ```cpp
   char ssid[] = "YOUR_WIFI";
   char pass[] = "YOUR_PASSWORD";

5.	Configure your Blynk credentials:
6.	Upload the code to the ESP32
7.	Open the Blynk App and configure the widgets
