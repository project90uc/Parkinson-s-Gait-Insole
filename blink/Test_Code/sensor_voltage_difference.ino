/*
  Dual FSR voltage divider - Vout range + difference
  Board: ESP32-C6-WROOM-1 (Arduino-ESP32 core 3.x)

  Wiring (same divider for each sensor):
      3.3V ---[ FSR1 ]---+---[ R_FIXED ]--- GND
                         |
                       GPIO2  (ADC1_CH2)

      3.3V ---[ FSR2 ]---+---[ R_FIXED ]--- GND
                         |
                       GPIO3  (ADC1_CH3)

  Serial Monitor: 115200 baud
    'r' -> reset min/max for both sensors
  Tip: if nothing prints, set Tools > "USB CDC On Boot" > Enabled.
*/

const int   FSR1_PIN    = 2;        // GPIO0-GPIO6 are all ADC1
const int   FSR2_PIN    = 3;
const int   NUM_SAMPLES = 32;       // averaging to cut ADC noise

float v1Min = 99.0, v1Max = 0.0;
float v2Min = 99.0, v2Max = 0.0;
float dMin  = 99.0, dMax  = -99.0;  // difference can be negative

// Averaged, calibrated reading in volts
float readVolts(int pin) {
  uint32_t sum = 0;
  for (int i = 0; i < NUM_SAMPLES; i++) {
    sum += analogReadMilliVolts(pin);
    delayMicroseconds(200);
  }
  return (sum / (float)NUM_SAMPLES) / 1000.0;
}

void resetMinMax() {
  v1Min = v2Min = dMin = 99.0;
  v1Max = v2Max = 0.0;
  dMax  = -99.0;
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  analogReadResolution(12);
  analogSetPinAttenuation(FSR1_PIN, ADC_11db);  // ~0 to 3.1 V usable range
  analogSetPinAttenuation(FSR2_PIN, ADC_11db);

  Serial.println("Dual FSR test - press the sensors. Send 'r' to reset min/max.");
}

void loop() {
  float v1   = readVolts(FSR1_PIN);
  float v2   = readVolts(FSR2_PIN);
  float diff = v1 - v2;   // positive = FSR1 higher than FSR2

  v1Min = min(v1Min, v1);    v1Max = max(v1Max, v1);
  v2Min = min(v2Min, v2);    v2Max = max(v2Max, v2);
  dMin  = min(dMin, diff);   dMax  = max(dMax, diff);

  Serial.print("V1: ");  Serial.print(v1, 3);
  Serial.print(" V [");  Serial.print(v1Min, 3); Serial.print("-"); Serial.print(v1Max, 3);
  Serial.print("] | V2: "); Serial.print(v2, 3);
  Serial.print(" V [");  Serial.print(v2Min, 3); Serial.print("-"); Serial.print(v2Max, 3);
  Serial.print("] | V1-V2: "); Serial.print(diff, 3);
  Serial.print(" V [");  Serial.print(dMin, 3);  Serial.print(" to "); Serial.print(dMax, 3);
  Serial.println("]");

  if (Serial.available()) {
    char c = Serial.read();
    if (c == 'r' || c == 'R') {
      resetMinMax();
      Serial.println("--- min/max reset ---");
    }
  }

  delay(1000);
}

