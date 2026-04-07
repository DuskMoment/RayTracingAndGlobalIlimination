#version 450
#define IX2(i,j) ((i)+(N+2)*(j)) 
/*
THIS RUNS PER PIXEL --> run this 20 times the gpu
and then also in between each loop use SETBND
*/



//CHANGE THE GRIDSIZE HERE 
const int GRID_LENGHT = 798;
const int GRID_SIZE = (GRID_LENGHT + 2) * (GRID_LENGHT + 2);


