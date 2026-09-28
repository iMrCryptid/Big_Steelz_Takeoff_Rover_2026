# Big Steelz Takeoff Rover 2026

<!-- TODO: add your best photo of the finished rover to Media/Photos and uncomment the line below -->
<!-- ![Big Steelz rover](Media/Photos/rover.jpg) -->

A remote controlled rover with rocker bogie suspension and a four servo arm and gripper, built for the **TMU Takeoff Rover Building Competition** (February to May 2026). Driven from a phone over WiFi.

**Result: 🥇 First place in the Guess the Word challenge.**

<!-- TODO: add a demo video link here, e.g. [Watch it run](https://youtube.com/...) -->

## Challenges

**1. Time trial.** Drive an obstacle course with bumps and walls to the finish line as fast as possible.
<!-- TODO: how it went -->

**2. Block sorting.** A head to head match. Pick up blocks from the centre and place them on a three level shelf in a set colour sequence. Blocks score 1, 2, or 3 points depending on shelf height.
<!-- TODO: how it went, and how the arm and gripper performed -->

**3. Guess the Word (🥇 first place).** A laser pointer is mounted on the rover. One teammate knows an engineering term and drives the rover to point at letters one by one; another teammate writes guesses on a whiteboard, and can only guess after the third letter.

We mounted the laser on the arm rather than the chassis. The arm servos move in small 2° steps per button press, which turned out to be ideal for aiming: the pilot could make fine adjustments to hit individual letters instead of steering the whole rover onto a target.

## Hardware

| Component | Details |
|---|---|
| Microcontroller | ESP32 DevKit V1 (WROOM 32) |
| Motor driver | L298N dual H bridge |
| Drive | 6 TT gear motors (3 to 6 V), 3 per side |
| Suspension | Rocker bogie |
| Arm | 3 servos |
| Gripper | 1 servo |
| Power | 7.4 V 5200 mAh 2S LiPo |
| Structure | 3D printed in PLA |

<!-- TODO: name the arm joints (e.g. base, shoulder, elbow) in the Arm row -->

**Suspension:** a rocker bogie design, the same layout used on Mars rovers. Each side has a rocker and a bogie carrying three wheels, which pivot so all six wheels stay on the ground over uneven terrain, with no springs needed.
<!-- TODO: say whether the two sides are linked by a differential bar, and how it handled the time trial bumps -->

## Control

The ESP32 runs its own WiFi access point. Connect a phone to it and open `192.168.4.1` to load the control page.

* **Drive mode:** forward, reverse, and turning; arm locked
* **Arm mode:** arm servos active; drive motors stopped and locked
* **Claw:** open and close in either mode
* **Stop:** cuts both drive motors immediately

## Repository layout

```
CAD/STL/
├── Arm/          arm base and segments
├── Body/         body, lid, mount, cap
├── Gripper/      base, mount, pincher, rod, servo horn parts, cap
└── Suspension/   rockers, bogie, caps
Code/             ESP32 firmware (Arduino)
Electronics/      wiring diagram and pinout
Media/            photos and video
```

GitHub shows a 3D preview of any STL file in the browser, so you can inspect parts without downloading them.

## Electronics

![Wiring diagram](<Electronics/Wiring Diagram.png>)

Wire colours on the motor leads show orientation only; the L298N reverses polarity on its outputs to change direction. Every pin assignment is listed in [`Electronics/pinout.md`](Electronics/pinout.md).

## Building it

* All parts are printed in PLA. <!-- TODO: printer, layer height, infill -->
* Print the **cap** parts in each subsystem folder. They secure joints where the original tolerances came out loose, and the parts will not hold together without them.
* Set your own network name and password at the top of `Code/main.ino` before uploading.
* Upload with the Arduino IDE using the ESP32 board package and the `ESP32Servo` library.

## Team

| Member | Contributions |
|---|---|
| **Jeffrey Acquah** (team lead) | Designed and finalized the gripper and suspension; finalized the arm; designed all tolerance caps and reprinted parts as needed; responsible for wiring the full rover; cowrote the code and led troubleshooting across the full program; guesser in Guess the Word |
| **Kiara** | Programmed the drivetrain; drew diagrams for the preliminary round; helped assemble, wire, and troubleshoot |
| **Josiah** | Designed arm segments and the gripper mount; helped finalize the body mount; built an enclosure for an ultrasonic sensor; helped assemble; main pilot |
| **Pravani** | CAD drawings and most of the research for the preliminary round; coded the controller UI and WiFi connection; helped assemble |
| **Eric** | Designed the body and lid; programmed the arm; helped assemble; copilot, and pilot after Josiah left |

## What I'd change

* **Tolerances.** Many printed joints came out too loose, so I designed caps to secure them. Next time I'd print small test pieces first to dial in clearances before printing full parts.
* **Servo power.** The L298N's onboard 5 V regulator powers the ESP32 and all four servos. Servo current spikes can brown out the ESP32. A separate 5 V buck converter for the servos would fix this.
* **Motor driver.** Six motors on one L298N is close to its limit, and it wastes a lot of power as heat. A modern MOSFET based driver would be more efficient.
* **Speed control.** The enable pins are switched fully on or off, so the motors only run at full speed. Using PWM on ENA and ENB would allow slower, more precise driving.
* **Pin choice.** The left motor uses GPIO 2 and 15, which are ESP32 boot strapping pins and can cause upload problems. Moving them to pins like 32 and 33 avoids that.
* **Aiming.** The 2° servo step worked well for the laser, but we found it rather than tuned it. Next time I'd make the step size adjustable from the control page.
