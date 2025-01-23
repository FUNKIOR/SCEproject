#include <lvgl.h>
#include <TFT_eSPI.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <freertos/semphr.h>


#include <XPT2046_Touchscreen.h>

#include <ui/ui.h>
#include <ui/vars.h>


#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

int32_t power;
int32_t rpm;
int32_t RPMD = 0;
int32_t RPME = 0;


SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

int i = 0;

uint32_t lastTick = 0;

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

SemaphoreHandle_t adcMutex;

// Define queues for communication between tasks
QueueHandle_t powerqueue;  // Queue

// Shared global variables for ADC values
volatile int adcValue1 = 0;
volatile int adcValue2 = 0;

// Define a portMUX_TYPE variable
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void parseADCValues(const String& input, int& rpmA, int& rpmB) {
    // Buffer to hold the input string
    char buffer[50];
    input.toCharArray(buffer, 50);

    // Parse the input string
    int parsed = sscanf(buffer, "%d,%d", &rpmA, &rpmB);
    return;
}

// If logging is enabled, it will inform the user about what is happening in the library
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  //Serial.println(buf);
  //Serial.flush();
}
// Get the Touchscreen data
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
  if(touchscreen.tirqTouched() && touchscreen.touched()) {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;

    // Set the coordinates
    data->point.x = x;
    data->point.y = y;
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}

int32_t get_var_rpm() {
    return rpm;
}
int32_t get_var_power() {
    return power;
}
static void bar_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        char buf[4];
        lv_snprintf(buf, sizeof(buf), "%d", power);
        lv_bar_set_value(objects.bar, power, LV_ANIM_OFF);
    }
}

void bar_disp(int32_t value) {
    char buf[4];
    lv_arc_set_value(objects.bar, value);
    lv_snprintf(buf, sizeof(buf), "%d", value);
    lv_label_set_text(objects.power, buf);
    i++;
    if(i > 100) {
    i = 0;
    }
}
void RPMdisplay(int32_t valueD, int32_t valueE) {
    char bufD[4];
    char bufE[4];
    lv_snprintf(bufD, sizeof(bufD), "%d", valueD);
    lv_snprintf(bufE, sizeof(bufE), "%d", valueE);
    lv_label_set_text(objects.rpm, bufD);
    lv_label_set_text(objects.rpm_2, bufE);
}
void lvgl_task(void *pvParameter) {
  int POW = 0;
    while (1) {
      xQueueReceive(powerqueue, &POW, portMAX_DELAY);
      bar_disp(POW);
      RPMdisplay(RPMD, RPME);
      lv_tick_inc(millis() - lastTick);     // tell LVGL how much time has passed
      lastTick = millis();
      lv_task_handler();
      vTaskDelay(pdMS_TO_TICKS(5));
    }
}
void Lora_task(void *pvParameter) {
  Serial.begin(115200);
  while (1){
    int localAdcValue1, localAdcValue2;
    if (xSemaphoreTake(adcMutex, portMAX_DELAY) == pdTRUE) {
      localAdcValue1 = adcValue1;
      localAdcValue2 = adcValue2;
      xSemaphoreGive(adcMutex);
    }
    taskENTER_CRITICAL(&mux);
    Serial.printf("%d,%d\n", localAdcValue1, localAdcValue2);
    taskEXIT_CRITICAL(&mux);
    vTaskDelay(pdMS_TO_TICKS(70));
  }
}
void Lora_Receive_task(void *pvParameter) {
  Serial.begin(115200);
  while (1){
    if (Serial.available() > 0) {
      taskENTER_CRITICAL(&mux);
      String data = Serial.readStringUntil('\n');
      parseADCValues(data, RPMD, RPME);
      taskEXIT_CRITICAL(&mux);
    }
    vTaskDelay(pdMS_TO_TICKS(373));
  }
}
void Stick_task(void *pvParameter) {
  analogReadResolution(9);
  int pow = 0;
  while (1){
    // Read the ADC values from the defined pins
    int localAdcValue1 = analogRead(35);
    int localAdcValue2 = analogRead(27);
    
    localAdcValue1 = localAdcValue1-255-(-13);
    localAdcValue2 = localAdcValue2-255-(40);
    
    pow = int((abs(localAdcValue1) + abs(localAdcValue2-5))/(5.11));
    xQueueSend(powerqueue, &pow, portMAX_DELAY);

    if (xSemaphoreTake(adcMutex, portMAX_DELAY) == pdTRUE) {
        adcValue1 = localAdcValue1;
        adcValue2 = localAdcValue2;
        xSemaphoreGive(adcMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}
void Check_task(void *pvParameter) {
  while (1) {
    digitalWrite(4, LOW);
    if (lastTick > 1000){
      digitalWrite(4, HIGH);
      vTaskDelete(NULL);
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}
void setup() {
  // Start LVGL
  lv_init();
  // Register print function for debugging
  lv_log_register_print_cb(log_print);

  // Start the SPI for the touchscreen and init the touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  // Set the Touchscreen rotation in landscape mode
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 0: touchscreen.setRotation(0);
  touchscreen.setRotation(0);

  // Create a display object
  lv_display_t * disp;
  // Initialize the TFT display using the TFT_eSPI library
  disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);
    
  // Initialize an LVGL input device object (Touchscreen)
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  // Set the callback function to read Touchscreen input
  lv_indev_set_read_cb(indev, touchscreen_read);

  ui_init();

  adcMutex = xSemaphoreCreateMutex();
   powerqueue = xQueueCreate(10, sizeof(int32_t));

  pinMode(4, OUTPUT); 
  xTaskCreatePinnedToCore(Check_task, "Check Task", 4096, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(lvgl_task, "LVGL Task", 8192, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(Stick_task, "Joystick Task", 8192, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(Lora_task, "Communication Task", 4096, NULL, 4, NULL, 0);
  xTaskCreatePinnedToCore(Lora_Receive_task, "Communication Send Task", 4096, NULL, 3, NULL, 0);

}
void set_var_power(int32_t value) {
    power = value;
}
void set_var_rpm(int32_t value) {
    rpm = value;
}

void loop() {
  vTaskDelete(NULL);
}