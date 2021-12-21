/******************************************************************************
* Library:   Library data types and functions for lift control
* Filename:  LiftLibrary.h
*
* Author:    Werner Odermatt
* Version:   2.0
* Date:	     14.09.2016
*
* Development flow(Version, Date, Author, Development step, Time):
* 1.0   25.05.15   WOd   Functions created
* 2.0   07.09.16   WOd   Revision of software, Code-Guidelines C#
********************************************************************************
*
* Using: C-Training, M121, M242
*
* Description:
* Library functions for controlling a lift model.
*
* Precondition:  -
*
* Postcondition: -
*
* Required Libraries:
* - avr/io.h
*
* Created Functions:
* - InitializePorts()
* - InitializeStart()
* - CalibrateElevatorPosition()
* - SetOutput()
* - ReadKeyEvent()
* - ReadDoorState()
* - SetDoorState()
* - MoveElevator()
* - ReadElevatorState()
* - SetDisplay()
* - SetIndicatorFloorState()
* - ClrIndicatorFloorState()
* - SetIndicatorElevatorState()
* - ClrIndicatorElevatorState()
* - TestLiftsimulator()
*
* Copyright (c) 2016 by W.Odermatt, CH-6340 Baar
*******************************************************************************/


/***  Include Files ***********************************************************/
#include <avr/io.h>   // Function library: I/O Interface AVR micro controller


/******************************************************************************/
/*** Own Data Types ***********************************************************/
/******************************************************************************/
typedef enum 
{
  Floor0_D  = 1, Floor1_D  = 2, Floor2_D  = 4, Floor3_D  = 8
}
FloorDoorType;

typedef enum 
{ 
  EmergencyButton = 0,
	LiftButton_F0 = 1, LiftButton_F1 = 2, LiftButton_F2 = 4,
	LiftButton_F3 = 8,
	FloorButton_F0 = 16, FloorButton_F1 = 32,
  FloorButton_F2 = 64, FloorButton_F3 = 128
}
ButtonType;

typedef enum 
{
  Door100 = 0xF0, Door75 = 0x70, Door50 = 0x30, Door25 = 0x10,
  Door00 = 0x00
}
DoorPosType;

typedef enum 
{
  Closed, Moves, Open
}
DoorStateType;

typedef enum 
{
  On, Off
}
DisplayStateType;

typedef enum 
{
  Released, Pressed
}
ButtonStateType;

typedef enum 
{
  Floor0 = 0, Floor1 = 1, Floor2 = 2, Floor3 = 3, Error = 4,
	Test = 6, None = 7, LiftMoves = 100, LiftStopped = 101,
  UpperStop = 200, Overload = 210
}
LiftPosType;

typedef enum 
{
  Down, Up
}
DirectionType;

typedef enum 
{
  Stop = 0, Slow = 4000, Medium = 2000, Fast = 1000
}
SpeedType;

typedef enum 
{
  Uninitialized = 0, Waiting, CloseDoor, MoveLift, OpenDoor, Trouble
}
StateMachineType;


/******************************************************************************/
/*** GLOBAL CONSTANT **********************************************************/
/******************************************************************************/

/** SYSTEM SETTINGS **/
// Clock frequency of the controller
//#define F_CPU		8000000UL   // 8 MHz


/** PROGRAM SETTINGS**/
#define maxDoors                4   // Number of available doors
#define liftDoorOpeningTime  5000   // Speed ​​indication for lift door opening
#define liftMaxPos             49   // highest position value of the lift


/** PORT- und PINZUWEISUNGEN **/
// PORT A: 
// 7-Seg.: Pin 0-2; 
// Floor Selection display: Pin 3-4; 
// Call buttons display: Pin 5-7
#define liftDisplay_D     DDRA 
#define liftDisplay       PORTA

// PORT B:
// Lift position: Pin 0-5
// Door simulation LED:1_10: Pin 6 (LOW = ON)
// Floor Selection display: Pin 7 (LOW = ON)
#define liftPos_D         DDRB
#define liftPos           PORTB

// PORT C
// Door simulation: Pin 0-3; Floor selection (HIGH = ON)
// LED:5_6: Pin 4
// LED:4_7: Pin 5
// LED:3_8: Pin 6
// LED:2_9: Pin 7
#define liftDoors_D       DDRC
#define liftDoors         PORTC

// PORT D:
// Floor selection button inside lift: Pin 0-3
// Call button per floor: Pin 4-7
#define buttons_D         DDRD
#define buttons           PIND


/******************************************************************************/
/*** GLOBAL VARIABLES *********************************************************/
/******************************************************************************/
// None


/******************************************************************************/
/*** GLOBAL FUNCTIONS *********************************************************/
/******************************************************************************/
// Initialize ports
void InitializePorts();

// Calibrating the passenger cabin at the position: Floor0
void CalibrateElevatorPosition();

// Control of output ports
void SetOutput();


// Set the initial state (I/O); Public-Function
void InitializeStart();

// Reading the state of a particular button; Public-Function
ButtonStateType ReadKeyEvent (ButtonType button);

// Reading the state of a floor door; Public-Function
DoorStateType ReadDoorState (LiftPosType floor);

// Set the door state per floor; Public-Function
void SetDoorState (DoorStateType state, LiftPosType floor);

// Move the passenger cabin (Lift)
void MoveElevator (DirectionType direction, SpeedType speed);

// Reading the lift state; Public-Function
LiftPosType ReadElevatorState();

// Set the position indicator in the lift(7-Seg. Display); Public-Function
void SetDisplay (LiftPosType displayValue);

// Set the call button indicator per floor; Public-Function
void SetIndicatorFloorState (LiftPosType floor);

// Set the call button indicator inside lift; Public-Function
void SetIndicatorElevatorState (LiftPosType floor);

// Clear the call button indicator per floor; Public-Function
void ClrIndicatorFloorState (LiftPosType floor);

// Clear the call button indicator inside lift; Public-Function
void ClrIndicatorElevatorState (LiftPosType floor);

// Test for checking the lift simulator board; Public-Function
void TestLiftsimulator();