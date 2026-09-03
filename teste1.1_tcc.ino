#include <Wire.h>

#define MPU6050_ADDR 0x68

// Pino I2C do ESP32
#define SDA_PIN 21
#define SCL_PIN 22

void setup() {

  Serial.begin(115200);

  // Inicializa I2C
  Wire.begin(SDA_PIN, SCL_PIN);

  delay(1000);

  Serial.println("=================================");
  Serial.println("TESTE DE VIBRACAO - MPU6050");
  Serial.println("=================================");

  // Verifica comunicacao com o MPU6050
  Wire.beginTransmission(MPU6050_ADDR);
  byte erro = Wire.endTransmission();

  if (erro != 0) {
    Serial.println("MPU6050 NAO ENCONTRADO!");
    
    while (1) {
      delay(1000);
    }
  }

  Serial.println("MPU6050 encontrado em 0x68");

  // ---------------------------------------------------
  // ACORDA O MPU6050
  // ---------------------------------------------------

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x6B);       // PWR_MGMT_1
  Wire.write(0x00);       // Wake up
  Wire.endTransmission();

  delay(100);

  // ---------------------------------------------------
  // CONFIGURACAO DO ACELEROMETRO
  // ---------------------------------------------------

  // Registrador ACCEL_CONFIG = 0x1C
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x1C);

  // 0x00 = ±2g
  Wire.write(0x00);

  Wire.endTransmission();

  // ---------------------------------------------------
  // CONFIGURACAO DO GIROSCOPIO
  // ---------------------------------------------------

  // Registrador GYRO_CONFIG = 0x1B
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x1B);

  // 0x00 = ±250 °/s
  Wire.write(0x00);

  Wire.endTransmission();

  // ---------------------------------------------------
  // CONFIGURACAO DO FILTRO
  // ---------------------------------------------------

  // CONFIG = 0x1A
  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x1A);

  // DLPF ≈ 21 Hz
  Wire.write(0x04);

  Wire.endTransmission();

  delay(100);

  Serial.println("Sensor configurado.");
  Serial.println();
}

void loop() {

  // ===================================================
  // POSICIONA O REGISTRADOR PARA LEITURA
  // ===================================================

  Wire.beginTransmission(MPU6050_ADDR);
  Wire.write(0x3B);

  // Repeated START
  Wire.endTransmission(false);

  // Leitura dos 14 registradores
  Wire.requestFrom(MPU6050_ADDR, 14, true);

  if (Wire.available() >= 14) {

    // =================================================
    // ACELEROMETRO
    // =================================================

    int16_t accelX = (Wire.read() << 8) | Wire.read();
    int16_t accelY = (Wire.read() << 8) | Wire.read();
    int16_t accelZ = (Wire.read() << 8) | Wire.read();

    // =================================================
    // TEMPERATURA
    // =================================================

    int16_t tempRaw = (Wire.read() << 8) | Wire.read();

    // =================================================
    // GIROSCOPIO
    // =================================================

    int16_t gyroX = (Wire.read() << 8) | Wire.read();
    int16_t gyroY = (Wire.read() << 8) | Wire.read();
    int16_t gyroZ = (Wire.read() << 8) | Wire.read();

    // =================================================
    // CONVERSAO DO ACELEROMETRO
    // =================================================

    // ±2g -> 16384 LSB/g

    float ax = accelX / 16384.0;
    float ay = accelY / 16384.0;
    float az = accelZ / 16384.0;

    // =================================================
    // CONVERSAO DO GIROSCOPIO
    // =================================================

    // ±250 °/s -> 131 LSB/(°/s)

    float gx = gyroX / 131.0;
    float gy = gyroY / 131.0;
    float gz = gyroZ / 131.0;

    // =================================================
    // TEMPERATURA
    // =================================================

    float temperatura = (tempRaw / 340.0) + 36.53;

    // =================================================
    // SAIDA PARA SERIAL PLOTTER
    // =================================================

    Serial.print("AX:");
    Serial.print(ax, 3);

    Serial.print("\tAY:");
    Serial.print(ay, 3);

    Serial.print("\tAZ:");
    Serial.println(az, 3);

    // =================================================
    // FREQUENCIA DE AMOSTRAGEM
    // =================================================

    // 10 ms -> aproximadamente 100 Hz
    delay(100);
  }
}