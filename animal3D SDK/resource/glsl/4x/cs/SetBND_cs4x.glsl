#version 450

//CHANGE THE GRIDSIZE HERE 
const int GRID_LENGHT = 798;
const int GRID_SIZE = (GRID_LENGHT + 2) * (GRID_LENGHT + 2);


//THIS SHOULD ONLY HAVE ONE WORK GROUP
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 1) buffer bufferInOut {

	float float_bufferInOut[];
};

uniform int uDirection;

#define IX2(i,j) ((i)+(N+2)*(j)) 

void main()
{

    int i, N;

    /*
    * loop iterates of the dashes for calculation of bounds
    * 0----0
    * |	   |
    * |	   |
    * |    |
    * 0----0
    */
    for (i = 1; i <= N; i++) {

        float leftVert = float_bufferInOut[IX2(1, i)];
        float rightVert = float_bufferInOut[IX2(N, i)];

        float topHori = float_bufferInOut[IX2(i, 1)];
        float bottomHori = float_bufferInOut[IX2(i, N)];


        float_bufferInOut[IX2(0, i)] = uDirection == 1 ? ( -1.0 * leftVert) : leftVert;

        float_bufferInOut[IX2(N + 1, i)] = uDirection == 1 ? (-1.0 * rightVert): rightVert;

        float_bufferInOut[IX2(i, 0)] = uDirection == 2 ? (-1.0 * topHori) : topHori;

        float_bufferInOut[IX2(i, N + 1)] = uDirection == 2 ? (-1.0 * bottomHori) : bottomHori;

        //add third dimention here 
    }

    //this handles the corners takes the average using adjacecys 

    float h = 0.5;
    //top left
    float_bufferInOut[IX2(0, 0)] = h * (float_bufferInOut[IX2(1, 0)] + float_bufferInOut[IX2(0, 1)]);

    //bottom left
    float_bufferInOut[IX2(0, N + 1)] = h * (float_bufferInOut[IX2(1, N + 1)] + float_bufferInOut[IX2(0, N)]);

    //top right
    float_bufferInOut[IX2(N + 1, 0)] = h * (float_bufferInOut[IX2(N, 0)] + float_bufferInOut[IX2(N + 1, 1)]);

    //bottom right
    float_bufferInOut[IX2(N + 1, N + 1)] = h * (float_bufferInOut[IX2(N, N + 1)] + float_bufferInOut[IX2(N + 1, N)]);

    //add third dimention here 

}


