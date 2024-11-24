# Esp32-C3 OLED Display

Feature:
- clock display
- display weather
- display turns off according to the clock
- Conways Games
- use blynk to control
  
*Pin Configuration
esp32-C3 --> Display OLED SSD1306 0.91 inch 128x32 pixel
Gnd -> Gnd
3.3V -> VCC
SDA -> SDA
SCK -> SCK

*Configuration
- USB CDC On Boot: *Enable
- Upload Speed: 921600 or 115200

Example of openweathermap weather API:
"https://api.openweathermap.org/data/2.5/weather?lat=-6.268895&lon=106.875129&appid=4997ec727dd6b0fbf3246663f400abc1"
  
setTextSize(1):
- Hight: 8 pixels
- Width: 6 pixels per character (depending on the character displayed)

setTextSize(2):
- Height: 16 pixels (2 times the height of text size 1)
- Width: 12 pixels per character (depending on the character)

Unit em
horizontal x vertical

- 1 em = 8 pixels
- 2 em = 16 pixels
- 3 em = 24 pixels
- 4 em = 32 pixels
- 5 em = 40 pixels
- 6 em = 48 pixels
- 7 em = 56 pixels
- 8 em = 64 pixels
- 16 em = 128 pixels
