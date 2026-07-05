# Nyx – Autonomous Security Patrol Robot

> Final Project | CPRE 2880 – Embedded Systems | Iowa State University

## Overview

Nyx is an autonomous security patrol robot designed to monitor a designated museum exhibit room. The robot continuously patrols its environment, detects obstacles and potential intruders, and can be manually controlled by on-duty security personnel when remote inspection is needed.

During operation, Nyx generates a live map of detected obstacles while simultaneously streaming a live camera feed, allowing users to monitor the environment in real time.

This project combines embedded systems, low-level programming, sensor integration, robotics, and autonomous navigation to simulate a practical security monitoring system.

---

## Problem Statement

Large buildings such as museums require continuous monitoring to improve security while reducing the need for constant human patrols.

Nyx was designed to explore how an embedded robotic platform can autonomously patrol an indoor environment, detect obstacles, communicate with onboard sensors, and provide security personnel with real-time situational awareness.

---

## Project Features

- Autonomous room patrol
- Obstacle detection and avoidance
- Manual remote control mode
- Real-time obstacle mapping
- Live camera streaming
- UART-based sensor communication
- Servo-controlled sensor scanning
- Embedded C implementation on the Tiva TM4 microcontroller

---

## Technologies & Concepts

- Embedded C
- Tiva TM4 Microcontroller
- UART Communication
- I2C Communication
- Servo Control
- ADC
- Autonomous Navigation
- Sensor Integration
- Robotics
- Embedded Systems

---

## Repository Contents

This repository includes:

- Embedded C source code
- Hardware interface modules
- Robot control logic
- Navigation and obstacle detection algorithms
- Wiring sketches and design diagrams
- Testing documentation
- Demonstration videos
- Final project report

---

## Team

This project was completed as the final project for **CPRE 2880 – Embedded Systems** at **Iowa State University**.

Team Members

- Rishita Kollu
- Olivia Blaise
- Lavanya Vangapandu
- Kaley Johnson

Course Instructor

- Professor Diane Rover

---

## Acknowledgements

This project was developed using the course framework and starter files provided for CPRE 2880.

Core infrastructure files including components such as the timer, LCD, and I2C drivers were provided as part of the course materials and served as the project's hardware abstraction framework.

Building upon that foundation, our team designed, implemented, integrated, and tested the primary functionality of the robot, including modules responsible for sensing, movement, obstacle detection, alarms, UART communication, servo control, system integration, and autonomous behavior.

The majority of the application logic and project-specific functionality contained in this repository was collaboratively developed by our team throughout the semester.

Special thanks to Professor Diane Rover for designing the project and providing guidance throughout the course.
