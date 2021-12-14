/*******************************************************************************
* Programm:  Bibliotheksfunktionen fuer Liftsteuerung
* Filename:   LiftLibraryWOd_V2.c
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
#include <avr/io.h>
#include "LiftLibrary.h"


/******************************************************************************/
/*** EIGENE DATENTYPEN ********************************************************/
/******************************************************************************/
typedef struct {
  DoorStateType	 state;
  int8_t		 position;
} DoorType;


/******************************************************************************/
/*** PRIVATE VARIABLEN ********************************************************/
/******************************************************************************/
DisplayStateType  liftDisplay7Seg_On;  // Zustand der 7-Seg. Anzeige (ON-OFF)
LiftPosType       liftDisplay7Seg;     // Stockwerkanzeige im Lift (7-Segmentanz.)
uint8_t           floorDisplayPort;    // Uebermittelter Wert an Port

DisplayStateType  buttonLiftLed_on;    // Zustand der Tastenanzeige im Lift (On-OFF)
LiftPosType       buttonLiftLed;       // Anzeige Etagenauswahl im Lift

DisplayStateType  buttonFloorLed_On;   // Zustand der Ruftastenanzeige (ON-OFF)
LiftPosType       buttonFloorLed;      // Anzeige der Ruftaste pro Etage

uint8_t           displayCache;        // Schattenspeicher der Anzeige (LED)

DisplayStateType  liftPosDisplay_On;   // Zustand der Luftpositionsanzeige
uint8_t           liftPosition;        // Position des Liftes
uint8_t           liftPositionCache;   // alter liftPositionPort
uint8_t           liftPositionPort;    // Uebermittelter Wert an Port

DisplayStateType  doorframe;           // LED: 1,10 der Lifttueren

uint8_t           repetitionRate = 0;  // repetitionRate wie oft die Anzeige-LEDs
// angesteuert werden
uint16_t          doorOpeningRate = 0; // Lifttueroeffnungsgeschwindigkeit (Hilfsvariable)

uint16_t          liftSpeed = 0;       // liftSpeed (Hilfsvariable)

DoorType          liftDoorState[maxDoors];   // Speichert den Zustand der einzelnen Tueren ab

DoorPosType       doorPositions[5] = {Door00, Door25, Door50, Door75, Door100};
// Moegliche Tuerpositionen (Hilfsarray)

// Variablen für die Funktion: testLiftsimulator
int16_t		count2 = 0;                // Variable wird nur fuer den Test des Boards verwendet
int16_t		count3 = 0;                // Variable wird nur fuer den Test des Boards verwendet


/*******************************************************************************
***  Funktions-Deklarationen ***************************************************
*******************************************************************************/
// Tueren in den angegebenen Zustand bringen; Private-Function
void MakeDoorStates();


/*******************************************************************************
********************************************************************************
*** PRIVATE FUNKTIONEN *********************************************************
********************************************************************************
*******************************************************************************/


/*******************************************************************************
* Zustand der Lifttuere einer Etage in den vorgegebenen Zustand bringen
*******************************************************************************/
void MakeDoorStates (){

  for (int8_t count = 0; count < maxDoors; count++){
    if ((liftDoorState[count].state == Closed) &&
    (liftDoorState[count].position > 0)){
      liftDoorState[count].position--;
    }
    if ((liftDoorState[count].state == Open) &&
    (liftDoorState[count].position < 4)){
      liftDoorState[count].position++;
    }
  }
}


/*******************************************************************************
********************************************************************************
*** GLOBALE FUNKTIONEN *********************************************************
********************************************************************************
*******************************************************************************/


/*******************************************************************************
* Initialisierung der Ports
*******************************************************************************/
void InitializePorts(){
  // Setzt die DDR-Ports der Liftsteuerungssimulation auf die richrtigen Werte
  liftPos_D     = 0xFF;
  liftDoors_D   = 0xFF;
  liftDisplay_D = 0xFF;
  buttons_D     = 0x00;
}


/*******************************************************************************
* Initialisierung des Anfangszustandes
*******************************************************************************/
void InitializeStart(){
  
  // Aktivierung der Liftpositionsanzeige
  liftPosDisplay_On = On;

  // Aktivierung der Etagenauswahlanzeige (Anzeige im Lift)
  buttonLiftLed_on = On;

  // Aktivierung der Etagenanzeige im Lift (7-Segment-Anzeige)
  liftDisplay7Seg_On = On;

  // Aktivierung der Ruftastenanzeige (Anzeige auf jeder Etage)
  buttonFloorLed_On = On;

  // Aktivierung des Tuerrahmens
  doorframe = On;

  // Alle Lifttueren schliessen
  for (int8_t count = Floor0; count <= Floor3; count++){
    liftDoorState[count].position = 0;
    liftDoorState[count].state = Closed;
  }

  // Setzt den Lift auf einen bestimmten Positionswert
  liftPosition = 31;
}


/*******************************************************************************
* Kalibrieren der Fahrgastzelle auf die Position: Etage0
*******************************************************************************/
void CalibrateElevatorPosition(){
  
  if (ReadElevatorState() != Floor0)
  MoveElevator(Down,Fast);
}


/*******************************************************************************
* Ansteuerung der Ausgabeports
*******************************************************************************/
void SetOutput(){
  
  //lokale Variablen
  DisplayStateType buttonLiftLed_on_tmp;
  DisplayStateType buttonFloorLed_On_tmp;

  // Ansteuerung der Lifttueren pro Etage
  // Festlegung der Tuergeschwindigkeit (oeffnen und schliessen)
  if (!(doorOpeningRate % liftDoorOpeningTime)){
    MakeDoorStates();
    doorOpeningRate = 0;
  }

  // Festlegung und Ausgabe der Tuerposition
  switch (repetitionRate){
    case 3:   {
      liftDoors = doorPositions[liftDoorState[Floor0].position] |
      Floor0_D;  // Floor 0
      break;
    }
    case 6:   {
      liftDoors = doorPositions[liftDoorState[Floor1].position] |
      Floor1_D;  // Floor 1
      break;
    }
    case 9:   {
      liftDoors = doorPositions[liftDoorState[Floor2].position] |
      Floor2_D;  // Floor 2
      break;
    }
    case 12:  {
      liftDoors = doorPositions[liftDoorState[Floor3].position] |
      Floor3_D;  // Floor 3
      break;
    }
    case 13:  {
      repetitionRate = 0;
    }
  }
  
  repetitionRate++;
  doorOpeningRate++;
  
  // Ansteuerung der einzelnen LED (Ruftastenanzeige, Etagenauswahlanzeige)
  if (displayCache > 0){
    switch (repetitionRate % 5){
      case 0: {
        if ((displayCache & 0x10) > 0)
        buttonLiftLed = Floor0;
        if ((displayCache & 0x01) > 0)
        buttonFloorLed = Floor0;
        break;
      }
      case 1: {
        if ((displayCache & 0x20) > 0)
        buttonLiftLed = Floor1;
        if ((displayCache & 0x02) > 0)
        buttonFloorLed = Floor1;
        break;
      }
      case 2: {
        if ((displayCache & 0x40) > 0)
        buttonLiftLed = Floor2;
        if ((displayCache & 0x04) > 0)
        buttonFloorLed = Floor2;
        break;
      }
      case 3: {
        if ((displayCache & 0x80) > 0)
        buttonLiftLed = Floor3;
        if ((displayCache & 0x08) > 0)
        buttonFloorLed = Floor3;
        break;
      }
    }
  }

  if ((displayCache & 0xF0) == 0)
  buttonLiftLed_on_tmp = Off;
  else buttonLiftLed_on_tmp = buttonLiftLed_on;
  
  if ((displayCache & 0x0F) == 0)
  buttonFloorLed_On_tmp = Off;
  else buttonFloorLed_On_tmp = buttonFloorLed_On;
  
  
  // Einzelzustaende werden auf die Ausgabeports gemeinsam ausgegeben
  liftPositionPort = (liftPosDisplay_On)? 0x3F : (liftPosition & 0x3f);
  floorDisplayPort = (liftDisplay7Seg_On)?  0x07: liftDisplay7Seg;
  liftPos     = liftPositionPort | (doorframe<<6) |
  (buttonLiftLed_on_tmp <<7);
  liftDisplay = floorDisplayPort | (buttonLiftLed <<3) |
  (buttonFloorLed <<5) |(buttonFloorLed_On_tmp <<7);
}


/*******************************************************************************
* Lesen des Tastenzustandes (Lifttasten und Etagentasten)
*******************************************************************************/
ButtonStateType ReadKeyEvent (ButtonType button){
  
  return ((buttons & button)? Pressed: Released);
}


/*******************************************************************************
* Lesen des Lifttuerenzustandes einer Etage
*******************************************************************************/
DoorStateType ReadDoorState (LiftPosType floor){

  DoorStateType state;

  if (doorPositions[liftDoorState[floor].position] == Door100)
  state = Open;
  else if (doorPositions[liftDoorState[floor].position] == Door00)
  state = Closed;
  else
  state = Moves;

  return (state);
}


/*******************************************************************************
* Setzen des Lifttuerenzustandes einer Etage
*******************************************************************************/
void SetDoorState (DoorStateType state, LiftPosType floor){
  
  liftDoorState [floor].state = state;
}


/*******************************************************************************
* Verschieben der Fahrgastzelle (Lift)
*******************************************************************************/
void MoveElevator (DirectionType direction, SpeedType speed){
  
  if (speed != Stop){
    if (liftSpeed++ > speed){
      liftSpeed = 0;
      if (direction == Up) {
        if (liftPosition < liftMaxPos)
        liftPosition++;
      }
      else {
        if (liftPosition > 0)
        liftPosition--;
      }
    }
  }
}


/*******************************************************************************
* Gibt den Zustand des Liftes bekannt
*******************************************************************************/
LiftPosType ReadElevatorState(){
  
  LiftPosType output;
  
  switch (liftPosition){
    case 0:   {
      output = Floor0;
      break;
    }
    case 16:  {
      output = Floor1;
      break;
    }
    case 32:  {
      output = Floor2;
      break;
    }
    case 48:  {
      output = Floor3;
      break;
    }
    default:  {
      if (liftPosition >= 49)
      output = UpperStop;
      else if (liftPositionCache != liftPosition)
      output = LiftMoves;
      else
      output = LiftStopped;
    }
  }
  
  liftPositionCache = liftPosition;    // Muss noch verbessert werden !!!
  
  return (output);
  
}


/*******************************************************************************
* Setzen der floorDisplayPort im Lift (7-Segment-Anzeige)
*******************************************************************************/
void SetDisplay (LiftPosType displayValue){
  
  liftDisplay7Seg = displayValue;
}


/*******************************************************************************
* Setzen der Ruftastenanzeige pro Etage
*******************************************************************************/
void SetIndicatorFloorState (LiftPosType floor){
  
  if (floor <= 3){
    displayCache |= (1<<floor); //Setzen der Bits: 0-3
  }
}


/*******************************************************************************
* Setzen der Etagenauswahlanzeige im Lift
*******************************************************************************/
void SetIndicatorElevatorState (LiftPosType floor){
  
  if (floor <= 3){
    displayCache |= 1<<(floor + 4); //Setzen der Bits: 4-7
  }
}


/*******************************************************************************
* Loeschen der Ruftastenanzeige pro Etage
*******************************************************************************/
void ClrIndicatorFloorState (LiftPosType floor){
  
  if (floor <= 3){
    displayCache &= ~(1<<floor); //Loeschen der Bits: 0-3
  }
}


/*******************************************************************************
* Loeschen der Etagenauswahlanzeige im Lift
*******************************************************************************/
void ClrIndicatorElevatorState (LiftPosType floor){
  
  if (floor <= 3){
    displayCache &= ~(1<<(floor + 4)); //Loeschen der Bits: 4-7
  }
}


/*******************************************************************************
* Test um das Board des Liftsimulators zu testen
*******************************************************************************/
void TestLiftsimulator(){

  if (!(count3++ % 3000)){
    if (count2 == 51) count2 = 0;
    else count2++;
    
    buttonLiftLed = count2 % 4;
    liftPosition = count2;
  }
  
  // Oeffnen und schliessen aller Lifttueren
  if (!(count3 % 20000)){
    if (liftDoorState[Floor0].state == Closed)
    liftDoorState[Floor0].state = Open;
    else liftDoorState[Floor0].state = Closed;

    if (liftDoorState[Floor1].state == Closed)
    liftDoorState[Floor1].state = Open;
    else liftDoorState[Floor1].state = Closed;

    if (liftDoorState[Floor2].state == Closed)
    liftDoorState[Floor2].state = Open;
    else liftDoorState[Floor2].state = Closed;

    if (liftDoorState[Floor3].state == Closed)
    liftDoorState[Floor3].state = Open;
    else liftDoorState[Floor3].state = Closed;
    count3 = 0;
  }
  
  // Gibt die Position des Liftes an, wenn die Etage erreicht wurde
  switch (ReadElevatorState()){
    case Floor0:        {
      buttonFloorLed_On = On;
      buttonFloorLed = Floor0;
      break;
    }
    case Floor1:        {
      buttonFloorLed_On = On;
      buttonFloorLed = Floor1;
      break;
    }
    case Floor2:        {
      buttonFloorLed_On = On;
      buttonFloorLed = Floor2;
      break;
    }
    case Floor3:        {
      buttonFloorLed_On = On;
      buttonFloorLed = Floor3;
      break;
    }
    case UpperStop:     {
      liftDisplay7Seg_On = Off;
      break;
    }
    case LiftMoves:        {
      buttonFloorLed_On = Off;
      break;
    }
    default:            {
      liftDisplay7Seg_On = On;
    }
    
  }
  
  // Test der einzelnen Tasten mit der 7-Segment-Anzeige
  switch (buttons){
    case EmergencyButton:       {
      liftDisplay7Seg = Test;
      break;
    }
    case LiftButton_F0:
    case FloorButton_F0:  {
      liftDisplay7Seg = Floor0;
      break;
    }
    case LiftButton_F1:
    case FloorButton_F1:  {
      liftDisplay7Seg = Floor1;
      break;
    }
    case LiftButton_F2:
    case FloorButton_F2:  {
      liftDisplay7Seg = Floor2;
      break;
    }
    case LiftButton_F3:
    case FloorButton_F3:  {
      liftDisplay7Seg = Floor3;
      break;
    }
    default:            {
      liftDisplay7Seg = Error;
    }
  }
}



