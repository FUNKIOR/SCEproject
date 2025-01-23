#include <Arduino.h>

// Define the PWM frequency and resolution
const int pwmFrequency = 1000; // 1 kHz
const int pwmResolution = 8;   // 8-bit resolution (0-255)

// Define the initial duty cycle
int dutyCycleR = 0; // duty cycle for right motor
int dutyCycleL = 0; // duty cycle for left motor

int changeR, change = 0;

// Motor control pins
const int RmotorF = 32;
const int RmotorB = 33;
const int LmotorF = 26;
const int LmotorB = 25;

int newDutyCycleR = 0;
int newDutyCycleL = 0;

TaskHandle_t xTask1Handle = NULL;

// Mutex for shared variables
SemaphoreHandle_t dutyCycleMutex;

// Define a portMUX_TYPE variable
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// Variables for Wheel A
volatile int32_t pulseCountA = 0;    // Pulse count for Wheel A
volatile float rpmA = 0.0;           // RPM for Wheel A
volatile uint32_t lastPulseTimeA = 0; // Last pulse time for Wheel A

// Variables for Wheel B
volatile int32_t pulseCountB = 0;    // Pulse count for Wheel B
volatile float rpmB = 0.0;           // RPM for Wheel B
volatile uint32_t lastPulseTimeB = 0; // Last pulse time for Wheel B
volatile float lastPrintTime = 0;

// Motor control task
void motorControlTask(void *pvParameters) {
  int lastCheckTime = 0;
  while (true) {
    // Update the duty cycle
    if (xSemaphoreTake(dutyCycleMutex, portMAX_DELAY) == pdTRUE) {
      if (dutyCycleR > 0) {
        ledcWrite(0, dutyCycleR);
        ledcWrite(1, 0);
      }else {
        ledcWrite(0, 0);
        ledcWrite(1, -(dutyCycleR));
      }
      if (dutyCycleL > 0) {
        ledcWrite(2, dutyCycleL);
        ledcWrite(3, 0);
      }else {
        ledcWrite(2, 0);
        ledcWrite(3, -(dutyCycleL));
      }
      xSemaphoreGive(dutyCycleMutex);
    }
        // Check if 1 second has passed
    if (millis() - lastCheckTime >= 1000) {
      // Check if the duty cycles are the same as 1 second ago
      if (changeR == change) {
        // Condition happens if the values are the same
        dutyCycleR = 0;
        dutyCycleL = 0;
      }
      // Update the previous duty cycle values and reset the timer
      changeR = change;
      lastCheckTime = millis();
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
// Function to parse ADC values from a string
void parseADCValues(const String& input, int& adc1, int& adc2) {
    // Buffer to hold the input string
    char buffer[50];
    input.toCharArray(buffer, 50);

    // Parse the input string
    int parsed = sscanf(buffer, "%d,%d", &adc1, &adc2);
    return;
}
// ISR for Sensor A (Wheel A)
void IRAM_ATTR sensorA_ISR() {
    pulseCountA++;
}
// ISR for Sensor B (Wheel B)
void IRAM_ATTR sensorB_ISR() {
    pulseCountB++;
}
// Function to calculate RPM
float calculateRPM(int32_t pulseCount, uint32_t lastPulseTime) {
    uint32_t currentTime = micros();
    uint32_t elapsedTime = currentTime - lastPulseTime;
    if (elapsedTime == 0) return 0; // Prevent division by zero
    float revolutions = (float)pulseCount / 20;
    return (revolutions * 60.0 * 1000000.0) / elapsedTime;
}
// Task to reset RPM values
// Function to reset RPM and pulse count values
void resetRPMAndPulseCount() {
    pulseCountA = 0;
    rpmA = 0.0;
    pulseCountB = 0;
    rpmB = 0.0;
}
void resetRPMTask(void *pvParameters) {
    while (true) {
    // Print RPM for both wheels every second
    lastPrintTime = 0;
    if (millis() - lastPrintTime > 1000) {
        lastPrintTime = millis();
        // Calculate RPM for Wheel A
        float rpmA = calculateRPM(pulseCountA, lastPulseTimeA);
        lastPulseTimeA = micros();

        // Calculate RPM for Wheel B
        float rpmB = calculateRPM(pulseCountB, lastPulseTimeB);
        Serial.printf("RPM A: %.2f, RPM B: %.2f\n", rpmA, rpmB);
        resetRPMAndPulseCount();
      }
      vTaskDelay(pdMS_TO_TICKS(500));

    }
}
// Telemetry control task

void telemetrySendTask(void *pvParameters) {
  while (true) {
    Serial2.printf("%d,%d\n",rpmA,rpmB);
    //vTaskPrioritySet(xTask1Handle, 2);
    vTaskDelay(pdMS_TO_TICKS(373));
  }
}
void telemetryControlTask(void *pvParameters) {
  while (true) {
    
    if (Serial2.available() > 0) {
      taskENTER_CRITICAL(&mux);
      change++;
      String incomingString = Serial2.readStringUntil('\n');
      parseADCValues(incomingString, newDutyCycleR, newDutyCycleL);
      taskEXIT_CRITICAL(&mux);
    }
    if (xSemaphoreTake(dutyCycleMutex, portMAX_DELAY) == pdTRUE) {
      dutyCycleR = newDutyCycleR;
      dutyCycleL = newDutyCycleL;
      xSemaphoreGive(dutyCycleMutex);
    }
    
    if (change > 1000) {
      change = 0;
    }
    //vTaskPrioritySet(xTask1Handle, 3);
    vTaskDelay(pdMS_TO_TICKS(70));
  }
}

void setup() {
  // Initialize serial communication  
  Serial.begin(115200);
  Serial2.begin(115200);
  pinMode(2, OUTPUT);
// Configure the motor control pins
  pinMode(RmotorF, OUTPUT);
  pinMode(RmotorB, OUTPUT);
  pinMode(LmotorF, OUTPUT);
  pinMode(LmotorB, OUTPUT);
  // Configure PWM channels
  ledcSetup(0, pwmFrequency, pwmResolution);
  ledcSetup(1, pwmFrequency, pwmResolution);
  ledcSetup(2, pwmFrequency, pwmResolution);
  ledcSetup(3, pwmFrequency, pwmResolution);
  // Attach PWM channels to motor control pins
  ledcAttachPin(RmotorF, 0);
  ledcAttachPin(RmotorB, 1);
  ledcAttachPin(LmotorF, 2);
  ledcAttachPin(LmotorB, 3);
  // Set the initial duty cycle

  pinMode(19, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  // Create the mutex for shared variables
  dutyCycleMutex = xSemaphoreCreateMutex();
  // Attach interrupts to sensors
  attachInterrupt(digitalPinToInterrupt(19), sensorA_ISR, RISING);
  attachInterrupt(digitalPinToInterrupt(18), sensorB_ISR, RISING);
  // Create the reset RPM task
  xTaskCreatePinnedToCore(resetRPMTask, "Reset RPM Task", 4096, NULL, 1, NULL, 1);
  // Create the motor control task with higher priority
  xTaskCreatePinnedToCore(motorControlTask,"Motor Control Task",4096,NULL,1, NULL, 1);
  // Create the telemetry control task with lower priority
  xTaskCreatePinnedToCore(telemetryControlTask,"Telemetry Control Task",4096,NULL,3, NULL,0);
  xTaskCreatePinnedToCore(telemetrySendTask,"Telemetry Send Task",4096,NULL,2,&xTask1Handle ,0);
}

void loop() {
  digitalWrite(2, !digitalRead(2)); // Toggle pin state
}
