#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "esp_camera.h"
#include "SPIFFS.h"

// Replace with your network credentials
const char* ssid = "जय हनुमान";
const char* password = "jaishreeram";

// Replace with your Telegram BOT Token
#define BOT_TOKEN "6375862647:AAGdwCzz8qBL2l3HN-qWa1WJ2u6JkPebfJo"

// Replace with your Telegram Chat ID
#define CHAT_ID "1457380992"

// Initialize Telegram bot with the bot token and WiFi client
WiFiClientSecure client;
UniversalTelegramBot bot(BOT_TOKEN, client);

// Pin configuration
#define TOUCH_PIN 4

// Camera configuration
#define CAMERA_MODEL_AI_THINKER

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("Failed to mount SPIFFS");
    while (1);
  }

  // Initialize camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  // ... configure other camera settings

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera initialization failed with error 0x%x", err);
    while (1);
  }

  // Set up touch sensor pin
  pinMode(TOUCH_PIN, INPUT);

  Serial.println("Setup complete");
}

void loop() {
  if (digitalRead(TOUCH_PIN) == HIGH) {
    captureAndSendPhoto();
    delay(1000); // Avoid multiple rapid triggers
  }
}

void captureAndSendPhoto() {
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  // Save the image to SPIFFS
  String photoFilePath = "/captured.jpg";
  File photoFile = SPIFFS.open(photoFilePath, FILE_WRITE);
  if (!photoFile) {
    Serial.println("Failed to create photo file");
    esp_camera_fb_return(fb);
    return;
  }
  photoFile.write(fb->buf, fb->len);
  photoFile.close();
  esp_camera_fb_return(fb);

  // Send the image to your Telegram bot
  String caption = "Captured Image";
  if (bot.sendPhoto(CHAT_ID, photoFilePath, caption)) {
    Serial.println("Photo sent to Telegram");
  } else {
    Serial.println("Photo sending failed");
  }
}
