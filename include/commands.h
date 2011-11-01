#ifndef _COMMANDS_
#define _COMMANDS_

#define Ghz 1
#define Mhz 2
#define Hz 3
#define MAX_COMMAND_BUFFER 15

/* Shows the commands functionality*/
void help();

/* When the up or down arrow are pressed, this function display the next
 * command in the buffer */
void displayCommand(int arrow);

/* A command to test one getc and a putc whith the character read*/
void getcTest();

/* Tests printf with float, with and without exponencial, and scanf with
 * float*/
void floatTest();

/* Prints Dinux first screen */
void welcome();

/* A command to test scanf */
void scanfTest();

/* Shows the available commands */
void commands();

/* Shows system info */
void info();

/* Calculates three times the CPU speed with method 1 and 2 and makes
 * an average, then prints it */
void getCPUSpeed();

/* Calculates the CPU Speed with method N°1 and returns it*/
float calculateSpeedMethod1();

/* Calculates the CPU Speed with method N°2 and returns it*/
float calculateSpeedMethod2();

/* According to the "speed" divides it by an specific number that makes
 * the unit smaller, and then sets the flag printKind so printSpeed
 * prints it correctly */
float convertSpeed(float speed);

/* Prints the speed result in the correct unit (Ghz, Mhz or hz) */
void printSpeed(float result);

/* Returns the actual cicles quantity */
unsigned int getRDTSC();

#endif