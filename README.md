# ARM7 LPC2138 Vending Machine Simulator

## Overview

This project implements an automated vending machine using the ARM7 LPC2138 microcontroller. The system simulates product selection, payment validation, and product dispensing through an interactive LCD and keypad interface.

The project was developed to demonstrate embedded system concepts such as GPIO interfacing, LCD communication, keypad scanning, and finite state machine implementation.

---
## Working Principle
Working principle follows a sequential state machine model, beginning with initialization where the system configures all GPIO pins and peripherals. During normal operation, the microcontroller first displays items on the LCD, then processes user selection through the keypad interface. After receiving valid item and quantity inputs, the system calculates the total price and prompts for payment. The payment processing includes amount entry and validation against the calculated total. For successful transactions, the system activates the dispensing sequence - energizing the motor for timed forward rotation while simultaneously triggering the LED and buzzer. The motor then reverses to reset the mechanism, completing the physical dispensing simulation. Finally, the system calculates and displays any change due, shows a thank you message, and resets.
---

## Features

- Product selection through keypad
- LCD-based user interface
- Simulated payment system
- Product dispensing logic
- Inventory management
- Invalid input handling
- User-friendly menu navigation

---

## Hardware Components

- ARM7 LPC2138 Microcontroller
- 16x2 LCD Display
- Matrix Keypad
- Push Buttons
- LEDs
- Power Supply

---

## Software Used

- Embedded C
- Proteus 8 Professional

---

## Project Workflow

1. Power on the system.
2. Display available products.
3. User selects a product.
4. Product price is displayed.
5. User enters payment.
6. System validates payment.
7. Product is dispensed.
8. Inventory is updated.
9. LCD displays transaction completion.

---

## Folder Structure

```
Code/
Circuit/
Images/
```

## Future Enhancements

- RFID card payment
- QR code payment
- Coin and currency detection
- IoT inventory monitoring
- Mobile application integration

---
