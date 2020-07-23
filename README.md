# PSoC_ADS1x15

Driver for TI's ADS1015 and ADS1115: 12 and 16-bit Differential or Single-Ended ADC with PGA and Comparator in PSoC Creator. Add the Header file and the Source File for the respective section in your project on the 'Workspace Explorer'. 

PSoC has internal ADC that can perform ADC, this library is aimed for project where an external ADC is required to send the digital data to PSoC, this may be due to the long distance between the actual sensor of the analog signal and the location of the PSoC or other reason. 

Since C is not a flexible langauge, there is no classes, so this library does not work for multiple ADS setup. If you are working on a project with multiple ADS with PSoC. It is better for you to copy and rename the files and add an extension number similar to how PSoC Creator build a new module for the project. For example: ADS1115_1, ADS1115_2...etc

Also, the I2C communication used in this libaray assume the module is named I2C_1 in PSoC. Please change the name accordingly for your project. 

You can also use the library for ADS1015 but using to Adafruit_ADS1105 initial the parameters. 

Read Info below to find out more about the product. 

## Info

This family of ADCs provide 4 single-ended or 2 differential channels.
Each has a programmable gain amplifier from 2/3 up to 16x. Available
in 12 or 16 bit versions:

* [ADS1015 12-bit ADC](https://www.adafruit.com/product/1083)
* [ADS1115 16-bit ADC](https://www.adafruit.com/product/1085)

The chip's fairly small so it comes on a breakout board with ferrites to keep the AVDD and AGND quiet. Interfacing is done via I2C. The address can be changed to one of four options (see the datasheet table 5) so you can have up to 4 ADS1x15's connected on a single 2-wire I2C bus for 16 single ended inputs.

Adafruit invests time and resources providing this open source code, please
support Adafruit and open-source hardware by purchasing products from
[Adafruit](https://www.adafruit.com)!

## Original Cpp Library

* [Adafruit_ADS1X15 ADC](https://github.com/adafruit/Adafruit_ADS1X15)

## License

BSD license, all text above must be included in any redistribution.
