bool isOn = false;

float smoothed = 0.0;
float baseline = 0.0;
float energy = 0.0;

const int sensorPin = A0;

// Fixed bias from your actual circuit behavior
const float biasVolts = 2.42;

// Signal smoothing
const float alphaSignal = 0.20;

// Baseline tracking speed while OFF
const float alphaBase = 0.0015;

const float energyGain = 0.75;
const float energyDecay = 0.75;

const float onEnergyThreshold = 0.020;
const float offEnergyThreshold = 0.010;

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

  // Remove fixed DC bias
  float corrected = sensorVolts - biasVolts;
  if (corrected < 0.0) corrected = 0.0;

  // Smooth corrected signal
  smoothed = (alphaSignal * corrected) + ((1.0 - alphaSignal) * smoothed);

  // Initialize baseline once
  if (baseline == 0.0) {
    baseline = smoothed;
  }

  // Let baseline drift only while OFF
  if (!isOn) {
    baseline = (alphaBase * smoothed) + ((1.0 - alphaBase) * baseline);
  }

  float delta = smoothed - baseline;
  if (delta < 0.0) delta = 0.0;

  // Accumulate activity over time
  energy = (energy * energyDecay) + (delta * energyGain);

  if (energy < 0.0) energy = 0.0;
  if (energy > 1.0) energy = 1.0;

  // ON/OFF decision
  if (!isOn) {
    if (energy >= onEnergyThreshold) {
      isOn = true;
    }
  } else {
    if (energy <= offEnergyThreshold) {
      isOn = false;
    }
  }

  // Plotter-friendly output:
  Serial.print(corrected, 4);
  Serial.print(",");
  Serial.println(isOn ? 1.0 : 0.0);

  delay(loopDelayMs);
}