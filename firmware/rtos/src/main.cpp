#include "config.local.h"
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BH1750.h>
#include <WiFi.h> //Task 7,8

// WiFi + TCP
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

const char* server_ip = TCP_SERVER_IP;
const uint16_t server_port = 5000;

WiFiClient client;

int led_sema_status = 0;
char server_response[32] = "No server";

SemaphoreHandle_t wifiMutex;

// Define Pin connections
#define LED_BLINK 27
#define SCL_PIN 22
#define SDA_PIN 21
#define LED_TG 26
#define BUTTON_1 32
#define LED_SEMA 25
#define BUTTON_2 33

// Global variables
float lux = 0.0;
int b = 0;
int toggle_status = 0;
SemaphoreHandle_t xBinarySemaphore;

// Display and Sensor Objects
Adafruit_SSD1306 display(128, 64, &Wire, -1);
BH1750 lightMeter(0x23);

// --- Task 1: Blink LED 200ms ---
void blink_led_200ms(void *pvParameters) {
    pinMode(LED_BLINK, OUTPUT);
    while (1) {
        digitalWrite(LED_BLINK, HIGH);
        vTaskDelay(pdMS_TO_TICKS(100));
        digitalWrite(LED_BLINK, LOW);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// --- Task 2: Read BH1750 ---
void read_bh1750(void *pvParameters) {
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE);
    while (1) {
        if (lightMeter.measurementReady()) {
            lux = lightMeter.readLightLevel();
            Serial.print("Lux: "); Serial.println(lux);
        }
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

// --- Task 3: Button Toggle ---
void button_toggle_task(void *pvParameters) {
    pinMode(LED_TG, OUTPUT);
    pinMode(BUTTON_1, INPUT_PULLUP);
    while (1) {
        if (digitalRead(BUTTON_1) == LOW) {
            while (digitalRead(BUTTON_1) == LOW) vTaskDelay(pdMS_TO_TICKS(10));
            toggle_status = !toggle_status;
            digitalWrite(LED_TG, toggle_status);
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

// --- Task 4 & 5: Semaphore LED ---
void IRAM_ATTR btn_2_ISR() {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xSemaphoreGiveFromISR(xBinarySemaphore, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) portYIELD_FROM_ISR();
}

void led_sema_task(void *pvParameters)
{
    pinMode(BUTTON_2, INPUT_PULLUP);
    pinMode(LED_SEMA, OUTPUT);

    attachInterrupt(digitalPinToInterrupt(BUTTON_2), btn_2_ISR, FALLING);

    while (1)
    {
        if (xSemaphoreTake(xBinarySemaphore, portMAX_DELAY) == pdPASS)
        {
            digitalWrite(LED_SEMA, HIGH);
            led_sema_status = 1;

            b = 6;  // 3s countdown

            while (b > 0)
            {
                vTaskDelay(pdMS_TO_TICKS(500));
                b--;
            }

            digitalWrite(LED_SEMA, LOW);
            led_sema_status = 0;
        }
    }
}

// --- Task 6: OLED Display ---
void oled_display_task(void *pvParameters) {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextColor(WHITE);


    char buffer[32];
    static float last_lux = -1.0;
    static int last_toggle = -1;
    static String last_server = "";

    while (1) {
        if (lux != last_lux || toggle_status != last_toggle || last_server != String(server_response)) {
            display.clearDisplay();
            
            // LED Status
            display.setCursor(0, 0);
            sprintf(buffer, "LED 2: %s", toggle_status ? "ON" : "OFF");
            display.print(buffer);
            
            // Lux Value
            display.setCursor(0, 20);
            sprintf(buffer, "Lux: %.1f", lux);
            display.print(buffer);
            
            // Timer
            display.setCursor(0, 40);
            sprintf(buffer, "Timer: %d s", b / 2);
            display.print(buffer);

            // Server response
            display.setCursor(0, 55);
            display.print(server_response);

            display.display();
            last_server = String(server_response);
            last_lux = lux;
            last_toggle = toggle_status;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
//Task 7+8: TCP client
void tcp_client_task(void *pvParameters) {

    while (1) {

        if (WiFi.status() != WL_CONNECTED) {
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
        }

        if (!client.connected())
        {
          Serial.println("Connecting TCP...");

          if (!client.connect(server_ip, server_port))
          {
            Serial.println("TCP connection failed");
            vTaskDelay(pdMS_TO_TICKS(2000));
            continue;
          }

          Serial.println("TCP connected");
        }

        if (client.connected()) {

            char buffer[128];

            sprintf(buffer,
                    "{\"lux\":%.2f,\"led_tg\":%d,\"led_sema\":%d}\n",
                    lux,
                    toggle_status,
                    led_sema_status);

            client.print(buffer);
            Serial.println(buffer);

            unsigned long timeout = millis();
            while (!client.available()) {
                if (millis() - timeout > 2000) break;
            }

            if (client.available())
            {

                String response = client.readStringUntil('\n');
                response.trim();

                response.toCharArray(server_response, 32);

                Serial.print("Server: ");
                Serial.println(response);

                // LED1
                if (response == "LED1_ON")
                {
                    xSemaphoreGive(xBinarySemaphore);
                }
                // LED2
                if (response == "LED2_ON")
                {
                    toggle_status = 1;
                    digitalWrite(LED_TG, HIGH);
                }

                if (response == "LED2_OFF")
                {
                    toggle_status = 0;
                    digitalWrite(LED_TG, LOW);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void setup() {
    Serial.begin(115200);
    Wire.begin(SDA_PIN, SCL_PIN); // Initialize shared I2C bus
    xBinarySemaphore = xSemaphoreCreateBinary();

    //Connecting WIFI
    WiFi.begin(ssid, password);
    Serial.print("Connecting WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.println(WiFi.localIP());

    // Create Tasks
    xTaskCreate(blink_led_200ms, "Blink", 2048, NULL, 5, NULL);
    xTaskCreate(read_bh1750, "BH1750", 4096, NULL, 5, NULL);
    xTaskCreate(button_toggle_task, "Toggle", 2048, NULL, 6, NULL);
    xTaskCreate(led_sema_task, "Sema", 2048, NULL, 5, NULL);
    xTaskCreate(oled_display_task, "OLED", 4096, NULL, 4, NULL);
    xTaskCreate(tcp_client_task, "TCP", 4096, NULL, 3, NULL);
}

void loop() {
    vTaskDelete(NULL); }
