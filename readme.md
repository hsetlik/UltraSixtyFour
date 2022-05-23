# **Ultra 64**

The Ultra 64 is a powerful hardware step sequencer built to Eurorack standards. It supports 4 concurrent gate/CV track outputs, wireless loading and saving of patterns, and an easy-to-use system of scale quantizing.

- [**Ultra 64**](#ultra-64)
  - [**Controls and features**](#controls-and-features)
    - [**Menu controls**](#menu-controls)
    - [**Track buttons**](#track-buttons)
    - [**Encoders**](#encoders)
    - [**Page buttons**](#page-buttons)
  - [**Hardware**](#hardware)
    - [**Shorcomings of older MCUs and past design challenges**](#shorcomings-of-older-mcus-and-past-design-challenges)
    - [**The brains- ESP32**](#the-brains--esp32)
    - [**Peripheral hardware**](#peripheral-hardware)
  - [**Software**](#software)
    - [**Approach and hardware considerations**](#approach-and-hardware-considerations)
    - [**Develoment environment and dependencies**](#develoment-environment-and-dependencies)
    - [**Interesting/challenging bits**](#interestingchallenging-bits)

## **Controls and features**

My second attempt at a Eurorack sequencer, the Ultra 64's interface is built for easy manipulation of the pattern and quick access to all of the sequencer's features. I set out to avoid menu diving and confusion by relying on a larger number of electromechanical inputs (four rotary encoders and a total of thirteen push buttons), each with its own limited and specific functionality.

### **Menu controls**

The encoder and pair of buttons directly below the display control the saving and loading of patterns.

### **Track buttons**

The vertical column of four buttons next to the output jack are used to select the track to be viewed or edited. When pressed and held, each button will reset its respective track with an empty sequence.

### **Encoders**

The three encoders just above the sequence LEDs are used to navigate and edit the current track's pattern.

- The leftmost knob uses its integrated push button to toggle between two modes; in the first mode the encoder controls the pitch of the selected note while in the second it controls the selected step's gate length.
- The center knob moves the cursor on the sequence LEDs to select the step to be edited. Its push button toggles the selected step on and off.
- The rightmost knob, like the leftmost, has two modes which are toggled by the encoder's push button. By default the encoder controls the tempo of the current sequence, but when toggled into quantize mode, it controls the scale to which the track is being quantized. NOTE: While in quantize mode, the leftmost encoder selects the tonal center of the quantizing scale.
  
### **Page buttons**

The pair of buttons below the output jacks are used to quickly jump around the sequence by shifting the selected step by 16, the length of a "page" subsection of the sequence. Pressing and holding the right page button will copy the contents of the current page to the other three pages, making it easy to extend shorter patterns across a longer sequence without manually entering the pattern numerous times. Pressing and holding the left page button will clear all the steps on the current page.

## **Hardware**

An ESP32 microcontroller development board is at the heart of the Ultra 64. The processing power and feature list of the ESP32 relative to its low cost make it the perfect choice for this project. The full circuit, available in the KiCad folder of this repository, makes use of only a handful of other components and is very straightforward in electrical terms.

### **Shorcomings of older MCUs and past design challenges**

 The [previous incarnation](https://github.com/hsetlik/CircleSixteen) of my digital step sequencer relied on a pair of Arduino Nano boards which are built around the 8-bit Atmega328 microcontroller. While perfectly functional, the Circle Sixteen left much to be desired in the way of capability and hardware control options; the 328 struggles to keep up with polling a large number of encoders and buttons for input while also driving the output and handling the sequence and playback logic. For this reason, the Circle Sixteen used only a handful of electromechanical inputs to control everything, which makes the process of entering and editing sequences tedious and slow. Additionally, the 328's 1024 byte EEPROM means that saving and loading sequences is out of the question without some additional hardware to provide more non-volatile memory.

### **The brains- ESP32**

The problems with the Arduino Nano board described above are non-issues with the ESP32 development board. Considering the [roughly 33x](https://www.makerguides.com/esp32-vs-arduino-speed-comparison/) speed difference between the two microcontrollers, polling the encoders and buttons quickly enough is no longer a limiting factor, so the two I2C-linked microcontroller boards used in the Circle Sixteen can be replaced with a single ESP32. The ESP32's built-in WiFi capability comes as a welcome bonus, and is useful for both over-the-air firmware updates in development and uploading and downloading pattern files to the sequencer. Those pattern files can be stored in the ESP32's 4mb flash memory without any need for additional hardware. The only drawback of the ESP32 is its higher power consumption. The device can pull anywhere from 15mA to 280mA, where an Atmega328 hardly draws 20mA under strain. In the context of a Eurorack system supplied by a wall adapter, drawing 250mA isn't a major issue; but it is worth keeping in mind when considering hardware for battery-powered devices.

### **Peripheral hardware**

The Ultra 64 uses relatively little peripheral hardware, all of which is connected to the ESP32 and used more or less as prescribed by the datasheets. The device uses:

1. 4x Quadrature Rotary encoders with integrated push-buttons.
2. 9x momentary push-buttons, configured in a pair of analog button ladders each connected to an ADC pin on the ESP32.
3. 1x 128x32 monochrome OLED display with an SSD1306 driver IC, connected to the ESP32 over I2C.
4. 24x WS2812b addressable LEDs connected in series and driven by a single digital output pin.
5. 2x MCP4822 2-channel 12-bit DACs, connected via SPI and used to provide the 1v/octave analog voltage to the output amplifier.

## **Software**

### **Approach and hardware considerations**

Having never used an ESP32 before, my assumption was to treat the board more or less like an Arduino with a bit more oomph. As I got more familiar with the device and read up a bit on modern embedded C++, I took an approach more in line with the object-oriented modern C++ paradigm I'm used to from writing C++ for desktop systems. After coming to understand the ESP32 a bit better, I took advantage of dynamic STL containers and other more memory-intensive code, which helped to improve much of the firmware's efficiency and readability by avoiding many of the headaches that come with C-style arrays and non-RAII memory management with the `new` and `delete` keywords.

### **Develoment environment and dependencies**

All this code was written in Visual Studio Code with the [PlatformIO IDE](https://platformio.org/) extension, which handles serial communication, flashing firmware to the microcontroller, and managing devices and bootloaders. PlatformIO also comes with a very powerful package manager and dependency handling system which makes working with C++ libraries uncharacteristically easy. I make a point of limiting dependencies and not relying on a laundry list of 3rd-party libraries. But certain libraries, namely those built to handle basic communication with other hardware like a DAC or an SSD1306 display, are invaluable time savers in embedded C++. A full list of dependencies is available in the `platformio.ini` file in this repository.

### **Interesting/challenging bits**

TODO: talk about analog button handling, JSON patterns, and the pixel animation thing
