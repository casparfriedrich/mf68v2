# MF68v2

Custom Magicforce 68 keyboard firmware. Version 2 is based on the the [Zephyr Project](https://www.zephyrproject.org).

## Prepare

```bash
west init --mr v2.2.1
west update
```

## Build

```bash
cmake -B build -G Ninja .
cmake --build build --target all
```

## Pin configuration

### Row pin configuration (outputs)

|                      |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |
|----------------------|-----|-----|-----|-----|-----|-----|-----|-----|
| Arduino              | D10 | D11 | D12 | D13 | A0  | A1  | A2  | A3  |
| SAMD21 Mini Breakout | B6  | B2  | B3  | B1  | F7  | F6  | F5  | F4  |
| SAMD21               | A18 | A16 | A19 | A17 | A02 | B08 | B09 | A04 |

### Column pin configuration (inputs)

|                      |  0  |  1  |  2  |  3  |  4  |  5  |  6  |  7  |  8  |
|----------------------|-----|-----|-----|-----|-----|-----|-----|-----|-----|
| Arduino              | D1  | D0  | D2  | D3  | D4  | D5  | D6  | D7  | D8  |
| SAMD21 Mini Breakout | D3  | D2  | D1  | D0  | D4  | C6  | D7  | E6  | B4  |
| SAMD21               | A10 | A11 | A14 | A09 | A08 | A15 | A20 | A21 | A06 |

### Other pins

- Backlight control (SAMD21: PA07, Arduino: D9), active High.
- Onboard LEDs (marked as 'TX' (PA27) and 'RX' (PB03)), active low
- Button (SAMD21: PA03, Pro Micro: AREF)

## Setup

```bash
west init
west update  -f always
```

## Build

```bash
source ./zephyr/zephyr-env.sh
cmake -B build -G Ninja .
cmake --build build --target all
```

## Flash

```bash
cmake --build build --target flash
```

## Todo

### N-Key roll over

https://github.com/qmk/qmk_firmware/blob/master/docs/usb_nkro.txt

```C
const u8_t REPORT_BYTES = 68;

const u8_t report_descriptor_nkro[] = {
    0x05, 0x01,  // Usage Page (Generic Desktop),
    0x09, 0x06,  // Usage (Keyboard),
    0xA1, 0x01,  // Collection (Application),
    // bitmap of modifiers
    0x75, 0x01,  //   Report Size (1),
    0x95, 0x08,  //   Report Count (8),
    0x05, 0x07,  //   Usage Page (Key Codes),
    0x19, 0xE0,  //   Usage Minimum (224),
    0x29, 0xE7,  //   Usage Maximum (231),
    0x15, 0x00,  //   Logical Minimum (0),
    0x25, 0x01,  //   Logical Maximum (1),
    0x81, 0x02,  //   Input (Data, Variable, Absolute), ;Modifier byte
    // LED output report
    0x95, 0x05,  //   Report Count (5),
    0x75, 0x01,  //   Report Size (1),
    0x05, 0x08,  //   Usage Page (LEDs),
    0x19, 0x01,  //   Usage Minimum (1),
    0x29, 0x05,  //   Usage Maximum (5),
    0x91, 0x02,  //   Output (Data, Variable, Absolute),
    0x95, 0x01,  //   Report Count (1),
    0x75, 0x03,  //   Report Size (3),
    0x91, 0x03,  //   Output (Constant),
    // bitmap of keys
    0x95, (REPORT_BYTES - 1) * 8,      //   Report Count (),
    0x75, 0x01,                        //   Report Size (1),
    0x15, 0x00,                        //   Logical Minimum (0),
    0x25, 0x01,                        //   Logical Maximum(1),
    0x05, 0x07,                        //   Usage Page (Key Codes),
    0x19, 0x00,                        //   Usage Minimum (0),
    0x29, (REPORT_BYTES - 1) * 8 - 1,  //   Usage Maximum (),
    0x81, 0x02,                        //   Input (Data, Variable, Absolute),
    0xc0                               // End Collection
};
```

https://www.microchip.com/forums/m368006.aspx

```C
//Class specific descriptor - HID mouse
ROM struct{BYTE report[HID_RPT01_SIZE];}hid_rpt01={
{ 0x05, 0x01, // USAGE_PAGE (Generic Desktop)
0x09, 0x06, // USAGE (Keyboard)
0xa1, 0x01, // COLLECTION (Application)
0x05, 0x07, // USAGE_PAGE (Keyboard)
0x19, 0xe0, // USAGE_MINIMUM (Keyboard LeftControl)
0x29, 0xe7, // USAGE_MAXIMUM (Keyboard Right GUI)
0x15, 0x00, // LOGICAL_MINIMUM (0)
0x25, 0x01, // LOGICAL_MAXIMUM (1)
0x75, 0x01, // REPORT_SIZE (1)
0x95, 0x08, // REPORT_COUNT (8)
0x81, 0x02, // INPUT (Data,Var,Abs)
0x95, 0x01, // REPORT_COUNT (1)
0x75, 0x08, // REPORT_SIZE (8)
0x81, 0x03, // INPUT (Cnst,Var,Abs)
0x95, 0x05, // REPORT_COUNT (5)
0x75, 0x01, // REPORT_SIZE (1)
0x05, 0x08, // USAGE_PAGE (LEDs)
0x19, 0x01, // USAGE_MINIMUM (Num Lock)
0x29, 0x05, // USAGE_MAXIMUM (Kana)
0x91, 0x02, // OUTPUT (Data,Var,Abs)
0x95, 0x01, // REPORT_COUNT (1)
0x75, 0x03, // REPORT_SIZE (3)
0x91, 0x03, // OUTPUT (Cnst,Var,Abs)
0x95, 0x1E, // REPORT_COUNT (30)
0x75, 0x08, // REPORT_SIZE (8)
0x15, 0x00, // LOGICAL_MINIMUM (0)
0x25, 0x65, // LOGICAL_MAXIMUM (101)
0x05, 0x07, // USAGE_PAGE (Keyboard)
0x19, 0x00, // USAGE_MINIMUM (Reserved (no event indicated))
0x29, 0x65, // USAGE_MAXIMUM (Keyboard Application)
0x81, 0x00, // INPUT (Data,Ary,Abs)
0xc0 }
```
