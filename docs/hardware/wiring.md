# Wiring

## Node 1
- MPU6050 SDA → GPIO21; SCL → GPIO22; VCC/GND → suitable supply/common ground.
- NEO-6M TX → GPIO16 (UART2 RX); GPS RX → GPIO17 (optional).
- Buzzer driver input → GPIO25.

Check the voltage requirements of the exact breakout. Do not drive a high-current buzzer directly from an ESP32 GPIO; use a suitable transistor/MOSFET driver.
