#ifndef HEADER_H
#define HEADER_H

// Standard includes
#include <stdio.h>
#include <stdlib.h>

/* 
Advanced C programming
BOULANGER AXELLE 
Student ID: 25040201
Project FACTORY FLOOR PLAN 
*/

// -- To compile gcc Factory_menu.c Factory_room.c Factory_machine.c -o project.exe
// -- then ./project.exe

// ====== STRUCTURES ======

typedef struct Machine {
    int machineId;
    int Row;
    int Col;
    int Maintenance; 
} Machine;

// variable globale (définie dans menu.c)
extern int nMachine;

// ====== ROOM FUNCTIONS ======

void createRoom();
void matrixSize(int *rows, int *cols);
void matrixCreation(int m, int n, int ***ptr1);
void doorInput(int m, int n, int **ptr1);
void display(int m, int n, int **ptr);
void updateFile(int m, int n, int **ptr1);
void freedom(int m, int **ptr);

void displayFile(FILE *fptr);
void displayROOM(int roomNum);
int numberOfRooms();

// ====== MACHINE FUNCTIONS ======

void createMachine();
int** getRoomInfo(int *roomNum, int *rows, int *cols);
void NewMachineInfo(int m, int n, int **CurrRoomMatrix, int roomNum);
void MachinePos(int m, int n, int **ptr1, int *inputRow, int *inputCol);
int doorToMach(int doorM, int doorN, int col, int row, int m, int n, int **ptr);
int nextPt(int testm, int testn, int m, int n, int ptm, int ptn, int **ptr);
int border(int ptm, int ptn, int m, int n, int **ptr);
int checkMach(int testm, int testn, int m, int n, int ptm, int ptn);
void cleanMatrix(int m, int n, int **ptr);
void updateMatrix(int roomNum, int **CurrRoomMatrix);

void saveMachineId();
void ReturnMachineId();

void displayMachines();
void displayMachinesRoom();
void displayMachinesID();

void MaintenanceStatus();
void MaintenanceList();

void RemoveMachinesID();
void removeMachineFromRoomFile(char *fileName, int id);

// ====== MENU / FACTORY FUNCTIONS ======

void checkExistingF(FILE **fptr);
void SupprRoomFiles();

#endif