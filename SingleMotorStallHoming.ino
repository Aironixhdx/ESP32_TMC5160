#include <TMCStepper.h>
#include <SPI.h>

// SPI Pins (ESP32-S3)
#define CS_PIN2 20  // Chip Select for Driver 2

#define MOSI_PIN 11
#define MISO_PIN 13
#define SCK_PIN 12

// Stepper Control Pins (Motor 2)
#define STEP_PIN2 6
#define DIR_PIN2 7

// Stepper Motor Settings
#define STEPS_PER_REV 200  // Full steps per revolution
#define MICROSTEPS 16      // Microstepping value

// StallGuard Sensitivity (Adjust as needed)
#define SG_HOMING -10  // Sensitivity for Homing (Lower = More Sensitive)
#define SG_NORMAL 6   // Sensitivity for Normal Operation

TMC5160Stepper driver2 = TMC5160Stepper(CS_PIN2);

// Function to Initialize Motor 2 with StallGuard
void setupDriver(TMC5160Stepper &driver, int csPin, int sgThresh) {
    pinMode(csPin, OUTPUT);
    digitalWrite(csPin, HIGH);  // Disable SPI by default

    driver.begin();
    driver.toff(8);             // SpreadCycle
    driver.rms_current(500);   // Set motor current
    driver.ihold(2);            // 25% holding current
    driver.en_pwm_mode(false);  // Ensure StealthChop is OFF (Required for StallGuard)
    driver.pwm_autoscale(true);
    driver.microsteps(MICROSTEPS);

    driver.semin(0);
    driver.sedn(0);
    driver.TCOOLTHRS(50000);  // Ensure StallGuard is active at most speeds
    driver.sfilt(false);

    driver.sgt(sgThresh);  // Set StallGuard Sensitivity
}

// Function to Perform Auto-Homing Using StallGuard
void stallHoming(TMC5160Stepper &driver, int stepPin, int dirPin, int sgHomingThresh) {
    Serial.println("Homing Motor 2...");

    driver.sgt(sgHomingThresh);  // Set StallGuard sensitivity for homing
    digitalWrite(dirPin, LOW);   // Move towards home position

    int homingStepDelay = 1000 / MICROSTEPS;  // Slow speed for better accuracy
    int stallValue = 0;

    while (true) {  
        stallValue = driver.sg_result(); // Get StallGuard result
        Serial.print("StallGuard Value (Homing) - Motor 2: ");
        Serial.println(stallValue);

        if (stallValue == 0) {  // Stall detected
            Serial.println("Motor 2 stalled, homing complete.");
            break;
        }

        // Step the motor
        digitalWrite(stepPin, HIGH);
        delayMicroseconds(100);
        digitalWrite(stepPin, LOW);
        delayMicroseconds(homingStepDelay);
    }

    driver.XACTUAL(0);  // Set current position as zero (Home)
    driver.sgt(SG_NORMAL);  // Restore normal StallGuard sensitivity
}

// Function to Move Motor 2 Forward & Backward After Homing
void moveMotor2(int steps, bool direction, int speed) {
    digitalWrite(DIR_PIN2, direction ? HIGH : LOW);
    int stepDelay = map(speed, 0, 100, 5000, 500) / MICROSTEPS;

    for (int i = 0; i < steps * MICROSTEPS; i++) {
        digitalWrite(STEP_PIN2, HIGH);
        delayMicroseconds(16000);
        digitalWrite(STEP_PIN2, LOW);
        delayMicroseconds(stepDelay);
        
        // Read StallGuard Value During Motion
        Serial.print("StallGuard Value (Moving) - Motor 2: ");
        Serial.println(driver2.sg_result());
    }
}

// Setup Function
void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("Initializing TMC5160 for Motor 2...");

    // Initialize SPI
    SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN);

    // Setup Motor 2
    setupDriver(driver2, CS_PIN2, SG_NORMAL);

    // Set Step and Direction Pins
    pinMode(STEP_PIN2, OUTPUT);
    pinMode(DIR_PIN2, OUTPUT);

    // Perform Auto-Homing
    stallHoming(driver2, STEP_PIN2, DIR_PIN2, SG_HOMING);
    delay(1000);

    Serial.println("Homing sequence complete.");
}

// Loop to Move Motor 2 Forward & Backward After Homing
void loop() {
    Serial.println("Moving Motor 2 Forward...");
    moveMotor2(STEPS_PER_REV / 2, HIGH, 50);
    delay(2000);

    Serial.println("Moving Motor 2 Backward...");
    moveMotor2(STEPS_PER_REV / 2, LOW, 50);
    delay(2000);
}
