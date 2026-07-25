# HK1621B Custom LCD Driver & Reverse-Engineering Guide

A lightweight driver and bit-mapping guide for controlling custom segment LCDs powered by the **HK1621B** (or **HT1621B**) driver chip using an Arduino Nano via a 3-wire serial interface.

---

## 📌 Project Overview

This project focuses on reverse-engineering a custom-designed LCD module with **no datasheet or pinout reference available**. By interfacing an Arduino Nano with the onboard HK1621B chip, the microcontroller offloads the high display refresh load and controls segments directly through memory mapping.

Because the original PCB designer routed the segment traces randomly across the board (e.g., Bit 1 is not Segment A, Bit 2 is not Segment B), every segment bit was manually identified and mapped through step-by-step debugging.

---

## 🔌 Hardware Connections & Pinout

The LCD module uses a 5-pin main connector (`CON1`) alongside a dedicated ground connection for the backlight.

### **Main Header (`CON1`) to Arduino Nano**

| `CON1` Pin | Function | Arduino Nano Pin | Notes |
| :---: | :---: | :---: | :--- |
| **Pin 1** | **VCC** | `5V` | Main power supply |
| **Pin 2** | **DATA** | `D12` | Serial Data line |
| **Pin 3** | **WR** | `D11` | Write Clock signal |
| **Pin 4** | **CS** | `D10` | Chip Select (Active Low) |
| **Pin 5** | **GND** | `GND` | Common Ground |

### 💡 Display Backlight Connection
> ⚠️ **Important:** To turn on the display backlight, connect the dedicated pin located on the side/top edge of the PCB directly to **GND**.

---

## 🧠 Why 4 Bits and Not 8 Bits?

The **HK1621B / HT1621B** memory architecture uses **4-bit RAM addresses**:

1. **4 COM Lines per SEG Pin:** Each RAM address controls one `SEG` output pin across 4 multiplexed common lines (`COM0`–`COM3`). Therefore, each RAM location holds exactly **4 bits of data** (`D0`–`D3`).
2. **2 Addresses per 7-Segment Digit:** A complete 7-segment digit (including the decimal point) requires **8 controllable segments** (A, B, C, D, E, F, G, DP).
3. **Nibble Pairing:** Because one RAM address only provides 4 bits, each digit on the display spans **2 RAM addresses** (4 bits + 4 bits = 8 bits).

In the code, this is handled using a pair of 4-bit nibbles:

```cpp
// HK1621B RAM cells are 4 bits wide. 
// n0 and n1 hold 4 bits each across 2 RAM addresses to drive 8 total segments.
struct SegPair { 
    uint8_t n0; // First 4-bit nibble  (SEG x)
    uint8_t n1; // Second 4-bit nibble (SEG x+1)
};
```
Reverse-Engineering the Bitmap
Since the PCB traces do not follow a standard segment ordering, the display array was built through manual bit-mapping:

Write Protocol: Commands send a 3-bit Identifier (0b101), a 6-bit RAM Address, and 4 bits of Data.

Bit Decoding: Each bit in n0 and n1 was tested individually to determine which physical segment (A through G) lit up on the display.

Lookup Table: Once isolated, these bit combinations were compiled into the DIGIT[10] array to reliably output numbers 0 through 9.
