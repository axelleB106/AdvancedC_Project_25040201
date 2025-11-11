#include <stdio.h>
#include <string.h> 
#include <stdlib.h>



/* Notes : 
Advanced C programming
BOULANGER AXELLE 
Student ID: 25040201
Project
Factory Machine-room floor plan -- Room Creation file
*/

/*
Project 

- user create a room - room is a matrix 
- define size - use malloc exo 2 
- initialize w/ only 0 = empty room 

-- Now we need to be able to create multiple rooms 
--when room is created put it inside a file 

--We can now display all of the files room without pb 
--need to add the room door position input 

*/

//SKULL FUNCTIONS================================================================================
void createRoom();
void matrixSize(int *rows, int *cols);
void matrixCreation(int m,int n, int ***ptr1);
void freedom(int m, int **ptr1);
void inputValues(int m, int n, int **ptr1); // only zero 
void display(int m, int n, int **ptr1);
void checkExistingF(FILE *fptr);
void updateFile(int m, int n, int **ptr1);
void displayFile(FILE *fptr);

//FUNCTIONS================================================================================
int main ()
{
    //variables
    FILE *fptr;
    checkExistingF(fptr);
    createRoom(); // instead of displaying put in a file 
    displayFile(fptr); 
    return 0;  
}

void checkExistingF(FILE *fptr) //CREATE OR UPDATE FACTORY 
{   
    int redo=0;
    printf("=====FACTORY=====\n");

    if ((fptr = fopen("factory.bin", "rb"))== NULL)
    {
        printf("No existing file detected or path not working \n");
    }

    else
    {
        printf("if you would you like to keep working the existing factory press '1'\n"
                "else press '0' to reset it : ");
        scanf("%d",&redo);
        fclose(fptr);
    }

    //printf("%d\n",redo);

    if (redo == 0)
    {   
        if ((fptr = fopen("factory.bin", "wb"))== NULL)
        {printf("Error - verify file path or permissions \n");}
        else {fclose(fptr);}
        printf("\n");
        printf("=====FACTORY CREATION=====\n");
    }
    else
    {
        printf("\n");
        printf("=====FACTORY UPDATING=====\n");
    }
}

void createRoom() //CREATE A ROOM MATRIX 
{ 
    //Variables 
    int **ptr1;
    int m,n; // n cols - m rows 

    //Function call
    matrixSize(&m,&n); //recup dimension
    matrixCreation(m,n,&ptr1); // need else matrix won't change
    inputValues(m,n, ptr1);
    display(m,n, ptr1);
    updateFile(m, n, ptr1);
    freedom(m, ptr1);
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

void matrixSize(int *rows, int *cols) //INPUT MATRIX SIZE
{
    printf("Enter number of rows: ");
    scanf("%d",rows);
    printf("Enter number of columns: ");
    scanf("%d",cols);
    printf("\n");
}

void inputValues(int m, int n, int **ptr1) //Need to add door
{
    //put all values = 0  
    for (int i = 0; i < m; i++)
    {   
        for (int j = 0; j < n; j++) 
        {
            ptr1[i][j] = 0; 
        }
    } 
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

void freedom(int m, int **ptr)
{
    for (int i = 0; i < m; i++) {free(ptr[i]);}
    free(ptr);
}

