#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Factory_header.h"

// Room Creation / Display file 

// CREATE ROOM ===================================================================

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

// DISPLAY ALL ROOMS ============================================================

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

// DISPLAY ONE ROOM BY ID =======================================================

void displayROOM(int roomNum){
    //method - counting the number of room - up to the number - then getting that one 
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

// NUMBER OF ROOMS ==============================================================

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
