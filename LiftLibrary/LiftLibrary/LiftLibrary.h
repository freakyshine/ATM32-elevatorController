/*******************************************************************************
* Programm:  Bibliotheksfunktionen fuer Liftsteuerung
* Filename:  Liftmodell_Lib.h
*
* Autor:     Werner Odermatt
* Version:   2.0
* Datum:	 07.09.2016
*
* Entwicklungsablauf(Version, Datum, Autor, Entwicklungsschritt, Zeit):
* 1.0   25.05.15   WOd   Funktion: initalizePorts() erstellt
* 2.0   07.09.16   WOd   Ueberarbeitung der Software, Code-Richtlinien C#
********************************************************************************
*
* Verwendungszweck: C-Schulung, M121, M242
*
* Beschreibung:
* Dieses Programm simuliert einen Lift mit 4 Etagen
*
* .
*
* Precondition:  -
*
* Postcondition: -
*
* Benötigte Libraries:
* - avr/io.h
* -
*
* Erstellte Funktionen:
* - initializePort()
* Copyright (c) 2016 by W.Odermatt, CH-6340 Baar
*******************************************************************************/


/***  Include Files ***********************************************************/
#include <avr/io.h>	     // Funktionsbibliothek: IO-Schnittstelle AVR-Controller


/******************************************************************************/
/*** EIGENE DATENTYPEN ********************************************************/
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


/******************************************************************************/
/*** GLOBALE KONSTANTEN *******************************************************/
/******************************************************************************/

/** SYSTEMEINSTELLUNGEN **/
// Taktfrequenz des Controllers
//#define F_CPU		8000000UL	// 8 MHz


/** PROGRAMMEINSTELLUNGEN **/
#define maxDoors                4   // Anzahl der vorhanden Tueren
#define liftDoorOpeningTime  5000   // Geschwindigsangabe fuer Liftoeffnung
#define liftMaxPos             49   // hoechster Positionswert des Liftes


/** PORT- und PINZUWEISUNGEN **/
// PORT A:
#define liftDisplay_D     DDRA  // 7-Seg.; Etagenauswahlanz.; Ruftastenanz.
#define liftDisplay       PORTA // 7-Seg.: Pin 0-2; Etagenauswahlanz.: Pin 3-4
// Ruftastenanz.: Pin 5-7

// PORT B:
#define liftPos_D         DDRB  // liftPosition: Pin 0-5
#define liftPos           PORTB // Tuersimulation LED:1_10: Pin 6; Low = ON
// Etagenauswahlanzeige: Pin 7; Low = ON

// PORT C
#define liftDoors_D       DDRC  // Tuersimulation: Pin 0-3; Etage; High = ON
#define liftDoors         PORTC // Pin 4: LED:5_6; Pin 5: LED:4_7
// Pin 6: LED:3_8; Pin 7: LED:2_9

// PORT D:
#define buttons_D         DDRD  // Etagenauswahltasten im Lift: Pin 0-3
#define buttons           PIND  // Ruftasten pro Etage: Pin 4-7


/******************************************************************************/
/*** GLOBALE VARIABLEN *******************************************************/
/******************************************************************************/
//Keine


/******************************************************************************/
/*** GLOBALE FUNKTIONEN *******************************************************/
/******************************************************************************/
// Ports initialisieren
void InitializePorts();

// Setzen des Anfangszustandes (I/O); Public-Function
void InitializeStart();

// Kalibrieren der Fahrgastzelle auf die Position: Etage0
void CalibrateElevatorPosition();

// Ansteuerung der Ausgabeports
void SetOutput();

// Lesen des Zustandes einer bestimmten Taste; Public-Function
ButtonStateType ReadKeyEvent (ButtonType button);

// Lesen des Zustandes der Lifttuere einer Etage; Public-Function
DoorStateType ReadDoorState (LiftPosType floor);

// Setzen des Tuerenzustandes pro Etage; Public-Function
void SetDoorState (DoorStateType state, LiftPosType floor);

// Verschieben der Fahrgastzelle (Lift)
void MoveElevator (DirectionType direction, SpeedType speed);

// Gibt die Infos zum Liftzustand bekannt; Public-Function
LiftPosType ReadElevatorState();

// Setzen der Etagenanzeige im Lift (7-Segment-Anzeige); Public-Function
void SetDisplay (LiftPosType displayValue);

// Setzen der Ruftastenanzeige pro Etage; Public-Function
void SetIndicatorFloorState (LiftPosType floor);

// Setzen der Etagenauswahlanzeige im Lift; Public-Function
void SetIndicatorElevatorState (LiftPosType floor);

// Löschen der Ruftastenanzeige pro Etage; Public-Function
void ClrIndicatorFloorState (LiftPosType floor);

// Löschen der Etagenauswahlanzeige im Lift; Public-Function
void ClrIndicatorElevatorState (LiftPosType floor);

// Test zur Üeberpruefung des Lifsimulatorboards; Public-Function
void TestLiftsimulator();