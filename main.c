#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool isEven(int n)
{
    // n^1 is n+1, then even, else odd
    if ( (n ^ 1) == n + 1 )
        return true;
    else
        return false;
}

int main( int argc, char *argv[argc+1]) {

    //File name from arguments
    if (argc != 2 ){
        printf("no file");
        return EXIT_SUCCESS;
    }

    // Declare the read file
    FILE *fp;

    fp = fopen( argv[1], "r");

    // Check if the file is empty
    if ( fp == NULL ){
        printf("Unable to read");
        return EXIT_SUCCESS;
    }
    // Get the data
    int num0;
    int num1;

    while ( fscanf( fp, "%d %d", &num0, &num1 ) != EOF ){
        printf("%d %d\n",num0, num1);
    }
    // Close the file and destroy memory allocations
    fclose(fp);




    return EXIT_SUCCESS;
}
