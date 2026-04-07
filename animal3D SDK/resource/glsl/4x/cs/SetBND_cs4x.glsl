#version 450

//CHANGE THE GRIDSIZE HERE 
const int GRID_LENGHT = 798;
const int GRID_SIZE = (GRID_LENGHT + 2) * (GRID_LENGHT + 2);


//THIS SHOULD ONLY HAVE ONE WORK GROUP
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) buffer bufferInOut {

	float float_bufferInOut[GRID_SIZE];
};

uniform int uDirection;

#define IX2(i,j) ((i)+(N+2)*(j)) 


//N = MAX_GRIDSIZE, b = horizontal or vertical
void FluidGridSetBnd(int N, int b, float x[GRID_SIZE])
{
    int i;

    /*
    * loop iterates of the dashes for calculation of bounds
    * 0----0
    * |	   |
    * |	   |
    * |    |
    * 0----0
    */
    for (i = 1; i <= N; i++) {

        float leftVert = x[IX2(1, i)];
        float rightVert = x[IX2(N, i)];

        float topHori = x[IX2(i, 1)];
        float bottomHori = x[IX2(i, N)];


        x[IX2(0, i)] = b == 1 ? ( -1.0 * leftVert) : leftVert;

        x[IX2(N + 1, i)] = b == 1 ? (-1.0 * rightVert): rightVert;

        x[IX2(i, 0)] = b == 2 ? (-1.0 * topHori) : topHori;

        x[IX2(i, N + 1)] = b == 2 ? (-1.0 * bottomHori) : bottomHori;

        //add third dimention here 
    }

    //this handles the corners takes the average using adjacecys 

    float h = 0.5;
    //top left
    x[IX2(0, 0)] = h * (x[IX2(1, 0)] + x[IX2(0, 1)]);

    //bottom left
    x[IX2(0, N + 1)] = h * (x[IX2(1, N + 1)] + x[IX2(0, N)]);

    //top right
    x[IX2(N + 1, 0)] = h * (x[IX2(N, 0)] + x[IX2(N + 1, 1)]);

    //bottom right
    x[IX2(N + 1, N + 1)] = h * (x[IX2(N, N + 1)] + x[IX2(N + 1, N)]);

    //add third dimention here 
 }

void main()
{

    FluidGridSetBnd(GRID_LENGHT, uDirection, float_bufferInOut);

}


