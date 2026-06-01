#include <Arduino.h>

// Pin
#define TRIG_PIN 4
#define ECHO_PIN 27
#define LED_PIN 2

// Queue handle
QueueHandle_t distanceQueue;

// Fungsi baca ultrasonik
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  return distance;
}

// Task 1: Sensor
void sensorTask(void *pvParameters) {
  float distance;

  while (1) {
    distance = readDistance();

    xQueueSend(distanceQueue, &distance, portMAX_DELAY);

    vTaskDelay(pdMS_TO_TICKS(200)); // baca tiap 200 ms
  }
}

// Task 2: LED
void ledTask(void *pvParameters) {
  float distance;
  int delayTime;

  while (1) {
    if (xQueueReceive(distanceQueue, &distance, portMAX_DELAY)) {

      // Mapping jarak ke delay
      if (distance < 10) delayTime = 100;
      else if (distance < 30) delayTime = 300;
      else delayTime = 700;

      digitalWrite(LED_PIN, HIGH);
      vTaskDelay(pdMS_TO_TICKS(delayTime));

      digitalWrite(LED_PIN, LOW);
      vTaskDelay(pdMS_TO_TICKS(delayTime));
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Buat queue
  distanceQueue = xQueueCreate(5, sizeof(float));

  // Buat task
  xTaskCreate(sensorTask, "Sensor Task", 2048, NULL, 1, NULL);
  xTaskCreate(ledTask, "LED Task", 2048, NULL, 1, NULL);
}

void loop() {
  // kosong (karena pakai RTOS)
}