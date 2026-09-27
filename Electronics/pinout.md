# Pinout

Microcontroller: ESP32 (DevKit style board)
Motor driver: dual H bridge (L298N style, IN1 to IN4 plus ENA and ENB)
Servos: 4 total, 3 for the arm and 1 for the claw

## Drive motors

| Signal | ESP32 GPIO | Connects to | Notes |
|---|---|---|---|
| IN1 | 2 | Motor driver IN1 | Left motor direction |
| IN2 | 15 | Motor driver IN2 | Left motor direction |
| ENA | 4 | Motor driver ENA | Left motor enable (on/off only, see notes) |
| IN3 | 27 | Motor driver IN3 | Right motor direction |
| IN4 | 26 | Motor driver IN4 | Right motor direction |
| ENB | 25 | Motor driver ENB | Right motor enable (on/off only, see notes) |

### Direction logic

| Left motor | IN1 | IN2 | ENA |
|---|---|---|---|
| Forward | HIGH | LOW | HIGH |
| Reverse | LOW | HIGH | HIGH |
| Stop | LOW | LOW | LOW |

The right motor follows the same pattern with IN3, IN4, and ENB.

## Servos

| Servo | ESP32 GPIO | Function | Start position | Allowed range |
|---|---|---|---|---|
| S1 | 19 | Arm joint 1 (TODO: name, e.g. base) | 180° | 0° to 180° |
| S2 | 21 | Arm joint 2 (TODO: name, e.g. shoulder) | 90° | 0° to 180° |
| S3 | 22 | Arm joint 3 (TODO: name, e.g. elbow) | 90° | 0° to 180° |
| S4 | 23 | Claw | 45° | 10° (closed) to 90° (open) |

All servos use a 500 to 2400 μs pulse range. Each button press moves a servo 2°.

## Power

TODO: fill in from the actual build.

| Supply | Voltage | Powers |
|---|---|---|
| Battery | ? V | Motor driver motor input |
| ? | 5 V | Servos |
| ? | 5 V or USB | ESP32 |

All grounds (battery, motor driver, servo supply, ESP32) must be connected together.

## Control

The ESP32 runs as a WiFi access point. Connect a phone to its network and open `192.168.4.1` in a browser to get the control page.

* **Drive mode:** drive buttons active, arm servos locked
* **Arm mode:** arm servos active, drive motors locked and stopped
* **Claw:** works in both modes
* **Stop:** cuts both drive motors

## Notes

* **ENA and ENB are driven fully on or off,** not with PWM, so the motors only run at full speed. Switching these to PWM would allow speed control.
* **GPIO 2 and GPIO 15 are ESP32 boot strapping pins.** They can interfere with uploading or booting if the motor driver pulls them the wrong way at power up. GPIO 2 also drives the onboard LED on most boards, so the LED lights when the left motor runs forward. If uploads ever fail, unplug the motor driver and try again, or move these to safer pins such as 32 and 33.
