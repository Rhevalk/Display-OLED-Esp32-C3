/* ==============================================================================
Read me

- USB CDC On Boot : *Enable

(setTextSize(1)):
Tinggi: 8 piksel
Lebar: 6 piksel per karakter (tergantung karakter yang ditampilkan)

(setTextSize(2)):
Tinggi: 16 piksel (2 kali tinggi ukuran teks 1)
Lebar: 12 piksel per karakter (tergantung pada karakter)

Satuan em
horizontal x vertical

1 em = 8 pixel
2 em = 16 pixel
3 em = 24 pixel
4 em  = 32 pixel
5 em = 40 pixel
6 em = 48 pixel
7 em = 56 pixel
8 em = 64 pixel
16 em = 128 pixel
============================================================================== */

#define BLYNK_TEMPLATE_ID "TMPL6rjyF2oiJ"
#define BLYNK_TEMPLATE_NAME "Control"
#define BLYNK_AUTH_TOKEN "k-nVQRiaQOetfuxm6yzoKMS5Ggicq_B1"

#include <WiFi.h>
#include <Wire.h>
#include <WiFiUdp.h>
#include <Arduino.h> 
#include <NTPClient.h>
#include <HTTPClient.h>
#include <WiFiClient.h> 
#include <Arduino_JSON.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h> 
#include <BlynkSimpleEsp32.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// API
const char* json;
JSONVar myObject;
String jsonBuffer;     // Menyimpan respons JSON dari server
String serverPath;     // Menyimpan URL server API
String API_keys = "https://api.openweathermap.org/data/2.5/weather?lat=-6.268895&lon=106.875129&appid=4997ec727dd6b0fbf3246663f400abc1";

// cuaca
String cuaca;
String mainWeather;
unsigned long lastTime = 0;
unsigned long interval = 10000;
int suhu, kelembapan, suhuTerasa, awan, angin;

// waktu
WiFiUDP ntpUDP;
int jam, menit;
char jamStr[3], menitStr[3], detikStr[3];
NTPClient timeClient(ntpUDP, "time.google.com", 3600 * 7, 60000);  // 7 jam offset, update setiap 6 detik

// wifi
const char* ssid = "Banyak makan"; 
const char* password = "rhevan1119";

//const char* ssid = "Titik Akses Nirkabel"; 
//const char* password = "DP1270HK3011gY"; 

// display
byte siklus = 0;
bool displayActive = false;
const long timerWaktu = 1000;
unsigned long waktuSebelum = 0; 
unsigned long waktuUpdateWaktu = 0;
int hourDisplayOff = 22;
int minuteDisplayOff = 0;

int hourDisplayOn = 4;
int minuteDisplayOn = 0;

int intervalDisplayOut = 8000;
#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 32   
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // Reset pin is -1 if not used

// game of life
#define GRID_HEIGHT 128
#define GRID_WIDTH 32

int grid[GRID_HEIGHT][GRID_WIDTH];
int newGrid[GRID_HEIGHT][GRID_WIDTH];

// Bylnk
WidgetTerminal terminal(V0);

// 'cerah', 31x32px
const unsigned char cerah [] PROGMEM = {
	0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x61, 0x0c, 0x00, 0x00, 0x61, 0x0c, 0x00, 
	0x08, 0x61, 0x0c, 0x20, 0x08, 0x61, 0x0c, 0x20, 0x02, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1f, 0xf0, 0x00, 0x38, 0x7f, 0xfc, 0x38, 0x38, 0x7f, 0xfc, 0x38, 0x01, 0xff, 0xff, 0x00, 
	0x01, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 0xf9, 0xff, 0xff, 0x3e, 
	0xf9, 0xff, 0xff, 0x3e, 0x01, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 0x01, 0xff, 0xff, 0x00, 
	0x01, 0xff, 0xff, 0x00, 0x38, 0x7f, 0xfc, 0x38, 0x38, 0x7f, 0xfc, 0x38, 0x00, 0x1f, 0xf0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x80, 0x08, 0x61, 0x0c, 0x20, 0x08, 0x61, 0x0c, 0x20, 
	0x00, 0x61, 0x0c, 0x00, 0x00, 0x61, 0x0c, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00
};
// 'awan', 32x32px
const unsigned char awanlogo [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 
	0x00, 0x3f, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xcf, 0x00, 
	0x00, 0xff, 0xcf, 0x00, 0x3f, 0x3f, 0x3f, 0xc0, 0x3f, 0x3f, 0x3f, 0xc0, 0xff, 0xcf, 0xff, 0xc0, 
	0xff, 0xcf, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x3c, 0xff, 0xff, 0xff, 0x3c, 0xcf, 0xff, 0xff, 0xff, 
	0xcf, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xfc, 
	0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'gerimis', 32x32px
const unsigned char gerimis [] PROGMEM = {
	0x00, 0x3f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xc0, 0x00, 
	0x00, 0xff, 0xcf, 0x00, 0x00, 0xff, 0xcf, 0x00, 0x3f, 0x3f, 0x3f, 0xc0, 0x3f, 0x3f, 0x3f, 0xc0, 
	0xff, 0xcf, 0xff, 0xc0, 0xff, 0xcf, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x3c, 0xff, 0xff, 0xff, 0x3c, 
	0xcf, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 
	0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x19, 0x99, 0x99, 0x98, 0x19, 0x99, 0x99, 0x98, 
	0x01, 0x81, 0x81, 0x80, 0x01, 0x81, 0x81, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'hujan', 32x32px
const unsigned char hujan [] PROGMEM = {
	0x00, 0x3f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xc0, 0x00, 
	0x00, 0xff, 0xcf, 0x00, 0x00, 0xff, 0xcf, 0x00, 0x3f, 0x3f, 0x3f, 0xc0, 0x3f, 0x3f, 0x3f, 0xc0, 
	0xff, 0xcf, 0xff, 0xc0, 0xff, 0xcf, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x3c, 0xff, 0xff, 0xff, 0x3c, 
	0xcf, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 
	0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0x00, 0xc0, 0x30, 0x03, 0x00, 0xc0, 0x30, 0x0c, 0x03, 0x00, 0xc0, 0x0c, 0x03, 0x00, 0xc0, 
	0x30, 0xcc, 0x33, 0x0c, 0x30, 0xcc, 0x33, 0x0c, 0x03, 0x00, 0xc0, 0x30, 0x03, 0x00, 0xc0, 0x30, 
	0x0c, 0x03, 0x00, 0xc0, 0x0c, 0x03, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'hujanPetir', 32x32px
const unsigned char hujanPetir [] PROGMEM = {
	0x00, 0x3f, 0x00, 0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xc0, 0x00, 
	0x00, 0xff, 0xcf, 0x00, 0x00, 0xff, 0xcf, 0x00, 0x3f, 0x3f, 0x3f, 0xc0, 0x3f, 0x3f, 0x3f, 0xc0, 
	0xff, 0xcf, 0xff, 0xc0, 0xff, 0xcf, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x3c, 0xff, 0xff, 0xff, 0x3c, 
	0xcf, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 
	0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x03, 0x00, 0xf0, 0xc0, 0x03, 0x00, 0xf0, 0xc0, 0x0c, 0x03, 0xc3, 0x00, 0x0c, 0x03, 0xc3, 0x00, 
	0x00, 0xcf, 0xf0, 0x30, 0x00, 0xcf, 0xf0, 0x30, 0x03, 0x00, 0xc0, 0xc0, 0x03, 0x00, 0xc0, 0xc0, 
	0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
// 'kabut', 32x32px
const unsigned char kabut [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x09, 0x20, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x07, 0x80, 0x00, 0x00, 0x6f, 0xec, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x0f, 0xe0, 
	0x00, 0x00, 0x0f, 0xe0, 0x00, 0x3f, 0x0f, 0xe0, 0x00, 0x3f, 0x6f, 0xec, 0x00, 0xff, 0xc7, 0x80, 
	0x00, 0xff, 0xc0, 0x00, 0x00, 0xff, 0xcf, 0x20, 0x00, 0xff, 0xcf, 0x00, 0x3f, 0x3f, 0x3f, 0xc0, 
	0x3f, 0x3f, 0x3f, 0xc0, 0xff, 0xcf, 0xff, 0xc0, 0xff, 0xcf, 0xff, 0xc0, 0xff, 0xff, 0xff, 0x3c, 
	0xff, 0xff, 0xff, 0x3c, 0xcf, 0xff, 0xff, 0xff, 0xcf, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 
	0xf0, 0xff, 0xff, 0xff, 0x3f, 0xff, 0xff, 0xfc, 0x3f, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'cld', 8x8px
const unsigned char cld [] PROGMEM = {
	0x00, 0x18, 0x3c, 0x7f, 0xff, 0xff, 0xfe, 0x00
};
// 'hum', 8x8px
const unsigned char hum [] PROGMEM = {
	0x10, 0x18, 0x3c, 0x7c, 0x7e, 0x7e, 0x7c, 0x3c
};
// 'tem', 8x8px
const unsigned char tem [] PROGMEM = {
	0x3c, 0x34, 0x34, 0x34, 0x7e, 0x7e, 0x7e, 0x3c
};
// 'w 1', 8x8px
const unsigned char wid [] PROGMEM = {
	0x7e, 0x00, 0x00, 0x00, 0x7f, 0x7f, 0x00, 0x00
};

// 'Android Medium - 1', 32x32px
const unsigned char logo1 [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x70, 0x00, 0x00, 0x00, 0x90, 0x00, 
	0x00, 0x01, 0x18, 0x00, 0x00, 0x02, 0x18, 0x00, 0x00, 0x04, 0x18, 0x00, 0x00, 0x0c, 0x14, 0x00, 
	0x00, 0x08, 0x14, 0x00, 0x00, 0x10, 0x12, 0x00, 0x00, 0x20, 0x12, 0x00, 0x00, 0x40, 0x08, 0x00, 
	0x00, 0x80, 0x09, 0x00, 0x01, 0x00, 0x09, 0x00, 0x02, 0x00, 0x08, 0x80, 0x04, 0x00, 0x08, 0x80, 
	0x0c, 0x00, 0x08, 0x40, 0x08, 0x00, 0x08, 0x40, 0x10, 0x00, 0x08, 0x40, 0x20, 0x00, 0x04, 0x20, 
	0x60, 0x00, 0x04, 0x20, 0x7f, 0xff, 0xfc, 0x10, 0x0e, 0x00, 0x06, 0x10, 0x01, 0xc0, 0x03, 0x10, 
	0x00, 0x38, 0x01, 0x88, 0x00, 0x07, 0x00, 0x48, 0x00, 0x00, 0xe0, 0x24, 0x00, 0x00, 0x1c, 0x14, 
	0x00, 0x00, 0x03, 0x8c, 0x00, 0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00
};
// 'Android Medium - 2', 26x12px
const unsigned char logo2 [] PROGMEM = {
	0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0x01, 0x80, 0xc3, 0xc3, 0x03, 0x00, 
	0xb9, 0x9d, 0x02, 0x00, 0xfd, 0xbf, 0x06, 0x00, 0xe1, 0x7f, 0x0c, 0x00, 0x9d, 0x7f, 0x18, 0x00, 
	0xbd, 0xbf, 0x10, 0x00, 0xb9, 0x9d, 0x30, 0x00, 0xc5, 0xc3, 0x60, 0x00, 0xff, 0xff, 0x7f, 0xc0
};
// 'Android Medium - 3', 32x14px
const unsigned char logo3 [] PROGMEM = {
	0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xf0, 0x06, 0xf3, 0xfc, 0xf0, 0x0c, 
	0x80, 0xf0, 0x30, 0x18, 0xfe, 0xe7, 0xb0, 0x30, 0xfe, 0xef, 0xf0, 0x30, 0xf0, 0x4f, 0xf0, 0x60, 
	0x8e, 0x4f, 0xf0, 0xc0, 0xbe, 0x4f, 0xf1, 0x80, 0xbe, 0x6f, 0xf1, 0x80, 0x9c, 0x73, 0x33, 0x00, 
	0xe3, 0xf8, 0x77, 0xff, 0xff, 0xff, 0xf7, 0xff
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

String httpGETRequest(const char* serverName) {
  WiFiClientSecure client;
  HTTPClient http;
  client.setInsecure(); 
  http.begin(client, serverName);
  int httpResponseCode = http.GET();
  String payload = "{}"; 
  if (httpResponseCode > 0) {
    payload = http.getString();
    if (payload.length() > 0) { return payload; }  
  }
  http.end();
  return payload;
}


void Cout(int x, int y, String text, int size) {
  display.clearDisplay(); 
  display.setTextSize(size); // Text size
  display.setCursor(x, y); // Set cursor to top left corner
  display.print(text); // Display text
  display.display(); // Update display
}

void CoutVal(int x, int y, String text, int size) {
  display.setTextSize(size);
  display.setCursor(x, y);
  display.print(text); // Tampilkan teks langsung
}

int em(int value) { int em = value * 8; return em; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void moduleSetup() {
  // display setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { for (;;); }
  display.setRotation(1); // Rotasi 90 deg
  display.setTextColor(SSD1306_WHITE);

  display.clearDisplay();
  display.drawBitmap(0, em(7), logo3, 32, 14, 1);
  display.display();

  delay(4000);

  // wifi setup
  int dots = 0;
  WiFi.begin(ssid, password);
  delay(1000);
  while (WiFi.status() != WL_CONNECTED) {
    display.clearDisplay();
    Cout(3, em(7), "<>", 2);
    display.setTextSize(2);
    
    // set posisi
    int x = 0;
    int y = em(9);

    for (int i = 0; i < dots; i++) { CoutVal(x + (i * 3), y, ".", 2); }
    display.display(); 
    
    dots = (dots + 1) % 8; 
    delay(500);
  }
  display.clearDisplay();
  display.drawBitmap(0, 30, logo1, 32, 32, 1);
  display.drawBitmap(4, 80, logo2, 26, 12, 1);
  display.display();
  delay(4000);
}

void gameSetup() {
  display.clearDisplay();
  // Inisialisasi grid secara acak
  randomSeed(millis());
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      grid[y][x] = random(2); // 0 atau 1
    }
  }
}

BLYNK_WRITE(V0) {
  String input = param.asStr();

  if (input.indexOf("-i") != -1) {
    input.replace("-i ", "");

    if (input == "timerDisplay") {
      terminal.println("timerDisplayOff: "+ String(hourDisplayOff) + ":" + String(minuteDisplayOff));
      terminal.println("timerDisplayOn: "+ String(hourDisplayOn) + ":" + String(minuteDisplayOn));
      terminal.flush();
    }
    
    else if (input == "timeOutDisplay") {
      terminal.println("timeOutDisplay: "+ String(intervalDisplayOut));
      terminal.flush();
    }

    else {
      terminal.println("Input Invalid");
      terminal.flush();
    }
  }

  else if (input.indexOf("-c") != -1) {
    input.replace("-c ", "");

    if (input.indexOf("timerDisplayOn") != -1) {
      input.replace("timerDisplayOn ", "");

      int colonIndex = input.indexOf(":");
      if (colonIndex != -1) {
        hourDisplayOn = input.substring(0, colonIndex).toInt(); // Ambil bagian jam
        minuteDisplayOn = input.substring(colonIndex + 1).toInt(); // Ambil bagian menit
      }
    }

    else if (input.indexOf("timerDisplayOff") != -1) {
      input.replace("timerDisplayOff ", "");

      int colonIndex = input.indexOf(":");
      if (colonIndex != -1) {
        hourDisplayOff = input.substring(0, colonIndex).toInt(); // Ambil bagian jam
        minuteDisplayOff = input.substring(colonIndex + 1).toInt(); // Ambil bagian menit
      }
    }

    else if (input.indexOf("timeOutDisplay") != -1) { // -c timeOutDisplay 3000
      input.replace("timeOutDisplay ", "");
      intervalDisplayOut = input.toInt();
    }

    else {
      terminal.println("Input Invalid");
      terminal.flush();
    }
  }

  input = "";
}

BLYNK_WRITE(V1) {  // ganti ke conways game
  int pinValue = param.asInt();  // Mendapatkan nilai dari tombol
  if (pinValue == 1) {  // Jika tombol ditekan
    gameSetup();
    displayActive = true;
  } else {
    displayActive = false;
  }
}

BLYNK_WRITE(V2) {  // reset conway game
  int pinValue = param.asInt();  // Mendapatkan nilai dari tombol
  if (pinValue == 1) {  // Jika tombol ditekan
    gameSetup();
  } 
}

void setup() {
  Serial.begin(115200);
  moduleSetup();
  timeClient.begin(); // Memulai NTPClient
  timeClient.forceUpdate(); // rst dan sinkronasi
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, password);
  displayOut_1(); // setup display 1
}

void loop() {
  dateTime(); // waktu
  weather(); // cuaca
  Blynk.run();
  !displayActive ? timeOutDisplay() : displayOut_3(); // pergantian display atau display game
  timerDisplay(); // display mati jam 22.00 -- 5.30
  delay(250);
}

void timeOutDisplay() {
  unsigned long waktuSaatIni = millis();

  // Pastikan tampilan diperbarui saat muncul
  if (waktuSaatIni - waktuUpdateWaktu >= timerWaktu) {
    waktuUpdateWaktu = waktuSaatIni;

    switch (siklus) {
      case 0:
        displayOut_1(); // Perbarui display 1
        break;
      case 1:
        displayOut_2(); // Perbarui display 2
        break;
    }
  }

  // Pergantian tampilan setiap 8 detik
  if (waktuSaatIni - waktuSebelum >= intervalDisplayOut) {
    siklus = (siklus + 1) % 2; // Siklus berputar 0 -> 1 // % 2 untuk banyaknya angka
    waktuSebelum = waktuSaatIni;

    switch (siklus) {
      case 0:
        displayOut_1(); // Tampilkan display 1
        break;
      case 1:
        displayOut_2(); // Tampilkan display 2
        break;
    }
  }
}


void timerDisplay() {
  // Dari jam 21:30 hingga 05:30
  if ((jam == hourDisplayOff && menit >= minuteDisplayOff) ||  // 21:30 hingga 21:59
      (jam > hourDisplayOff || jam < hourDisplayOn) ||       // 22:00 hingga 04:59
      (jam == hourDisplayOn && menit <= minuteDisplayOn)) {   // 05:00 hingga 05:30
      display.ssd1306_command(SSD1306_DISPLAYOFF);
  } 
  else { display.ssd1306_command(SSD1306_DISPLAYON); }
}


void dateTime() {
  timeClient.update();

  // Mengambil jam, menit, dan detik
  jam = timeClient.getHours();
  menit = timeClient.getMinutes();
  int detik = timeClient.getSeconds();

  int jamSebelum = jam;

  // Setelah mengambil waktu, perbarui nilai jam
  jam = timeClient.getHours();

  // Memastikan selisih jam tepat 1 (satu jam maju)
  if (abs(jam - jamSebelum) > 1) {
    timeClient.forceUpdate(); // Reset dan sinkronisasi jika selisih jam lebih dari 1
  }

  if (jam == -1 && menit == -1 && detik == -1){
    Cout(3, em(7), "!!", 2);
  } else { 
    // format 2 digit jam, menit, dan detik
    sprintf(jamStr, "%02d", jam);
    sprintf(menitStr, "%02d", menit);
    sprintf(detikStr, "%02d", detik);
  }
}

void weather() {
  // Send an HTTP GET request
  if ((millis() - lastTime) > interval) {
    // Check WiFi connection status
    if(WiFi.status() == WL_CONNECTED){
      serverPath = API_keys;
      jsonBuffer = httpGETRequest(serverPath.c_str());
      myObject = JSON.parse(jsonBuffer);
      //Serial.println(jsonBuffer);
      
      if (JSON.typeof(myObject) == "undefined") {
        Cout(0, em(7), "!!", 2);
        //Serial.println("Gagal");
        return;
      }

      // Cek apakah semua data yang diperlukan ada di dalam JSON
      if (myObject["main"].hasOwnProperty("feels_like") &&
          myObject["main"].hasOwnProperty("temp") &&
          myObject["main"].hasOwnProperty("humidity") &&
          myObject["wind"].hasOwnProperty("speed") &&
          myObject["clouds"].hasOwnProperty("all")) 
      {
        // Semua data ada
        awan = myObject["clouds"]["all"];
        angin = myObject["wind"]["speed"];
        kelembapan = myObject["main"]["humidity"];
        suhu = int(myObject["main"]["temp"]) - 273; // Kelvin ke Celsius
        suhuTerasa = int(myObject["main"]["feels_like"]) - 273; // Kelvin ke Celsius
      } 
      else { Cout(3, em(7), "!!", 2); }

      mainWeather = String(myObject["weather"][0]["main"]);
      if (mainWeather == "Clear") {
        cuaca = "Clear";
      } 
      else if (mainWeather == "Mist" || mainWeather == "Fog" || mainWeather == "Clouds") {
        cuaca = "Mist";
      } 
      else if (mainWeather == "Smoke" ||  mainWeather == "Haze") {
        cuaca = "Haze";
      }
      else if (mainWeather == "Rain" || mainWeather == "Drizzle" || mainWeather == "Thunderstorm") {
        cuaca = "Rain";
      }
      lastTime = millis();
    }
    else { 
      Cout(0, em(7), "!!", 2);
    }
  }
}

void displayOut_1() {
  display.clearDisplay(); 
  display.drawLine(0, 5, 128, 5, SSD1306_WHITE);

  CoutVal(5, 10, String(jamStr), 2);
  CoutVal(5, 30, String(menitStr), 2);
  CoutVal(5, 50, String(detikStr), 2);

  display.drawLine(0, 70, 128, 70, SSD1306_WHITE);

  CoutVal(5, 80, String(cuaca), 1);
  CoutVal(em(1) - 4, 95, String(suhuTerasa) + " " + "C", 1);
  CoutVal(em(1) - 4, 110, (kelembapan >= 100) ? "1> %" : String(kelembapan) + " " + "%", 1);

  display.display(); 
}

void displayOut_2() {
  display.clearDisplay(); 
  if (mainWeather == "Clear") { // cerah
    display.drawBitmap(0, 0, cerah, 31, 32, 1);
  } 

  else if ( mainWeather == "Haze" || mainWeather == "Smoke" ) { // cerah berawan
    display.drawBitmap(0, 0, kabut, 32, 32, 1);
  } 

  else if ( mainWeather == "Mist" || mainWeather == "Fog" || mainWeather == "Clouds") { // berawan atau kabut
    display.drawBitmap(0, 0, awanlogo, 32, 32, 1);
  }
  
  else if ( mainWeather == "Rain" ) { // hujan sedang dan hujan lebat
    display.drawBitmap(0, 0, hujan, 32, 32, 1);
  }
  
  else if ( mainWeather == "Drizzle" ) { // gerimis
    display.drawBitmap(0, 0, gerimis, 32, 32, 1);
  }

  else if (mainWeather == "Thunderstorm") { // hujan petir
    display.drawBitmap(0, 0, hujanPetir, 32, 32, 1);
  }

  CoutVal(5, em(5), String(cuaca), 1);
  
  display.drawBitmap(3, em(8) - 2, tem, 8, 8, 1);
  CoutVal(em(3) - 4, em(8) - 2, String(suhu), 1);

  display.drawBitmap(3, em(10) - 2, hum, 8, 8, 1);
  CoutVal(em(3) - 4, em(10) - 2, (kelembapan >= 100) ? "1>" : String(kelembapan), 1);

  display.drawBitmap(3, em(12) - 2, cld, 8, 8, 1);
  CoutVal(em(3) - 4, em(12) - 2, (awan >= 100) ? "1>" : String(awan), 1);

  display.drawBitmap(3, em(14) - 2, wid, 8, 8, 1);
  CoutVal(em(3) - 4, em(14) - 2, String(angin), 1);

  display.display(); 
}

void displayOut_3() {
  // Render grid ke OLED
  display.clearDisplay();
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      if (grid[y][x] == 1) {
        display.fillRect(x * 2, y * 2, 2, 2, SSD1306_WHITE); // Sel hidup
      }
    }
  }
  display.display();

  // Update grid
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      int neighbors = countNeighbors(x, y);
      if (grid[y][x] == 1) {
        newGrid[y][x] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
      } else {
        newGrid[y][x] = (neighbors == 3) ? 1 : 0;
      }
    }
  }

  // Copy newGrid ke grid
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      grid[y][x] = newGrid[y][x];
    }
  }
}

int countNeighbors(int x, int y) {
  int count = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx == 0 && dy == 0) continue; // Jangan hitung sel sendiri
      int nx = (x + dx + GRID_WIDTH) % GRID_WIDTH;
      int ny = (y + dy + GRID_HEIGHT) % GRID_HEIGHT;
      count += grid[ny][nx];
    }
  }
  return count;
}