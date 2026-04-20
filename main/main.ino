bool isOn = false;
float smoothed = 0.0;
unsigned long pendingSince = 0;

void setup() {
  Serial.begin(115200);
}

void loop() {
  long sum = 0;
  const int samples = 60;   // was 150

  for (int i = 0; i < samples; i++) {
    sum += analogRead(A0);
    delay(1);
  }

  float avgCounts = (float)sum / samples;
  float volts = avgCounts * (5.0 / 1023.0);

  const float alpha = 0.40; // was 0.30
  smoothed = (alpha * volts) + ((1.0 - alpha) * smoothed);

  const float onThreshold = 0.0155;
  const float offThreshold = 0.0115;
  const unsigned long confirmMs = 150; // was 300

  if (!isOn) {
    if (smoothed >= onThreshold) {
      if (pendingSince == 0) pendingSince = millis();
      else if (millis() - pendingSince >= confirmMs) {
        isOn = true;
        pendingSince = 0;
      }
    } else {
      pendingSince = 0;
    }
  } else {
    if (smoothed <= offThreshold) {
      if (pendingSince == 0) pendingSince = millis();
      else if (millis() - pendingSince >= confirmMs) {
        isOn = false;
        pendingSince = 0;
      }
    } else {
      pendingSince = 0;
    }
  }

  Serial.print(avgCounts, 1);
  Serial.print(",");
  Serial.print(volts, 4);
  Serial.print(",");
  Serial.print(smoothed, 4);
  Serial.print(",");
  Serial.println(isOn ? 1 : 0);

  delay(10);
}