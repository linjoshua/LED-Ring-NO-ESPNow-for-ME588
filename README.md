# 🔵 LED Ring Controller (Standalone Version)

This is the **non-networked** version of the LED Ring Controller for our robot project. It controls an 8-LED WS2812 ring and supports manual FSM state input through serial communication.

---

## 🧠 FSM State Mapping

| State | Meaning         | LED Behavior          |
|-------|------------------|------------------------|
| 0     | Drive Forward    | Clockwise rotation     |
| 1     | Drive Backward   | Counter-clockwise rot. |
| 2     | Launch Left      | Slow blinking          |
| 3     | Launch Right     | Fast blinking          |
| 4     | Stop             | Constant light         |
| 5     | To Home          | Wipe fill              |
| 6     | At Home          | Comet-style pulse      |

---

## 🧩 Hardware Setup

- **ESP32 DevKit-C**
- **LED Ring (WS2812)** connected to pin **13**
- **Color Select Button** on **GPIO 14**
- **Enable Button** on **GPIO 26**

---

## 🧪 Debug Instructions

Open Serial Monitor (115200 baud) and type any number from **0 to 6** to change the FSM state and observe LED effects.

---

## 🧱 Project Folder Structure

