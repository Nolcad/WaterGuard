#include <Arduino.h>

#define VERSION "v1.0"

#define LED_PULSE D1
#define LED_ALARM D2
#define BUTTON_N D6
#define PULSE_N D7

#define LED_BLINK_PERIOD 250

// Leak monitoring period: 24 hours
#define MONITORING_PERIOD (24 * 60 * 60)
// #define MONITORING_PERIOD (30)

// Required inactivity time: 2 hours
#define REQUIRED_INACTIVITY_TIME (2 * 60 * 60)
// #define REQUIRED_INACTIVITY_TIME (15)

// Abnormal continuous activity time: 2 hours
#define ABNORMAL_ACTIVITY_TIME (2 * 60 * 60)
// #define ABNORMAL_ACTIVITY_TIME (60)

// Minimal activity detection time to define the minimum flowrate: 1 minute (1
// l/min)
#define MINIMAL_ACTIVITY_TIME (1 * 60)
// #define MINIMAL_ACTIVITY_TIME (5)

// True when a pulse is detected
bool isPulseActive = false;

// Timing variables
unsigned long now;
bool isPauseRegistered = false;
bool isMonitoringPeriodExpired = false;

// Time accumulator
unsigned long lastSecondTimer = 0;
unsigned long secondTimer = 0;

// Monitoring window
unsigned long lastMonitoringStart = 0;

// Pause tracking
unsigned long pauseTime = 0;
bool isPauseActive = false;

// Activity tracking
unsigned long lastActivityTime = 0;
unsigned long flowActiveTime = 0;
bool isFlowActive = false;

// Alarms
bool isLeakAlarm = false;
bool isAbnormalActivityAlarm = false;

// Blinking utilities
unsigned long lastBlinkTime;
bool currentBlinkState = false;

// Function prototypes
void ledControl(uint8_t ledPin, bool onOff);

// Setup function (called once when booting)
void setup() {
  // Start Serial console
  Serial.begin(115200);
  Serial.println("");
  Serial.printf("Starting (%s)\n", VERSION);

  // Initialize hardware
  pinMode(LED_PULSE, OUTPUT);
  pinMode(LED_ALARM, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BUTTON_N, INPUT_PULLUP);
  pinMode(PULSE_N, INPUT_PULLUP);

  ledControl(LED_PULSE, false);
  ledControl(LED_ALARM, false);
  ledControl(LED_BUILTIN, false);
}

// Led control logic (accounts for positive or negative logic)
void ledControl(uint8_t ledPin, bool onOff) {
  if (ledPin == LED_BUILTIN) {
    onOff = !onOff;
  }
  digitalWrite(ledPin, onOff ? HIGH : LOW);
}

// Loop function, called as often as possible
void loop() {
  now = millis();

  // Process seconds
  if (now - lastSecondTimer >= 1000) {
    lastSecondTimer = now;
    // If flow is active, increment active time
    if (isFlowActive) {
      flowActiveTime++;
    }
    // If pause is active, increment pause time
    if (isPauseActive) {
      pauseTime++;
    }

    secondTimer++;
    Serial.printf("Current time: %lu\n", secondTimer);
    Serial.printf("  - Monitoring Period: %lu s (Extending: %s)\n",
                  secondTimer - lastMonitoringStart,
                  isMonitoringPeriodExpired ? "Yes" : "No");
    Serial.printf("  - Pause Registered: %s (Pause time:  %lu s)\n",
                  isPauseRegistered ? "Yes" : "No", pauseTime);
    Serial.printf("  - Flow Active: %s (Active time: %lu s)\n",
                  isFlowActive ? "Yes" : "No", flowActiveTime);
  }

  // Process pulse input
  isPulseActive = false;
  if (digitalRead(PULSE_N) == LOW) {
    isPulseActive = true;
  }

  // Process button input
  if (digitalRead(BUTTON_N) == LOW) {
    lastMonitoringStart = secondTimer;
    isMonitoringPeriodExpired = false;
    pauseTime = 0;

    isFlowActive = false;
    flowActiveTime = 0;

    isAbnormalActivityAlarm = false;
    isLeakAlarm = false;
    ledControl(LED_ALARM, false);
  }

  // Activity check
  if (isPulseActive) {
    // Save last activity time
    lastActivityTime = secondTimer;

    // Pulse, so: active flow (for now...)
    isFlowActive = true;

    // Reset pause
    isPauseActive = false;
    pauseTime = 0;

    // Mirror input pulse on the LED
    ledControl(LED_PULSE, true);
  } else {
    // No pulse, pause may be starting...
    isPauseActive = true;

    // Mirror input pulse on the LED
    ledControl(LED_PULSE, false);
  }

  // Reset flow state when time is out (we are below the minimum)
  if (secondTimer - lastActivityTime >= MINIMAL_ACTIVITY_TIME) {
    isFlowActive = false;
    flowActiveTime = 0;
  }

  // Process pause time
  if (pauseTime >= REQUIRED_INACTIVITY_TIME) {
    // Pause was longer that required
    isPauseRegistered = true;
  }

  // Process monitoring time
  if (secondTimer - lastMonitoringStart >= MONITORING_PERIOD) {
    // Signal we are at the end of the monitoring time
    isMonitoringPeriodExpired = true;
  }

  // By using this expiration flag, we let a pause go on and allow it
  // to expand over the monitoring period.
  // But as soon has we detect a pulse, we process the monitoring window
  if (isMonitoringPeriodExpired && isPulseActive) {
    // If a long enough pause was registered
    if (isPauseRegistered) {
      // Everything is OK
    } else {
      // Alarm!
      isLeakAlarm = true;
    }
    // Clear and restart from here
    isPauseRegistered = false;
    isMonitoringPeriodExpired = false;
    lastMonitoringStart = secondTimer;
  }

  // Process abnormal flow duration
  if (flowActiveTime >= ABNORMAL_ACTIVITY_TIME) {
    isAbnormalActivityAlarm = true;
  }

  // Process alarms
  if (isAbnormalActivityAlarm) {
    ledControl(LED_ALARM, currentBlinkState);
  } else if (isLeakAlarm) {
    ledControl(LED_ALARM, true);
  } else {
    ledControl(LED_ALARM, false);
  }

  // Update blinker
  if (now - lastBlinkTime >= LED_BLINK_PERIOD) {
    lastBlinkTime = now;
    currentBlinkState = !currentBlinkState;
    ledControl(LED_BUILTIN, currentBlinkState);
  }
}
