#include <TM1637Display.h>

#define PUMP 2
#define MOIST_SENSOR A0
#define WATER_SENSOR 5
#define CLK 4
#define DIO 3
#define DEBUG 1
#define NEVER -1

const unsigned long MAX_UNSIGNED_LONG = 4294967295UL;
const int PUMP_ACTIVATION_TIMEOUT = 5000;
const int MOIST_SENSOR_WET = 239;
const int MOIST_SENSOR_DRY = 595;
const int WATERING_TIMEOUT = 10000;
const int DRY_SOIL = 60;

unsigned long lastWateringMilis = 0;

TM1637Display display(CLK, DIO);

unsigned long calculateLastWatering() {
  if (!lastWateringMilis) {
    return WATERING_TIMEOUT + 1;
  }

  unsigned long elapsed = millis() - lastWateringMilis;

  /* For the case of millis() reset due to variable memory overflow */
  if (elapsed < 0) {
    return (MAX_UNSIGNED_LONG - lastWateringMilis) + millis();
  }

  return elapsed;
}

void activatePump(int ms = 1000) {
  unsigned long elapsedSinceLastWatering = calculateLastWatering();

  Serial.print(elapsedSinceLastWatering);
  Serial.print("\n");

  if (elapsedSinceLastWatering < WATERING_TIMEOUT) {
    return;
  }

  digitalWrite(PUMP, HIGH);
  delay(ms);
  digitalWrite(PUMP, LOW);

  lastWateringMilis = millis();
}

int getSoilMoisture() {
  int moisture = analogRead(MOIST_SENSOR);

  return map(moisture, MOIST_SENSOR_WET, MOIST_SENSOR_DRY, 100, 0);
}

void setup() {
  pinMode(PUMP, OUTPUT);
  pinMode(MOIST_SENSOR, INPUT);

  display.setBrightness(2);

  if (DEBUG) {
    Serial.begin(9600);
  }
}

void loop() {
  int soilMoisture = getSoilMoisture();
  display.showNumberDec(soilMoisture, false);

  Serial.print(soilMoisture);
  Serial.print("\n");

  if (soilMoisture <= DRY_SOIL) {
    activatePump(PUMP_ACTIVATION_TIMEOUT);
  }

  delay(1000);
}
