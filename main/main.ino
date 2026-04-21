bool isOn = false;
float smoothed = 0.0;
unsigned long pendingSince = 0;

const int sensorPin = A0;
const int thresholdPotPin = A1;

// Signal smoothing
const float alpha = 0.40;

// Timing
const unsigned long confirmOnMs = 120;
const unsigned long confirmOffMs = 250;

// Threshold range in volts for the useful sensor window
const float thresholdMinV = 0.006;
const float thresholdMaxV = 0.025;

// Hysteresis in volts
const float hysteresisV = 0.0015;

// Sampling
const int samples = 60;
const int sampleDelayMs = 1;
const int loopDelayMs = 25;

void setup() {
  Serial.begin(115200);
}

void loop() {
  long sumSensor = 0;

  for (int i = 0; i < samples; i++) {
    sumSensor += analogRead(sensorPin);
    delay(sampleDelayMs);
  }

  float avgSensorCounts = (float)sumSensor / samples;
  float sensorVolts = avgSensorCounts * (5.0 / 1023.0);

  smoothed = (alpha * sensorVolts) + ((1.0 - alpha) * smoothed);

  int potRaw = analogRead(thresholdPotPin);
  float thresholdVolts =
      thresholdMinV + ((thresholdMaxV - thresholdMinV) * potRaw / 1023.0);

  if (!isOn) {
    if (smoothed >= thresholdVolts) {
      if (pendingSince == 0) {
        pendingSince = millis();
      } else if (millis() - pendingSince >= confirmOnMs) {
        isOn = true;
        pendingSince = 0;
      }
    } else {
      pendingSince = 0;
    }
  } else {
    if (smoothed <= (thresholdVolts - hysteresisV)) {
      if (pendingSince == 0) {
        pendingSince = millis();
      } else if (millis() - pendingSince >= confirmOffMs) {
        isOn = false;
        pendingSince = 0;
      }
    } else {
      pendingSince = 0;
    }
  }

  // Plotter-friendly output:
  // raw sensor volts, smoothed volts, threshold volts, on/off marker
  // Serial.print(sensorVolts, 4);
  // Serial.print(",");
  Serial.print(smoothed, 4);
  Serial.print(",");
  Serial.print(thresholdVolts, 4);
  Serial.print(",");
  Serial.println(isOn ? 0.022 : 0.007, 4);

  delay(loopDelayMs);
}