#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Factory_header.h"

// Machine creation / path check / maintenance file 

// CREATE MACHINE ===============================================================

void createMachine(){
    // ask user wich room 
    int roomNum, m,n;
    int **CurrRoomMatrix;

    // Recuperate the matrix as well as m and n
    printf("Enter factory room number : "); 
    scanf("%d", &roomNum);
    CurrRoomMatrix= getRoomInfo(&roomNum, &m, &n); 

    //verify right matrix 
    display(m,n,CurrRoomMatrix); 

    //Machine Info 
    NewMachineInfo(m,n, CurrRoomMatrix,roomNum);

    //verify matrix 
    display(m,n,CurrRoomMatrix); 

    //update matrix in factory file
    updateMatrix(roomNum, CurrRoomMatrix);

    freedom(m, CurrRoomMatrix); 
}

int** getRoomInfo(int* roomNum, int *rows, int *cols){
    //method - counting  the number of room - up to the number - then getting that one 
    FILE *fptr;
    int m, n; // read them for each room
    int current = 1;
    int found = 0; 

    while (!found){

        if ((fptr = fopen("factory.bin", "rb")) == NULL){
        printf("No existing file detected or path not working\n");
        return NULL;}
        
        // Loop through all stored rooms
        while (fread(&m, sizeof(int), 1, fptr) == 1 &&
            fread(&n, sizeof(int), 1, fptr) == 1)
        {

            /*bug catching prints-----
            printf("room num %d\n",roomNum);
            printf("current room num %d\n",current);
            printf("m %d et n %d \n",m, n);*/
            
            if (current == *roomNum)
            {
                printf("\n--- ROOM %d (Size %dx%d) ---\n", *roomNum, m, n);

                // //buffer of right type 
                int **buffer;
                matrixCreation(m, n, &buffer);

                for (int i = 0; i < m; i++)
                    fread(buffer[i], sizeof(int), n, fptr);

                found=1;
                fclose(fptr);

                //return the needed info for the machine
                *rows = m;
                *cols = n;
                return buffer; 
            }
            else {fseek(fptr, sizeof(int) * m * n, SEEK_CUR);} //skip to realign the bytes
            current +=1;
        }
        fclose(fptr);

        if (found ==0){
            printf("\nRoom Number not found\n");
            current = 1;
            printf("Enter factory room number : "); 
            scanf("%d", roomNum);
        }        
    }
}

void NewMachineInfo(int m,int n, int **CurrRoomMatrix, int roomNum){
    int TotalMachine=0; 
    struct Machine currentMachine;
    nMachine++; 

    //ID -----------------------------------------------------------------
    //auto_increment id --> counting nb of 1 already in matrix 
    //- useless now but kept it because its cool to tell the number of already existing machines
    for (int i = 0; i < m; i++)
        {   
            for (int j =0; j< n; ++j)
            { if (CurrRoomMatrix[i][j] == 1){ TotalMachine +=1;}} 
        }
    printf("Number of already existing machine : %d\n",TotalMachine ); // test
    currentMachine.machineId = nMachine;
    saveMachineId();
    
    //POSITION -----------------------------------------------------------------
    //localisation - most complicated TT     
    int inputRow, inputCol; 
    MachinePos(m, n, CurrRoomMatrix, &inputRow, &inputCol);
    currentMachine.Row =inputRow;
    currentMachine.Col =inputCol;
    
    //MAINTENANCE -----------------------------------------------------------------
    //zero by default 
    currentMachine.Maintenance =0;

    //UPDATE LOCAL ROOM MACHINE FILE 
    FILE *fptrRoom;
    //updating file
    char fileName[50];
    sprintf(fileName, "room%d.bin", roomNum); // using + doesn't work in C ...

    if ((fptrRoom = fopen(fileName, "ab"))== NULL)
    {printf("Error - verify file path or permissions \n");
    return;}

    fwrite(&currentMachine, sizeof(currentMachine), 1, fptrRoom);

    fclose(fptrRoom);
}

    //Position cdt check --------------------------------
void MachinePos(int m, int n, int **ptr1, int* inputRow, int* inputCol){
    int row, col, ok=0; 

    int doorM, doorN;//--> for door to mach fct 
    //door pos ---------------------------------
    for (int i = 0; i < m; i++){   
            for (int j =0; j< n; ++j)
            { if (ptr1[i][j]==2){
                doorM =i; 
                doorN =j;
            }}} 
    //------------------------------------------------
    
    //handle exception error message == ok 
    while (ok ==0){
        printf("\n---------------------\n");
        printf("Input machine position -> rows(y) : ");
        scanf("%d", &row);
        printf("-> columns (x) : ");
        scanf("%d", &col);
        //reading logic 
        col= col-1; 
        row = m-row;
        //printf(" row %d, col  %d\n", row, col);

        //FIRST CDT ==================================
        // must be in bouds --------------------------
        if (col < 0 || col >= n || row < 0 || row >= m){
            printf("Out of bounds.\n");
        }
        // Cannot block the door--------------------------
        else if (ptr1[row][col]==2){
            printf("Please don't block the door.\n");
        }
        // Cannot put a machine on top of another--------------------------
        else if (ptr1[row][col]==1){
            printf("Cannot put a machine on top of another.\n");
        }
        //Path from door to every Machine must be accessible 
        else if (doorToMach(doorM, doorN, col, row, m, n, ptr1)) {ok =1;}
    }

    *inputCol = col+1;
    *inputRow = m-row;
}

int doorToMach(int doorM, int doorN, int col, int row, int m, int n, int**ptr){
    //need door pos + ptr + mach pos + m and n 

    ptr[row][col]=1;// check the new machine alongside the others 

    //Check everymachine - in case the new one blocks and already existing path -
    
    for (int i = 0; i < m; i++){   
        for (int j =0; j< n; ++j){
            if (ptr[i][j]==1){
                if (!nextPt(doorM, doorN, m, n,i, j, ptr)){
                    printf("Machine path obstructed- cannot place a machine here\n");
                    ptr[row][col]=0; //remove the machine and ask user to input again
                    cleanMatrix(m, n, ptr);
                    return 0; 
                }
                cleanMatrix(m, n, ptr);
            }
        } 
    }
    
    //no return then ok !
    printf("Path found.\n"); 
    return 1; 
}

int nextPt(int testm, int testn, int m, int n, int ptm, int ptn, int** ptr){
    if (checkMach(testm, testn, m, n, ptm, ptn)){ return 1;}; // check if machine is next to pt already - if yes paths is free machine can be here 

    //explore RIGHT
    if (border(testm,testn+1,m,n, ptr) == 0){ // if 0 move to case - put visited == 3 that way we can erase it after
        ptr[testm][testn+1] =3; 
        if (nextPt(testm, testn+1, m, n ,ptm, ptn, ptr) ==1){// recursion to verify all the line 
            return 1;
        } 
    }
    //explore UP
    if (border(testm-1,testn,m,n, ptr) == 0){
        ptr[testm-1][testn] =3; 
        if (nextPt(testm-1, testn, m, n ,ptm, ptn, ptr) ==1){
            return 1;
        } 
    }
    if (border(testm+1,testn,m,n, ptr) == 0){
        ptr[testm+1][testn] =3; 
        if (nextPt(testm+1, testn, m, n ,ptm, ptn, ptr) ==1){
            return 1;
        } 
    }
    if (border(testm,testn-1,m,n, ptr) == 0){
        ptr[testm][testn-1] =3; 
        if (nextPt(testm, testn-1, m, n ,ptm, ptn, ptr)==1){
            return 1;
        } 
    }    

    return 0; //no path
}

int border (int ptm, int ptn, int m, int n, int** ptr){
    if(ptm< 0 || ptm >=m || ptn< 0 || ptn >=n){
        return 1;
    }
    else {return ptr[ptm][ptn];}
}

int checkMach(int testm, int testn, int m, int n, int ptm, int ptn){

    if (testm==ptm && testn+1==ptn){return 1;}
    if (testm==ptm && testn-1==ptn){return 1;}
    if (testm-1==ptm && testn==ptn){return 1;}
    if (testm+1==ptm && testn==ptn){return 1;}

    return 0; //no machine next to pt 
}

void cleanMatrix(int m, int n, int**ptr){
    for (int i = 0; i < m; i++){   
        for (int j =0; j< n; ++j){
            if (ptr[i][j]==3){
                ptr[i][j]=0;
            }
        } 
    }
}
    // -------------------------------------------------

void updateMatrix(int roomNum,int **CurrRoomMatrix)
{
    FILE *fptr;
    int m, n; // read them for each room
    int current = 1;

    //updating file
    if ((fptr = fopen("factory.bin", "r+b"))== NULL) 
    {printf("Error - verify file path or permissions \n"); return;}

    // Loop through all stored rooms
    while (fread(&m, sizeof(int), 1, fptr) == 1 &&
           fread(&n, sizeof(int), 1, fptr) == 1)
    {     
        if (current == roomNum)
        {

            fseek(fptr, 0, SEEK_CUR); // put at right pos

            for (int i = 0; i < m; i++)
                fwrite(CurrRoomMatrix[i], sizeof(int), n, fptr);

            printf("Updated successfully\n");
            fclose(fptr);
            return;
        }
        fseek(fptr, (long)m * n * sizeof(int), SEEK_CUR); //skip to realign the bytes
        current +=1;
    }

    fclose(fptr);
}

void ReturnMachineId() {
    FILE *fptrId = fopen("machine_id.bin", "rb");
    if (fptrId == NULL) {
        printf("Error - verify file path or permissions \n");
        return;}
    fread(&nMachine, sizeof(int), 1, fptrId);
    fclose(fptrId);
}

void saveMachineId() {
    FILE *fptrId = fopen("machine_id.bin", "wb");
    if (fptrId == NULL) {
        printf("Error - verify file path or permissions \n");
        return;}

    fwrite(&nMachine, sizeof(int), 1, fptrId);
    fclose(fptrId);
}

// DISPLAY MACHINES =============================================================

void displayMachines(){
    FILE *fptrRoom;
    struct Machine bufferMachine;

    int NbrOfRoom = numberOfRooms();
    char fileName[50];
    //printf("number of rooms %d\n", NbrOfRoom); - test

    printf("\n");
    printf("-------MACHINES-------\n");

    for (int i = 1; i <=NbrOfRoom; i++){
        printf("-------Room%d-------\n",i);
        sprintf(fileName, "room%d.bin", i); 
        if ((fptrRoom = fopen(fileName, "rb"))== NULL) 
            {printf("Error - verify file path or permissions \n"); return;}
        //read all the file
        while (fread(&bufferMachine, sizeof(bufferMachine), 1, fptrRoom) ==1)
        {
            printf("Machine Id: %d\n", bufferMachine.machineId);
            printf("Position on y axis: %d\n", bufferMachine.Row);
            printf("Position on x axis: %d\n", bufferMachine.Col);
            printf("Maintenance status: %d\n", bufferMachine.Maintenance);
            printf("\n");
        }

        fclose(fptrRoom);        
    }
}

void displayMachinesRoom(){
    int choice, ok=0; 
    FILE *fptrRoom;
    struct Machine bufferMachine;

    int NbrOfRoom = numberOfRooms();
    char fileName[50];
    char roomWeWant[50]; // cannot use filename directly in case the room asked doesnt exist

    while(!ok){
        printf("Enter the room number you want to see the machines of : ");
        scanf("%d",&choice);
        sprintf(roomWeWant, "room%d.bin", choice);

        for (int i = 1; i <=NbrOfRoom; i++){
            sprintf(fileName, "room%d.bin", i);  

            if (strcmp(roomWeWant, fileName) == 0){
                printf("\n-------MACHINES-------\n");
                ok = 1; 
                printf("-------Room%d-------\n",i);
                if ((fptrRoom = fopen(fileName, "rb"))== NULL) 
                    {printf("Error - verify file path or permissions \n"); return;}
                //read all the file
                while (fread(&bufferMachine, sizeof(bufferMachine), 1, fptrRoom) ==1)
                {
                    printf("Machine Id: %d\n", bufferMachine.machineId);
                    printf("Position on y axis: %d\n", bufferMachine.Row);
                    printf("Position on x axis: %d\n", bufferMachine.Col);
                    printf("Maintenance status: %d\n", bufferMachine.Maintenance);
                    printf("\n");
                }

                fclose(fptrRoom);        
            }
        }
        if (ok==0){printf("This room doesn't exist\n");}
    }
}

void displayMachinesID(){
    int choice, ok=0; 
    FILE *fptrRoom;
    struct Machine bufferMachine;

    int NbrOfRoom = numberOfRooms();
    char fileName[50];
    //printf("number of rooms %d\n", NbrOfRoom); - test

    while(!ok){
        printf("Enter the Id of the machine you want to see : ");
        scanf("%d",&choice);

        for (int i = 1; i <=NbrOfRoom; i++){
            sprintf(fileName, "room%d.bin", i);  
                if ((fptrRoom = fopen(fileName, "rb"))== NULL) 
                    {printf("Error - verify file path or permissions \n"); return;}
                //read all the file
                while (fread(&bufferMachine, sizeof(bufferMachine), 1, fptrRoom) ==1)
                {   
                    if (choice ==bufferMachine.machineId){
                        ok = 1; 
                        printf("\n");
                        printf("-------MACHINE-------\n");
                        printf("-------Room%d-------\n",i);
                        printf("Machine Id: %d\n", bufferMachine.machineId);
                        printf("Position on y axis: %d\n", bufferMachine.Row);
                        printf("Position on x axis: %d\n", bufferMachine.Col);
                        printf("Maintenance status: %d\n", bufferMachine.Maintenance);
                        printf("\n");
                    }
                }

                fclose(fptrRoom);        
        }
        if (ok == 0){printf("This id doesn't exist");}
    }
}

// MAINTENANCE ==================================================================

void MaintenanceStatus(){
    int choice, ok=0; 
    FILE *fptrRoom;
    struct Machine bufferMachine;

    int NbrOfRoom = numberOfRooms();
    char fileName[50];

    while(!ok){
        printf("Enter the Id of the machine you want to change the maintenance status of : ");
        scanf("%d",&choice);

        for (int i = 1; i <=NbrOfRoom; i++){
            sprintf(fileName, "room%d.bin", i);  
                if ((fptrRoom = fopen(fileName, "r+b"))== NULL) 
                    {printf("Error - verify file path or permissions \n"); return;}
                //read all the file
                while (fread(&bufferMachine, sizeof(bufferMachine), 1, fptrRoom) ==1)
                {   
                    if (choice ==bufferMachine.machineId){
                        ok = 1; 
                        bufferMachine.Maintenance = (bufferMachine.Maintenance ==1 ? 0:1); 
                        fseek(fptrRoom, -(long)sizeof(bufferMachine), SEEK_CUR);
                        fwrite(&bufferMachine, sizeof(bufferMachine), 1, fptrRoom);
                        break;
                    }
                }
                fclose(fptrRoom);        
        }
        if (ok == 0){printf("This id doesn't exist\n");}
    }
}

void MaintenanceList(){
    FILE *fptrRoom;
    struct Machine bufferMachine;

    int NbrOfRoom = numberOfRooms();
    char fileName[50];

    for (int i = 1; i <=NbrOfRoom; i++){
        sprintf(fileName, "room%d.bin", i);  
        if ((fptrRoom = fopen(fileName, "rb"))== NULL) 
            {printf("Error - verify file path or permissions \n"); return;}
        //read all the file
        while (fread(&bufferMachine, sizeof(bufferMachine), 1, fptrRoom) ==1){ 
                if (bufferMachine.Maintenance == 1){
                        printf("\n");
                        printf("-------Room%d-------\n",i);
                        printf("Machine Id: %d\n", bufferMachine.machineId);
                        printf("Position on y axis: %d\n", bufferMachine.Row);
                        printf("Position on x axis: %d\n", bufferMachine.Col);
                        printf("Maintenance status: %d\n", bufferMachine.Maintenance);
                        printf("\n");
                    }
            }
        fclose(fptrRoom);    
    }
}

// REMOVE MACHINE ===============================================================

void RemoveMachinesID(){
    int choice, ok=0; 
    FILE *fptrRoom;
    struct Machine bufferMachine;
    int **MachineRoomMatrix;

    int NbrOfRoom = numberOfRooms();
    char fileName[50];

    //Machine info for next part 
    int MachineR, MachineC, MachineRoom, m, n;

    while(!ok){
        printf("Enter the id of the machine you want to remove : ");
        scanf("%d",&choice);

        for (int i = 1; i <=NbrOfRoom; i++){
            sprintf(fileName, "room%d.bin", i);  

            if ((fptrRoom = fopen(fileName, "r+b"))== NULL) 
                {printf("Error - verify file path or permissions \n"); return;}
                //read all the file
                while (fread(&bufferMachine, sizeof(bufferMachine), 1, fptrRoom) ==1)
                {   
                    if (choice ==bufferMachine.machineId){
                        ok = 1; 
                        MachineR = bufferMachine.Row;
                        MachineC = bufferMachine.Col;
                        MachineRoom = i;
                        break;
                    }
                }
            fclose(fptrRoom);    
            if (ok ==1){break;}
        }
        if (ok == 0){printf("This id doesn't exist");}
    }

    sprintf(fileName, "room%d.bin", MachineRoom);
    removeMachineFromRoomFile(fileName, choice);

    //get matrix of the right room 
    MachineRoomMatrix= getRoomInfo(&MachineRoom, &m, &n); 

    //- put 0 at machine we want to remove position -- careful -> humain to machine indices
    MachineRoomMatrix[ m - MachineR][MachineC - 1] = 0; 

    //verify matrix 
    display(m, n, MachineRoomMatrix);

    //update matrix in factory file
    updateMatrix(MachineRoom,MachineRoomMatrix);

    freedom(m, MachineRoomMatrix); 
}

void removeMachineFromRoomFile(char *fileName, int id) {
    FILE *fptrOld = fopen(fileName, "rb");
    FILE *fptrNew = fopen("temp_room.bin", "wb");
    struct Machine m;

    while (fread(&m, sizeof(m), 1, fptrOld) == 1) {
        if (m.machineId != id) {
            fwrite(&m, sizeof(m), 1, fptrNew);
        }
    }

    fclose(fptrOld);
    fclose(fptrNew);

    remove(fileName);
    rename("temp_room.bin", fileName);
}

