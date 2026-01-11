# STM32F401 Hardware Wiring Guide

This guide shows how to wire the STM32F401 board for the macropad project.

## 硬件连接指南 (Hardware Connection Guide)

## Components Needed

1. **STM32F401CCU6 Development Board** ("Black Pill" or similar)
2. **WS2812B LED Strip** (12 LEDs or more)
3. **12x Push Button Switches**
4. **1x Rotary Encoder** (with built-in push button)
5. **Resistors**: 12x 10kΩ (for pull-ups if not using internal)
6. **Capacitors**: 
   - 1x 1000µF (for WS2812 power supply)
   - 1x 100nF (near each power pin)
7. **Power Supply**: 5V for WS2812, 3.3V for STM32
8. **Logic Level Shifter** (optional but recommended for WS2812 data line)

## Wiring Diagram

```
STM32F401CCU6 Pin Configuration:
┌─────────────────────────────────────┐
│         STM32F401CCU6               │
│         "Black Pill"                │
│                                     │
│  VBAT ○                      ○ VB   │
│  PC13 ○                      ○ 3V3  │ ──→ 3.3V Power
│  PC14 ○                      ○ GND  │ ──→ Ground
│  PC15 ○                      ○ 5V   │ ──→ 5V Power
│  PA0  ○ ←─ Encoder A         ○ PB9  │
│  PA1  ○ ←─ Encoder B         ○ PB8  │
│  PA2  ○ ←─ Encoder Btn       ○ PB7  │ ──→ Key 8
│  PA3  ○                      ○ PB6  │ ──→ Key 7
│  PA4  ○                      ○ PB5  │ ──→ Key 6
│  PA5  ○                      ○ PB4  │ ──→ Key 5
│  PA6  ○                      ○ PB3  │ ──→ Key 4
│  PA7  ○                      ○ PA15 │
│  PB0  ○ ──→ Key 1            ○ PA12 │ ──→ USB D+
│  PB1  ○ ──→ Key 2            ○ PA11 │ ──→ USB D-
│  PB2  ○ ──→ Key 3            ○ PA10 │
│  PB10 ○ ──→ Key 11           ○ PA9  │
│  PB11 ○ ──→ Key 12           ○ PA8  │ ──→ WS2812 Data
│  RST  ○                      ○ PB15 │
│  3V3  ○                      ○ PB14 │
│  GND  ○                      ○ PB13 │
│  GND  ○                      ○ PB12 │
└─────────────────────────────────────┘

Key 9:  PB8
Key 10: PB9
```

## Detailed Connections

### Power Supply
```
STM32F401:
├─ 3V3 pins ──→ 3.3V power supply
├─ GND pins ──→ Common ground
└─ 5V pin   ──→ 5V input (from USB or external)

WS2812B Strip:
├─ VCC ──→ 5V power supply (separate, with 1000µF capacitor)
├─ GND ──→ Common ground (connected to STM32 ground)
└─ DIN ──→ PA8 (through level shifter recommended)
```

### WS2812B LED Connection
```
STM32 PA8 (3.3V) ──→ [Level Shifter] ──→ WS2812 DIN (5V)
                 ──→ Optional: Direct connection may work
                     but level shifter is recommended

5V Supply ──→ [1000µF Cap] ──→ WS2812 VCC
                         ├──→ WS2812 GND ──→ Common GND
```

**Important**: 
- Add 100-330Ω resistor in series with data line
- Connect large capacitor (1000µF) across WS2812 power supply
- Keep WS2812 data wire short (<30cm without level shifter)

### Push Button Keys (PB0-PB11)
```
Each button connection (12 total):

         3.3V
           │
         [10kΩ]  ← Optional (using internal pull-up)
           │
           ├─────○ STM32 Pin (PB0-PB11)
           │
         [Button]
           │
          GND
```

Connections:
- Key 1:  PB0 ──→ Button ──→ GND
- Key 2:  PB1 ──→ Button ──→ GND
- Key 3:  PB2 ──→ Button ──→ GND
- Key 4:  PB3 ──→ Button ──→ GND
- Key 5:  PB4 ──→ Button ──→ GND
- Key 6:  PB5 ──→ Button ──→ GND
- Key 7:  PB6 ──→ Button ──→ GND
- Key 8:  PB7 ──→ Button ──→ GND
- Key 9:  PB8 ──→ Button ──→ GND
- Key 10: PB9 ──→ Button ──→ GND
- Key 11: PB10 ──→ Button ──→ GND
- Key 12: PB11 ──→ Button ──→ GND

### Rotary Encoder
```
Encoder Connection:

        3.3V
         │
      [10kΩ] [10kΩ] [10kΩ]  ← Optional (using internal pull-up)
         │     │     │
         ├─────┼─────┼───── Common (if encoder has common pin)
         │     │     │
      CLK   DT   SW
       │     │     │
       │     │     └──→ PA2 (Button)
       │     └────────→ PA1 (Encoder B)
       └──────────────→ PA0 (Encoder A)

GND ────────────────→ GND pin of encoder
```

### USB Connection
```
USB Mini/Micro connector:
┌──────┐
│ USB  │
│ Port │
└──┬───┘
   │
   ├─ D+ ──→ PA12
   ├─ D- ──→ PA11
   ├─ VCC ──→ 5V (to board 5V pin)
   └─ GND ──→ GND
```

**Note**: Some STM32F401 boards have USB connector built-in.

## Power Considerations

### Current Requirements
- STM32F401: ~50mA (typical)
- Each WS2812 LED: ~60mA at full white
- 12 LEDs at full white: 720mA
- **Total**: ~800mA minimum

### Recommended Power Supply
1. **USB Only**: OK for testing with dim LEDs
2. **USB + External 5V**: Best for full brightness
   - Connect external 5V to WS2812 VCC
   - Connect USB for STM32 power and programming
3. **External 5V Only**: Use with voltage regulator for 3.3V

### Power Supply Schematic
```
External 5V ──┬──→ [1000µF] ──→ WS2812 VCC
              │
              ├──→ [3.3V Regulator] ──→ STM32 3.3V
              │        (AMS1117-3.3 or similar)
              │
              └──→ Common GND
```

## Level Shifter Circuit (Optional but Recommended)

### Using 74HCT125 or 74HCT245
```
3.3V ──→ VCC (74HCT125)
         │
PA8 ─────┤ Input
         │
         ├──→ Output ──→ WS2812 DIN
         │
5V ──────┤ VCC (output side)
         │
GND ─────┴──→ GND
```

### Simple Diode Method (Alternative)
```
            330Ω
PA8 (3.3V) ──[R]──┬──→ WS2812 DIN
                  │
                 [1N4148]
                  │
              5V ─┴
```

## PCB Layout Recommendations

If designing a PCB:

1. **Keep WS2812 data traces short and direct**
2. **Use wide traces for power (>20mil)**
3. **Add ground plane**
4. **Place decoupling caps close to ICs**
5. **Add test points for debugging**

### Suggested Layer Stackup
```
Top Layer:    Components + Signal traces
Bottom Layer: Ground plane + Power traces
```

## Troubleshooting Hardware

### LEDs not working
- ✓ Check 5V power to WS2812
- ✓ Verify data signal reaches first LED
- ✓ Try adding level shifter
- ✓ Check ground connection

### Keys not responding
- ✓ Verify pull-up resistors (or internal enabled)
- ✓ Test continuity with multimeter
- ✓ Check for shorts

### USB not detected
- ✓ Check D+/D- connections
- ✓ Verify 1.5kΩ pull-up on D+ (usually built-in)
- ✓ Try different USB cable

## Example Build

### Breadboard Layout
```
┌────────────────────────────────────────┐
│  [  Breadboard Top View  ]             │
│                                        │
│  [STM32]  [Buttons x12]  [Encoder]    │
│     │                                  │
│     └──→ [Level Shifter] ──→ [WS2812] │
│                                        │
│  [5V Supply] [Capacitors]              │
└────────────────────────────────────────┘
```

### Component Placement Tips
1. Place STM32 on one side of breadboard
2. Buttons in 3x4 matrix layout
3. Encoder at top or side
4. WS2812 strip along bottom edge
5. Power supply and caps near WS2812

## Safety Notes

⚠️ **Important Safety Information**:

1. **Never exceed 5V on WS2812**
2. **Don't connect 5V directly to STM32 GPIO** (except 5V-tolerant pins)
3. **Always connect grounds together**
4. **Use proper fusing on power supply**
5. **Add reverse polarity protection**

## Bill of Materials (BOM)

| Item | Quantity | Notes |
|------|----------|-------|
| STM32F401CCU6 | 1 | Black Pill or equivalent |
| WS2812B Strip | 1 | 12+ LEDs |
| Push Buttons | 12 | Tactile switches |
| Rotary Encoder | 1 | With push button |
| 10kΩ Resistors | 13 | Optional (if not using internal pull-ups) |
| 330Ω Resistor | 1 | For WS2812 data line |
| 1000µF Capacitor | 1 | For WS2812 power |
| 100nF Capacitors | 5+ | Decoupling |
| Level Shifter | 1 | Optional but recommended |
| USB Cable | 1 | Mini or Micro USB |
| Breadboard/PCB | 1 | For assembly |
| Jumper Wires | As needed | For connections |

Total estimated cost: $10-20 USD (excluding PCB if custom made)
