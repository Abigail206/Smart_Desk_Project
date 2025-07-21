#include <Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);

// Pins
#define FSR_LEFT_PIN A0
#define FSR_RIGHT_PIN A1
#define BUZZER_PIN 9

// Thresholds
#define MOTION_THRESHOLD 2.0       // Angle change threshold
#define FSR_DIFF_THRESHOLD 100     // Pressure imbalance threshold
#define FSR_MIN_TOTAL 50           // Minimum total to detect someone sitting

// Previous angles
float prevX = 0, prevY = 0, prevZ = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  pinMode(FSR_LEFT_PIN, INPUT);
  pinMode(FSR_RIGHT_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  byte status = mpu.begin();
  if (status != 0) {
    Serial.print("MPU6050 init failed with code: ");
    Serial.println(status);
    while (1);
  }

  delay(1000);
  mpu.calcOffsets(true, true);

  mpu.update();  // First reading
  prevX = mpu.getAngleX();
  prevY = mpu.getAngleY();
  prevZ = mpu.getAngleZ();

  // CSV header for serial output
  Serial.println("angleX,angleY,angleZ,fsrLeft,fsrRight,fsrDiff");
}

void loop() {
  mpu.update();

  // Get angles
  float angleX = mpu.getAngleX();
  float angleY = mpu.getAngleY();
  float angleZ = mpu.getAngleZ();

  // Get FSR readings
  int fsrLeft = analogRead(FSR_LEFT_PIN);
  int fsrRight = analogRead(FSR_RIGHT_PIN);
  int fsrDiff = fsrLeft - fsrRight;
  int fsrTotal = fsrLeft + fsrRight;

  // Motion detection
  float diffX = abs(angleX - prevX);
  float diffY = abs(angleY - prevY);
  float diffZ = abs(angleZ - prevZ);

  // Alert conditions
  bool motionAlert = (diffX > MOTION_THRESHOLD || diffY > MOTION_THRESHOLD || diffZ > MOTION_THRESHOLD);
  bool pressureAlert = (fsrTotal > FSR_MIN_TOTAL && abs(fsrDiff) > FSR_DIFF_THRESHOLD);

  // 🔔 Buzzer control
  if (motionAlert || pressureAlert) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(300);
    digitalWrite(BUZZER_PIN, LOW);
  }

  // CSV output for Python
  Serial.print(angleX); Serial.print(",");
  Serial.print(angleY); Serial.print(",");
  Serial.print(angleZ); Serial.print(",");
  Serial.print(fsrLeft); Serial.print(",");
  Serial.print(fsrRight); Serial.print(",");
  Serial.println(fsrDiff);

  // Update previous angles
  prevX = angleX;
  prevY = angleY;
  prevZ = angleZ;

  delay(500); // Adjust as needed
}