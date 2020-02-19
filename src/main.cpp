
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WS2812FX.h>
#include <WiFiClient.h>


extern const char index_html[];
extern const char main_js[];

#define WIFI_SSID "Lighte-Hi-Lighte"
#define WIFI_PASSWORD "AP12345678"

//#define STATIC_IP                       // uncomment for static IP, set IP below
#ifdef STATIC_IP
  IPAddress ip(192,168,0,123);
  IPAddress gateway(192,168,0,1);
  IPAddress subnet(255,255,255,0);
#endif

// QUICKFIX...See https://github.com/esp8266/Arduino/issues/263
#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

#define LED_PIN 23                       // 0 = GPIO0, 2=GPIO2
#define LED_COUNT 24

#define WIFI_TIMEOUT 30000              // checks WiFi every ...ms. Reset after this time, if WiFi cannot reconnect.
#define HTTP_PORT 80

#define DEFAULT_COLOR 0xFF5900
#define DEFAULT_BRIGHTNESS 255
#define DEFAULT_SPEED 1000
#define DEFAULT_MODE FX_MODE_STATIC

unsigned long auto_last_change = 0;
unsigned long last_wifi_check_time = 0;
String modes = "";
uint8_t myModes[] = {}; // *** optionally create a custom list of effect/mode numbers
boolean auto_cycle = false;

WS2812FX ws2812fx = WS2812FX(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WebServer server(HTTP_PORT);




/*
 * Connect to WiFi. If no connection is made within WIFI_TIMEOUT, ESP gets resettet.
 */
void wifi_setup() {
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // WiFi.mode(WIFI_STA);
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  
  #ifdef STATIC_IP  
    WiFi.config(ip, gateway, subnet);
  #endif

  // unsigned long connect_start = millis();
  // while(WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");

  //   if(millis() - connect_start > WIFI_TIMEOUT) {
  //     Serial.println();
  //     Serial.print("Tried ");
  //     Serial.print(WIFI_TIMEOUT);
  //     Serial.print("ms. Resetting ESP now.");
  //     ESP.reset();
  //   }
  // }

  Serial.println("");
  Serial.println("WiFi connected");  
  // Serial.print("IP address: ");
  // Serial.println(WiFi.localIP());
  Serial.println();
}


/*
 * Build <li> string for all modes.
 */
void modes_setup() {
  modes = "";
  uint8_t num_modes = sizeof(myModes) > 0 ? sizeof(myModes) : ws2812fx.getModeCount();
  for(uint8_t i=0; i < num_modes; i++) {
    uint8_t m = sizeof(myModes) > 0 ? myModes[i] : i;
    modes += "";
    // modes += "<li><a href='#' class='m' id='";
    modes += m;
    modes += "|";
    modes += ws2812fx.getModeName(m);
    modes += "";
    modes += i < num_modes - 1 ? "," : "";
  }
  
}

/* #####################################################
#  Webserver Functions
##################################################### */

void srv_handle_not_found() {
  server.send(404, "text/plain", "File Not Found");
}

void srv_handle_index_html() {
  server.send_P(200,"text/html", index_html);
}

void srv_handle_main_js() {
  server.send_P(200,"application/javascript", main_js);
}

void srv_handle_modes() {
  server.send(200,"text/plain", modes);
}

void srv_handle_set() {
  for (uint8_t i=0; i < server.args(); i++){
    if(server.argName(i) == "c") {
      uint32_t tmp = (uint32_t) strtol(server.arg(i).c_str(), NULL, 16);
      if(tmp >= 0x000000 && tmp <= 0xFFFFFF) {
        ws2812fx.setColor(tmp);
      }
    }

    if(server.argName(i) == "m") {
      uint8_t tmp = (uint8_t) strtol(server.arg(i).c_str(), NULL, 10);
      ws2812fx.setMode(tmp % ws2812fx.getModeCount());
      Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    }

    if(server.argName(i) == "b") {
      if(server.arg(i)[0] == '-') {
        ws2812fx.setBrightness(ws2812fx.getBrightness() * 0.8);
      } else if(server.arg(i)[0] == ' ') {
        ws2812fx.setBrightness(min(max(ws2812fx.getBrightness(), 5) * 1.2, 255));
      } else { // set brightness directly
        uint8_t tmp = (uint8_t) strtol(server.arg(i).c_str(), NULL, 10);
        ws2812fx.setBrightness(tmp);
      }
      Serial.print("brightness is "); Serial.println(ws2812fx.getBrightness());
    }

    if(server.argName(i) == "s") {
      if(server.arg(i)[0] == '-') {
        ws2812fx.setSpeed(max(ws2812fx.getSpeed(), 5) * 1.2);
      } else if(server.arg(i)[0] == ' ') {
        ws2812fx.setSpeed(ws2812fx.getSpeed() * 0.8);
      } else {
        uint16_t tmp = (uint16_t) strtol(server.arg(i).c_str(), NULL, 10);
        ws2812fx.setSpeed(tmp);
      }
      Serial.print("speed is "); Serial.println(ws2812fx.getSpeed());
    }

    if(server.argName(i) == "a") {
      if(server.arg(i)[0] == '-') {
        auto_cycle = false;
      } else {
        auto_cycle = true;
        auto_last_change = 0;
      }
    }
  }
  server.send(200, "text/plain", "OK");
}

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.println("Starting...");

  modes.reserve(5000);
  modes_setup();

  Serial.println("WS2812FX setup");
  ws2812fx.init();
  ws2812fx.setMode(DEFAULT_MODE);
  ws2812fx.setColor(DEFAULT_COLOR);
  ws2812fx.setSpeed(DEFAULT_SPEED);
  ws2812fx.setBrightness(DEFAULT_BRIGHTNESS);
  ws2812fx.start();

  Serial.println("Wifi setup");
  wifi_setup();
 
  Serial.println("HTTP server setup");
  server.on("/", srv_handle_index_html);
  server.on("/main.js", srv_handle_main_js);
  server.on("/modes", srv_handle_modes);
  server.on("/set", srv_handle_set);
  server.onNotFound(srv_handle_not_found);
  server.begin();
  Serial.println("HTTP server started.");
  Serial.println("ready!");
  
}


void loop() {
  unsigned long now = millis();

  server.handleClient();
  ws2812fx.service();

  // if(now - last_wifi_check_time > WIFI_TIMEOUT) {
  //   Serial.print("Checking WiFi... ");
  //   if(WiFi.status() != WL_CONNECTED) {
  //     Serial.println("WiFi connection lost. Reconnecting...");
  //     wifi_setup();
  //   } else {
  //     Serial.println("OK");
  //   }
  //   last_wifi_check_time = now;
  // }

  if(auto_cycle && (now - auto_last_change > 10000)) { // cycle effect mode every 10 seconds
    uint8_t next_mode = (ws2812fx.getMode() + 1) % ws2812fx.getModeCount();
    if(sizeof(myModes) > 0) { // if custom list of modes exists
      for(uint8_t i=0; i < sizeof(myModes); i++) {
        if(myModes[i] == ws2812fx.getMode()) {
          next_mode = ((i + 1) < sizeof(myModes)) ? myModes[i + 1] : myModes[0];
          break;
        }
      }
    }
    ws2812fx.setMode(next_mode);
    Serial.print("mode is "); Serial.println(ws2812fx.getModeName(ws2812fx.getMode()));
    auto_last_change = now;
  }
}




// #include<Arduino.h>
// #include <Adafruit_NeoPixel.h>
// #ifdef __AVR__
//  #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
// #endif

// // Which pin on the Arduino is connected to the NeoPixels?
// // On a Trinket or Gemma we suggest changing this to 1:
// #define LED_PIN    23

// // How many NeoPixels are attached to the Arduino?
// #define LED_COUNT 13

// // Declare our NeoPixel strip object:
// Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
// // Argument 1 = Number of pixels in NeoPixel strip
// // Argument 2 = Arduino pin number (most are valid)
// // Argument 3 = Pixel type flags, add together as needed:
// //   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
// //   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
// //   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
// //   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
// //   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)


// // Some functions of our own for creating animated effects -----------------

// // Fill strip pixels one after another with a color. Strip is NOT cleared
// // first; anything there will be covered pixel by pixel. Pass in color
// // (as a single 'packed' 32-bit value, which you can get by calling
// // strip.Color(red, green, blue) as shown in the loop() function above),
// // and a delay time (in milliseconds) between pixels.
// void colorWipe(uint32_t color, int wait) {
//   for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
//     strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
//     strip.show();                          //  Update strip to match
//     delay(wait);                           //  Pause for a moment
//   }
// }

// // Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// // a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// // between frames.
// void theaterChase(uint32_t color, int wait) {
//   for(int a=0; a<10; a++) {  // Repeat 10 times...
//     for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
//       strip.clear();         //   Set all pixels in RAM to 0 (off)
//       // 'c' counts up from 'b' to end of strip in steps of 3...
//       for(int c=b; c<strip.numPixels(); c += 3) {
//         strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
//       }
//       strip.show(); // Update strip with new contents
//       delay(wait);  // Pause for a moment
//     }
//   }
// }

// // Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
// void rainbow(int wait) {
//   // Hue of first pixel runs 5 complete loops through the color wheel.
//   // Color wheel has a range of 65536 but it's OK if we roll over, so
//   // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
//   // means we'll make 5*65536/256 = 1280 passes through this outer loop:
//   for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
//     for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
//       // Offset pixel hue by an amount to make one full revolution of the
//       // color wheel (range of 65536) along the length of the strip
//       // (strip.numPixels() steps):
//       int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
//       // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
//       // optionally add saturation and value (brightness) (each 0 to 255).
//       // Here we're using just the single-argument hue variant. The result
//       // is passed through strip.gamma32() to provide 'truer' colors
//       // before assigning to each pixel:
//       strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
//     }
//     strip.show(); // Update strip with new contents
//     delay(wait);  // Pause for a moment
//   }
// }

// // Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
// void theaterChaseRainbow(int wait) {
//   int firstPixelHue = 0;     // First pixel starts at red (hue 0)
//   for(int a=0; a<30; a++) {  // Repeat 30 times...
//     for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
//       strip.clear();         //   Set all pixels in RAM to 0 (off)
//       // 'c' counts up from 'b' to end of strip in increments of 3...
//       for(int c=b; c<strip.numPixels(); c += 3) {
//         // hue of pixel 'c' is offset by an amount to make one full
//         // revolution of the color wheel (range 65536) along the length
//         // of the strip (strip.numPixels() steps):
//         int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
//         uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
//         strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
//       }
//       strip.show();                // Update strip with new contents
//       delay(wait);                 // Pause for a moment
//       firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
//     }
//   }
// }

// // setup() function -- runs once at startup --------------------------------

// void setup() {
//   // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
//   // Any other board, you can remove this part (but no harm leaving it):
// #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
//   clock_prescale_set(clock_div_1);
// #endif
//   // END of Trinket-specific code.

//   strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
//   strip.show();            // Turn OFF all pixels ASAP
//   strip.setBrightness(100); // Set BRIGHTNESS to about 1/5 (max = 255)
// }


// // loop() function -- runs repeatedly as long as board is on ---------------

// void loop() {
//   // Fill along the length of the strip in various colors...
//   colorWipe(strip.Color(255,   0,   0), 50); // Red
//   colorWipe(strip.Color(  0, 255,   0), 50); // Green
//   colorWipe(strip.Color(  0,   0, 255), 50); // Blue

//   // Do a theater marquee effect in various colors...
//   theaterChase(strip.Color(127, 127, 127), 50); // White, half brightness
//   theaterChase(strip.Color(127,   0,   0), 50); // Red, half brightness
//   theaterChase(strip.Color(  0,   0, 127), 50); // Blue, half brightness

//   rainbow(10);             // Flowing rainbow cycle along the whole strip
//   theaterChaseRainbow(50); // Rainbow-enhanced theaterChase variant
// }


