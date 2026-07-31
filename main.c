#include <LPC213x.H>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* For atoi and atof functions */
// Delay function
void delay_ms(unsigned int ms) {
unsigned int i, j;
for (i = 0; i < ms; i++) {
    for (j = 0; j < 10000; j++); // Approximate delay
}
}
// LCD Control Pins
#define LCD_RS (1 << 0) // P0.0
#define LCD_EN (1 << 1) // P0.1
#define LCD_DATA_OFFSET 8 // Data pins start at P0.8
// Output Control Pins
#define MOTOR_PIN1 (1 << 26) // P1.26
#define MOTOR_PIN2 (1 << 27) // P1.27
#define MOTOR_PIN3 (1 << 28) // P1.28
#define LED_PIN (1 << 24) // P1.24
#define BUZZER_PIN (1 << 25) // P1.25
// LCD Commands
#define LCD_CLEAR 0x01
#define LCD_HOME 0x02
#define LCD_ENTRY_MODE 0x06
#define LCD_DISPLAY_ON 0x0C
#define LCD_FUNCTION_SET 0x38 // 8-bit, 2 lines, 5x8 dots
#define LCD_SET_CURSOR 0x80
// Keypad configuration
#define KEYPAD_ROW_START 16 // P1.16-P1.19 (rows)
#define KEYPAD_COL_START 20 // P1.20-P1.23 (columns)
// Item definitions
struct Item {
char name[10];
int price;
};
// Global variables
struct Item items[4] = {
{"Candy", 500}, // Price in cents
{"Juice", 1000},
{"Chips", 1500},
{"Cola", 2000}
};
// Function prototypes
void lcd_init(void);
void lcd_cmd(unsigned char command);
void lcd_data(unsigned char data);
void lcd_print(const char *str);
void lcd_set_cursor(unsigned char row, unsigned char col);
void lcd_clear(void);
unsigned char keypad_scan(void);
unsigned char get_key(void);
void motor_forward(void);
void motor_backward(void);
void motor_stop(void);
void led_on(void);
void led_off(void);
void buzzer_on(void);
void buzzer_off(void);
void dispense_item(void);
void display_menu(void);
int get_selection(void);
int get_quantity(void);
int get_payment(int total);
void show_balance(int balance);
void thank_you_message(void);
int main(void) {
int selection, quantity, total_price, payment, balance;
char price_str[10];
// Initialize IO pins
PINSEL0 = 0; // Set P0.0-P0.15 as GPIO
PINSEL1 = 0; // Set P0.16-P0.31 as GPIO
PINSEL2 = 0; // Set P1.0-P1.31 as GPIO
// Set directions
IODIR0 |= 0xFF << LCD_DATA_OFFSET; // P0.8-P0.15 as outputs for LCD data
IODIR0 |= LCD_RS | LCD_EN; // P0.0 and P0.1 as outputs for LCD control
IODIR1 |= MOTOR_PIN1 | MOTOR_PIN2 | MOTOR_PIN3; // Motor pins as outputs
IODIR1 |= LED_PIN | BUZZER_PIN; // LED and buzzer as outputs
// Set rows as outputs and columns as inputs for keypad
IODIR1 |= (0x0F << KEYPAD_ROW_START); // P1.16-P1.19 as outputs (rows)
IODIR1 &= ~(0x0F << KEYPAD_COL_START); // P1.20-P1.23 as inputs (columns)
IOSET1 = (0x0F << KEYPAD_ROW_START); // Set rows high
// Initialize LCD
lcd_init();
while (1) {
// 1. Display menu
display_menu();
// 2. Get user selection
selection = get_selection();
// 3. Get quantity
quantity = get_quantity();
// Display item price
lcd_clear();
// lcd_print(items[selection].name);
lcd_print("Price: $");
sprintf(price_str, "%d.%02d", items[selection].price / 100, items[selection].price % 100);
lcd_print(price_str);
lcd_set_cursor(1, 0);
lcd_print("Quantity: ");
sprintf(price_str, "%d", quantity);
lcd_print(price_str);
delay_ms(800);
// 4. Calculate and display total price
total_price = items[selection].price * quantity;
lcd_clear();
lcd_print("Total: $");
sprintf(price_str, "%d.%02d", total_price / 100, total_price % 100);
lcd_print(price_str);
delay_ms(1000);
// 5. Get payment
payment = get_payment(total_price);
// 6. Dispense if payment is sufficient
if (payment >= total_price) {
// Calculate balance
balance = payment - total_price;
// Dispense item
dispense_item();
// 7. Show balance
show_balance(balance);
// 8. Thank you message and restart
thank_you_message();
} else {
lcd_clear();
lcd_print("Insufficient");
lcd_set_cursor(1, 0);
lcd_print("Payment!");
delay_ms(1000);
}
}
}
// Initialize LCD
void lcd_init(void) {
delay_ms(50); // Wait for LCD to power up
lcd_cmd(LCD_FUNCTION_SET); // 8-bit, 2 lines, 5x8 dots
delay_ms(5);
lcd_cmd(LCD_FUNCTION_SET); // Repeat
delay_ms(1);
lcd_cmd(LCD_FUNCTION_SET); // Repeat
lcd_cmd(LCD_DISPLAY_ON); // Display on, cursor off, blink off
lcd_cmd(LCD_CLEAR); // Clear display
delay_ms(2);
lcd_cmd(LCD_ENTRY_MODE); // Increment cursor, no shift
delay_ms(2);
}
// Send command to LCD
void lcd_cmd(unsigned char command) {
// RS = 0 for command
IOCLR0 = LCD_RS;
// Put command on data bus
IOCLR0 = 0xFF << LCD_DATA_OFFSET;
IOSET0 = (command << LCD_DATA_OFFSET);
Generate enable pulse
IOSET0 = LCD_EN;
delay_ms(1);
IOCLR0 = LCD_EN;
delay_ms(2);
}
// Send data to LCD
void lcd_data(unsigned char data) {
// RS = 1 for data
IOSET0 = LCD_RS;
// Put data on data bus
IOCLR0 = 0xFF << LCD_DATA_OFFSET;
IOSET0 = (data << LCD_DATA_OFFSET);
// Generate enable pulse
IOSET0 = LCD_EN;
delay_ms(1);
IOCLR0 = LCD_EN;
delay_ms(2);
}
// Print string to LCD
void lcd_print(const char *str) {
while (*str) {
lcd_data(*str++);
}
}
// Set LCD cursor position
void lcd_set_cursor(unsigned char row, unsigned char col) {
unsigned char address;
if (row == 0) {
address = 0x80 + col; // First row starts at 0x80
} else {
address = 0xC0 + col; // Second row starts at 0xC0
}
lcd_cmd(address);
}
// Clear LCD display
void lcd_clear(void) {
lcd_cmd(LCD_CLEAR);
delay_ms(2);
}
// Scan keypad and return key value
unsigned char keypad_scan(void) {
unsigned char keypad[4][4] = {
{'7', '8', '9', '/'},
{'4', '5', '6', '*'},
{'1', '2', '3', '-'},
{'O', '0', '=', '+'}
};
unsigned char row, col;
// Check each row
for (row = 0; row < 4; row++) {
// Set all rows high
IOSET1 = (0x0F << KEYPAD_ROW_START);
// Set current row low
IOCLR1 = (1 << (KEYPAD_ROW_START + row));
// Small delay for signal to stabilize
delay_ms(1);
// Read column inputs
col = (IOPIN1 >> KEYPAD_COL_START) & 0x0F;
// Check if any key is pressed in this row
if (col != 0x0F) {
// Determine which column
for (col = 0; col < 4; col++) {
if (!(IOPIN1 & (1 << (KEYPAD_COL_START + col)))) {
return keypad[row][col];
}
}
}
}
return 0; // No key pressed
}
// Get key with debouncing
unsigned char get_key(void) {
unsigned char key = 0;
while (1) {
key = keypad_scan();
if (key != 0) {
delay_ms(20); // Debounce
if (key == keypad_scan()) {
// Wait for key release
while (keypad_scan() != 0);
delay_ms(20); // Debounce
return key;
}
}
delay_ms(10);
}
}
// Control motor to rotate forward
void motor_forward(void) {
IOSET1 = MOTOR_PIN1;
IOCLR1 = MOTOR_PIN2;
IOSET1 = MOTOR_PIN3; // Enable motor
}
// Control motor to rotate backward
void motor_backward(void) {
    IOCLR1 = MOTOR_PIN1;
IOSET1 = MOTOR_PIN2;
IOSET1 = MOTOR_PIN3; // Enable motor
}
// Stop motor
void motor_stop(void) {
IOCLR1 = MOTOR_PIN3; // Disable motor
}
// Turn LED on
void led_on(void) {
IOSET1 = LED_PIN;
}
// Turn LED off
void led_off(void) {
IOCLR1 = LED_PIN;
}
// Turn buzzer on
void buzzer_on(void) {
IOSET1 = BUZZER_PIN;
}
// Turn buzzer off
void buzzer_off(void) {
IOCLR1 = BUZZER_PIN;
}
// Display menu items
void display_menu(void) {
char price_str[10];
lcd_clear();
lcd_print("Vendo_Matic");
delay_ms(200);
lcd_set_cursor(1, 0);
lcd_print("YOUR SNACK HUB.!");
delay_ms(300);
lcd_clear();
lcd_print("1:Candy $");
sprintf(price_str, "%d.%02d", items[0].price / 100, items[0].price % 100);
lcd_print(price_str);
lcd_set_cursor(1, 0);
lcd_print("2:Juice $");
sprintf(price_str, "%d.%02d", items[1].price / 100, items[1].price % 100);
lcd_print(price_str);
delay_ms(500);
lcd_clear();
lcd_print("3:Chips $");
sprintf(price_str, "%d.%02d", items[2].price / 100, items[2].price % 100);
lcd_print(price_str);
lcd_set_cursor(1, 0);
lcd_print("4:Cola $");
sprintf(price_str, "%d.%02d", items[3].price / 100, items[3].price % 100);
lcd_print(price_str);
delay_ms(500);
}
// Get user selection
int get_selection(void) {
int selection = 0;
unsigned char key;
lcd_clear();
lcd_print("Select (1-4):");
while (1) {
key = get_key();
if (key >= '1' && key <= '4') {
selection = key - '1'; // Convert to 0-based index
lcd_set_cursor(1, 0);
lcd_print("Selected: ");
lcd_data(key);
delay_ms(500);
return selection;
}
}
}
// Get quantity
int get_quantity(void) {
int quantity = 0;
unsigned char key;
char buffer[5] = "";
int pos = 0;
lcd_clear();
lcd_print("Quantity.?(1-9)");
lcd_set_cursor(1, 0);
while (1) {
key = get_key();
if (key >= '0' && key <= '9' && pos < 2) {
buffer[pos++] = key;
buffer[pos] = '\0';
lcd_set_cursor(1, 0);
lcd_print(buffer);
} else if (key == '=' && pos > 0) {
quantity = atoi(buffer);
if (quantity > 0 && quantity <= 9) {
delay_ms(300);
return quantity;
} else {
lcd_set_cursor(1, 0);
lcd_print("Invalid! Try again");
delay_ms(300);
lcd_set_cursor(1, 0);
lcd_print(" ");
lcd_set_cursor(1, 0);
pos = 0;
buffer[0] = '\0';
}
}
}
}
// Get payment
int get_payment(int total) {
int payment = 0;
unsigned char key;
char buffer[7] = "";
int pos = 0;
lcd_clear();
lcd_print("Enter payment:");
lcd_set_cursor(1, 0);
lcd_print("$");
while (1) {
key = get_key();
if (key >= '0' && key <= '9' && pos < 5) {
buffer[pos++] = key;
buffer[pos] = '\0';
lcd_set_cursor(1, 1);
lcd_print(buffer);
// Format display to show dollars and cents
if (pos == 3) {
// Insert decimal point
memmove(buffer + 2, buffer + 1, pos);
buffer[1] = '.';
buffer[++pos] = '\0';
lcd_set_cursor(1, 1);
lcd_print(buffer);
}
} else if (key == '=' && pos > 0) {
// Convert string to cents
if (strchr(buffer, '.') != NULL) {
// Already has decimal point
payment = atof(buffer) * 100;
} else {
// No decimal point, assume whole dollars
payment = atoi(buffer) * 100;
}
delay_ms(500);
return payment;
}
}
}
// Dispense item
void dispense_item(void) {
lcd_clear();
lcd_print("Dispensing...");
// Turn on motor
motor_forward();
delay_ms(2000);
// Turn on LED and buzzer
led_on();
buzzer_on();
delay_ms(500);
buzzer_off();
lcd_clear();
lcd_print("Item dispensed!");
delay_ms(300);
// Return motor to original position
motor_backward();
delay_ms(2000);
motor_stop();
// Turn off LED
led_off();
}
// Show balance
void show_balance(int balance) {
char balance_str[10];
lcd_clear();
lcd_print("Your change:");
lcd_set_cursor(1, 0);
lcd_print("$");
sprintf(balance_str, "%d.%02d", balance / 100, balance % 100);
lcd_print(balance_str);
delay_ms(1000);
}
// Display thank you message
void thank_you_message(void) {
lcd_clear();
lcd_print("Thank you!");
lcd_set_cursor(1, 0);
lcd_print("Visit again!");
delay_ms(500);
}
