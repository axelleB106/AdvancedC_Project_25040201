#include <stdio.h>
#include <string.h> 
#include <stdlib.h>
#include <math.h>

#include "header.h"


/* Notes : 
Advanced C programming
BOULANGER AXELLE 
Student ID: 25040201
Project
Factory Machine-room floor plan -- Room Creation file
*/

/*
Project's Notes:

Ask next class --> How to use header file ? and launch from there

-----------------------------------------------------------
Room
- user create a room - room is a matrix 
- define size - use malloc exo 2 
- initialize w/ only 0 = empty room 

-- Now we need to be able to create multiple rooms 
--when room is created put it inside a file 

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

Left to do : 
--> Separate project in multiple file (use of header) - ask teacher 

*/
//STRUCT================================================================================

struct Machine {
    int machineId;
    int Row;
    int Col;
    int Maintenance; 
}; 

int nMachine; //id for machines


//SKULL FUNCTIONS================================================================================
void checkExistingF(FILE **fptr);
void SupprRoomFiles();
int numberOfRooms();
void display(int m, int n, int **ptr1); //common to every file

void createRoom();
void matrixSize(int *rows, int *cols);
void matrixCreation(int m,int n, int ***ptr1); // common to machine and room files
void doorInput(int m, int n, int **ptr1);
void updateFile(int m, int n, int **ptr1);
void freedom(int m, int **ptr1); // common to machine and room files

void displayFile(FILE *fptr);

void displayROOM(int roomNum);

void createMachine();
int** getRoomInfo(int* roomNum, int *rows, int *cols);
void NewMachineInfo(int m,int n, int **CurrRoomMatrix, int roomNum);
void MachinePos(int m, int n, int **ptr1, int* inputRow, int* inputCol);
int doorToMach(int doorM, int doorN, int col, int row, int m, int n, int**ptr);
int nextPt(int testm, int testn, int m, int n, int ptm, int ptn, int** ptr);
int border (int ptm, int ptn, int m, int n, int** ptr);
int checkMach(int testm, int testn, int m, int n, int ptm, int ptn);
void cleanMatrix(int m, int n, int**ptr);
void updateMatrix(int roomNum,int **CurrRoomMatrix);
void saveMachineId();
void ReturnMachineId();

void displayMachines();
void displayMachinesRoom();
void displayMachinesID();

void MaintenanceStatus();
void MaintenanceList();

void RemoveMachinesID();
void removeMachineFromRoomFile(char *fileName, int id);

//FUNCTIONS================================================================================

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

//INIT FACTORY ========================================================================================================================

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

int numberOfRooms(){
    //method - counting  the number of room - up to the number - then getting that one 
    FILE *fptr;
    int m, n; // read them for each room
    int current = 0;

    if ((fptr = fopen("factory.bin", "rb")) == NULL){
    printf("No existing file detected or path not working\n");
    return 0;}
        
    // Loop through all stored rooms
    while (fread(&m, sizeof(int), 1, fptr) == 1 &&
        fread(&n, sizeof(int), 1, fptr) == 1)
    {
        fseek(fptr, (long)m * n * sizeof(int), SEEK_CUR); // skip matrix
        current +=1;
    }

    fclose(fptr);
    return current; 
}

//CREATE ROOM ========================================================================================================================

void createRoom() //CREATE A ROOM MATRIX 
{ 
    //Variables 
    int **ptr1;
    int m,n; // n cols - m rows 

    //Function call
    matrixSize(&m,&n); //recup dimension
    matrixCreation(m,n,&ptr1); // need else matrix won't change
    doorInput(m,n, ptr1);
    display(m,n, ptr1);
    updateFile(m, n, ptr1);
    freedom(m, ptr1);
}

void matrixSize(int *rows, int *cols) //INPUT MATRIX SIZE
{
    printf("Enter number of rows: ");
    scanf("%d",rows);
    printf("Enter number of columns: ");
    scanf("%d",cols);
    printf("\n");
}

void matrixCreation(int m,int n, int***ptr1){
    //----------------rows----------------
    *ptr1 = (int**)calloc(m, sizeof(int*));
    if (*ptr1 == NULL) { printf("Memory not allocated.\n"); exit(0);}

    //----------------cols----------------
    for (int i = 0; i < m; i++)
    {   
        //go and allocate each cols one row at a time
        (*ptr1)[i] = (int*)calloc(n, sizeof(int));
        if ((*ptr1)[i] == NULL) { printf("Memory not allocated for row %d.\n",i); exit(0);}
    }
}

void doorInput(int m, int n, int **ptr1) 
{
    int row, col, ok=0; 
    
    //handle exception error message == ok 
    while (ok ==0){
        printf("Input door position -> rows(y) : ");
        scanf("%d", &row);
        printf("-> columns (x) : ");
        scanf("%d", &col);
        col= col-1; 
        row = m -row;
        //printf(" row %d, col  %d\n", row, col); -- test
        if (col == 0 && row <m && row >=0 || col == n-1 && row <m && row >=0 || row == 0 && col <n && col >=0 ||row == m-1 && col <n && col >=0){
            ok =1;
        }
        else if (col < 0 || col >= n || row < 0 || row >= m){
            printf("Out of bounds.\n");
        }
        else{
            printf("Door must sit on a perimeter wall\n");
        }
    }
    //put all values = 0  
    for (int i = 0; i < m; i++)
    {   
        for (int j = 0; j < n; j++) 
        {
            ptr1[i][j] = 0; 
        }
    } 

    ptr1[row][col]=2; 
}

void display(int m, int n, int **ptr) 
{
    //input information 
    printf("-------ROOM-------\n");
        for (int i = 0; i < m; i++)
        {   
            for (int j =0; j< n; ++j)
            { printf("%d ", ptr[i][j]);}
            printf("\n");
        } 
}

void updateFile(int m, int n,int **ptr1){
    FILE *fptr;
    //updating file
    if ((fptr = fopen("factory.bin", "ab"))== NULL)
    {printf("Error - verify file path or permissions \n");
    return;}

    //write room size for display 
    fwrite(&m, sizeof(int), 1, fptr);
    fwrite(&n, sizeof(int), 1, fptr);

    // Then write all matrix values
    for (int i = 0; i < m; i++) {
        fwrite(ptr1[i], sizeof(int), n, fptr);
    }
    fclose(fptr);

}

void freedom(int m, int **ptr)
{
    for (int i = 0; i < m; i++) {free(ptr[i]);}
    free(ptr);
}

//DISPLAY FILE ========================================================================================================================

void displayFile(FILE *fptr)
{
    int m,n, count=1;

    printf("\n");
    printf("=========FACTORY ROOMS=========\n");
    if ((fptr = fopen("factory.bin", "rb"))== NULL)
    {printf("Error - verify file path or permissions \n"); return;}

    //read all the file
    while (fread(&m, sizeof(int), 1, fptr) == 1 &&
           fread(&n, sizeof(int), 1, fptr) == 1)
    {
        printf("\n---ROOM %d - Size : %dx%d\n", count, m, n);
        //need buffer of right type
        //buffer matrice 
        int **bufferMatrix;
        matrixCreation(m, n, &bufferMatrix);
        for (int i = 0; i < m; i++)
        {   
            fread(bufferMatrix[i], sizeof(int), n, fptr);
        }
        display(m,n,bufferMatrix);
        freedom(m, bufferMatrix); 
        count++; 
    }

    fclose(fptr);
}

//DISPLAY ROOM BY ID  ========================================================================================================================

void displayROOM(int roomNum){
    //method - counting  the number of room - up to the number - then getting that one 
    FILE *fptr;
    int m, n; // read them for each room
    int current = 1;
    int found = 0; 

    if ((fptr = fopen("factory.bin", "rb")) == NULL){
        printf("No existing file detected or path not working\n");
        return;
    }

    // Loop through all stored rooms
    while (fread(&m, sizeof(int), 1, fptr) == 1 &&
           fread(&n, sizeof(int), 1, fptr) == 1)
    {

        //bug catching prints-----
        //printf("current room num %d\n",current);
        //printf("m %d et n %d \n",m, n);
        
        if (current == roomNum)
        {
            printf("\n--- ROOM %d (Size %dx%d) ---\n", roomNum, m, n);

            // //buffer of right type 
            int **buffer;
            matrixCreation(m, n, &buffer);

            for (int i = 0; i < m; i++)
                fread(buffer[i], sizeof(int), n, fptr);

            // display matrix
            display(m, n, buffer);
            freedom(m, buffer);

            found=1;
            fclose(fptr);
            return;
        }
        else {fseek(fptr, sizeof(int) * m * n, SEEK_CUR);} //skip to realign the bytes
        current +=1;
    }
    if (found ==0) {printf("\nRoom Number not found\n");}
}

//ADD MACHINE ========================================================================================================================

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

    //Machine Id --------------------------------
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

//DISPLAY MACHINES ========================================================================================================================

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

//MAINTENANCE ========================================================================================================================

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
                        printf("Position on y axis: %d\n", bufferMachine.Col);
                        printf("Maintenance status: %d\n", bufferMachine.Maintenance);
                        printf("\n");
                    }
            }
        fclose(fptrRoom);    
    }
}

//REMOVE MACHINE ========================================================================================================================

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

