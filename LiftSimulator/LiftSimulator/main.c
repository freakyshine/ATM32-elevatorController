/******************************************************************************
 * Program:   Lift Simulator Basic Structure
 * Filename:  LiftSimulator
 * Author:    Werner Odermatt
 * Version:   1.0
 * Date:	     09.12.2016
 *
 * Development flow(Version, Date, Author, Development step, Time):
 * 1.0   09.12.16   WOd   Basic structure created
 ********************************************************************************
 *
 * Using: C-Training, M121, M242
 *
 * Description:
 * Basic structure for lift simulation.
 *
 * Precondition:  -
 *
 * Postcondition: -
 *
 * Required Libraries:
 * - avr/io.h
 * - LiftLibrary.h
 *
 * Created Functions:
 * - ConvertButtonTypeToLiftPosType()
 * - CheckKeyEvent()
 * - UpdateDisplay()
 *
 * Copyright (c) 2016 by W.Odermatt, CH-6340 Baar
 *******************************************************************************/

/*** OWN DATA TYPES ***********************************************************/

/*** CONSTANTS ****************************************************************/

/*** INCLUDE FILES ************************************************************/
#include "LiftLibrary/LiftLibrary.h" // lift model library

/*** GLOBAL Variablen *********************************************************/
StateMachineType state = Uninitialized;
LiftPosType requestedElevatorPosition = None;
LiftPosType currentElevatorState = None;
DirectionType elevatorDirection = Down;
SpeedType elevatorSpeed = Fast;
LiftPosType requestQueue[4] = { None, None, None, None };
short queueSize = 0;

/*************************************************************** ****************
 ***  PRIVATE FUNCTIONS  ********************************************************
 *******************************************************************************/
// Get higher speed if distance bigger, lower speed if distance shorter
SpeedType GetSpeed(LiftPosType low, LiftPosType high);

// Convert ButtonType to LiftPosType
LiftPosType ConvertButtonTypeToLiftPosType(ButtonType button);

// Check if buttons are pressed
ButtonType CheckKeyEvent();

// Check Buttons for input, if present store to queue. As button is pressed, it will also light up the corresponding indicator
void CheckButtons ();

// Check Buttons for input, if present store to queue. As button is pressed, it will also light up the corresponding indicator
short CheckQueue (LiftPosType floor);

// Shifts the queue to the left and replaces the last element with a placeholder / empty slot
void ShiftQueue();

// Update the 7-Segment. display
void UpdateDisplay(LiftPosType elevatorState);

/*******************************************************************************
 *** MAIN PROGRAM
 *******************************************************************************/
int main(void) {
    InitializePorts(); // Initialization of ports
    InitializeStart(); // Set start state of the system

    // Endless loop
    while (1) {
        currentElevatorState = ReadElevatorState();
        CheckButtons();
        // Handling state machine
        switch (state) {
        case Uninitialized: {
			requestedElevatorPosition = Floor0;
            // Lift position calibration to ground floor (Floor0)
            if (currentElevatorState == Floor0) {
                SetDoorState(Open, Floor0);
                state = Waiting;
            }
            CalibrateElevatorPosition();
            break;
        }

        // Waiting for new floor request
        case Waiting: {
			if (requestQueue[0] == currentElevatorState)
			{
				ShiftQueue();
			}
			else if (requestQueue[0] != None) {
				requestedElevatorPosition = requestQueue[0];
				state = CloseDoor;
			}
            break;
        }

        case CloseDoor: {
            // Close the door and wait until the door is closed
            SetDoorState(Closed, currentElevatorState);
            if (ReadDoorState(currentElevatorState) == Closed) {
                if (currentElevatorState != requestedElevatorPosition) {
                    if (requestedElevatorPosition < currentElevatorState) {
                        elevatorDirection = Down;
                        elevatorSpeed = GetSpeed(requestedElevatorPosition, currentElevatorState);
                    } else {
                        elevatorDirection = Up;
                        elevatorSpeed = GetSpeed(currentElevatorState, requestedElevatorPosition);
                    }
                }
                state = MoveLift;
            }
            break;
        }

        case MoveLift: {
            // Move cabin to the requested floor
            if (currentElevatorState != requestedElevatorPosition) {
                MoveElevator(elevatorDirection, elevatorSpeed);
            } else {
                state = OpenDoor;
            }
            break;
        }

        case OpenDoor: {
            // Open the door and wait still the door is open completely
            ClrIndicatorFloorState(requestedElevatorPosition);
            ClrIndicatorElevatorState(requestedElevatorPosition);
            SetDoorState(Open, requestedElevatorPosition);
            if (ReadDoorState(requestedElevatorPosition) == Open) {
                state = Waiting;
            }
            break;
        }

        case Trouble: {
            // Fault condition is not treated
            break;
        }
        }

        UpdateDisplay(currentElevatorState); // Update the 7-Seg. display (lift)
        SetOutput(); // Send the calculated output values to the ports
    }

    return (0);
}

/*******************************************************************************
 ***  PRIVATE FUNCTIONs *********************************************************
 *******************************************************************************/
// Check Buttons for input, if present store to queue. As button is pressed, it will also light up the corresponding indicator
void CheckButtons () {
	// EmergencyButton being the first in the enum, and FloorButton_F3 the last => change this if enum in library alters
	ButtonType btn = CheckKeyEvent();
	LiftPosType liftPosTypeBtn = ConvertButtonTypeToLiftPosType(btn);
	
	if (btn != EmergencyButton && currentElevatorState != liftPosTypeBtn && CheckQueue(liftPosTypeBtn) == 0) {
		if (queueSize < 4) {
			requestQueue[queueSize] = liftPosTypeBtn;
			
			if (btn > 8) {
				SetIndicatorFloorState(liftPosTypeBtn);
			}
			else {
				SetIndicatorElevatorState(liftPosTypeBtn);
			}
			
			queueSize++;
		}
	}
}

// Check if element is present in queue
short CheckQueue (LiftPosType floor) {
	short duplicate = 0;
	for (int i = 0; i < 4; i++) {
		if (requestQueue[i] == floor) {
			duplicate++;
			break;
		}
	}
	return duplicate;
}

// Shifts the queue to the left and replaces the last element with a placeholder / empty slot
void ShiftQueue() {
	requestQueue[0] = requestQueue[1];
	requestQueue[1] = requestQueue[2];
	requestQueue[2] = requestQueue[3];
	requestQueue[3] = None;
	queueSize--;
}

// Get higher speed if distance bigger, lower speed if distance shorter
SpeedType GetSpeed(LiftPosType low, LiftPosType high) {
    if (high - low > 2) {
        return Fast;
    }  else {
        return Medium;
    }
}

// Convert ButtonType to LiftPosType
LiftPosType ConvertButtonTypeToLiftPosType(ButtonType button) {
    LiftPosType retVal = None;

    switch (button) {
    case LiftButton_F0:
    case FloorButton_F0: {
        retVal = Floor0;
        break;
    }
    case LiftButton_F1:
    case FloorButton_F1: {
        retVal = Floor1;
        break;
    }
    case LiftButton_F2:
    case FloorButton_F2: {
        retVal = Floor2;
        break;
    }
    case LiftButton_F3:
    case FloorButton_F3: {
        retVal = Floor3;
        break;
	}
    default: {
        //retVal = None;
        break;
    }
    }

    return retVal;
}

// Check if buttons are pressed
ButtonType CheckKeyEvent() {
    ButtonType retVal = EmergencyButton;

    for (ButtonType key = FloorButton_F3;
        ((key >= LiftButton_F0) && (retVal == EmergencyButton)); key >>= 1) {
        if (ReadKeyEvent(key) == Pressed) {
            retVal = key;
        }
    }
    return retVal;
}

// Update the 7-Seg. display
void UpdateDisplay(LiftPosType elevatorState) {
    switch (elevatorState) {
    case Floor0:
    case Floor1:
    case Floor2:
    case Floor3:
    case Error:
    case Test: {
        SetDisplay(elevatorState);
        break;
    }
    default: {
        break;
    }
    }
}