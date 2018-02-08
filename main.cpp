/* 
 * File:   main.cpp
 * Author: mk
 */
#include "SensorBoard.h"
#include <stdio.h>


int main(int argc, char **argv )
{
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc < 2)
    {
        printf("Specify a device.\n");
        return 1;
    }
    
    SensorBoard board(argv[1]);
    if (board.Start()) {
        // pritisni Enter konec
        getchar();
        board.Stop();
    }
  
    return 0;
}
