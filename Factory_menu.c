#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Factory_header.h"

/* Notes : 
Advanced C programming
BOULANGER AXELLE 
Student ID: 25040201
Project
Factory Machine-room floor plan -- MENU file
*/

/*
Project's Notes:

-----------------------------------------------------------
Room
- user create a room - room is a matrix 
- define size - use malloc exo 2 
- initialize w/ only 0 = empty room 

-- Now we need to be able to create multiple rooms 
-- when room is created put it inside a file 

--We can now display all of the files room without pb 
--add the room door position input => 2 == door

----------------------------------------------------------
Machine 
- When Input Machine 
    -Id - auto_increment - put var in a little file  
        else to complicated to add to the others or count everything each time

    - ask user localisation - wich room - verify room exist - get m and n
    - enter machine pos - verfy matrix inbound + free pos 
    - assign "1" in machine pos 
    - register machine struc -- create new File by room  
    - auto maintenance status = 0 (None) - user need to enter 

    -  register the created machine struct into it's room file 
            - create room file // in factory init - del file if (0)

-Mark/unmark machines as under maintenance with id - add menu option

-display all machines underMaintenance (organized by room) - add menu option

remove machines by Id  
    - Verify existing Id 
    - search position in room file 
    - Remove from matrix - buffer -> 0 at pos c and y 
    - Save new matrix in factory file 
    - Remove from room file 

----------------------------------------------------------
*/

int nMachine = 0;   

// INIT FACTORY ==================================================================

void checkExistingF(FILE **fptr) //CREATE OR UPDATE FACTORY 
{   
    int redo=0;
    printf("=====STARTING APP=====\n");

    if ((*fptr = fopen("factory.bin", "rb"))== NULL)
    {printf("No existing file detected or path not working \n");}
    else
    {
        printf("if you would you like to keep working the existing factory press '1'\n"
                "else press '0' to reset it : ");
        scanf("%d",&redo);
        fclose(*fptr);
    }

    //printf("%d\n",redo);
    if (redo == 0)
    {   
        // SUPPR THE FILE - But first read it to know number of rooms and suppr room files 
        nMachine = 0;
        saveMachineId(); // init value as 0 
        SupprRoomFiles();
        if ((*fptr = fopen("factory.bin", "wb"))== NULL)
        {printf("Error - verify file path or permissions \n");}
        else {fclose(*fptr);} 
        printf("\n");
        printf("=====FACTORY CREATION=====\n");
    }
    else
    {
        ReturnMachineId();
        //printf("%d\n", nMachine); -test
        printf("\n");
        printf("=====FACTORY UPDATING=====\n");
    }
}

void SupprRoomFiles(){
    int NbrOfRoom = numberOfRooms();
    char fileName[50];
    //printf("number of rooms %d\n", NbrOfRoom); - test

    for (int i = 1; i <=NbrOfRoom; i++){
        //ERASE LOCAL ROOM MACHINE FILE 
        sprintf(fileName, "room%d.bin", i); // using + doesn't work in C ...
        //printf("%s\n", fileName); - test
        remove(fileName);
    }
}

// MAIN / MENU ===================================================================

int main ()
{
    //variables
    FILE *fptr = NULL;
    int choice, roomNum; 

    //init message
    checkExistingF(&fptr);

    while (1)
    {        
        printf("\n----Menu----");
        printf("\n1.New room \n"
            "2.New machine\n"
            "3.Display all Rooms --"
            "4.Only One Room\n"
            "Display Machine informations --5.All --"
            "6.By Room --7.By Id\n"
            "8.Toggle machine's maintenance status\n"
            "9.Display all machines underMaintenance\n"
            "10.Remove machine\n"
            "11.Exit");

        printf("\n");
        printf("\nEnter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        { 
        case 1:
            createRoom();
            break;
        case 2:
            createMachine();
            break;
        case 3:
            displayFile(fptr); 
            break;
        case 4:
            printf("Enter factory room number : "); 
            scanf("%d", &roomNum);
            displayROOM(roomNum);
            break;
        case 5:
            displayMachines();
            break;
        case 6:
            displayMachinesRoom();
            break;
        case 7:
            displayMachinesID();
            break;
        case 8:
            MaintenanceStatus();
            break;
        case 9:
            MaintenanceList();
            break;
        case 10:
            RemoveMachinesID();
            break;
        case 11:
            exit(0);
        default:
            printf("\nInvalid choice!!");       
        }    
    }   

    return 0;  
}