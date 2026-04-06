#version 450


#define IX2(i,j) ((i)+(N+2)*(j)) 
const int GRID_SIZE = 800 * 800;


//ONLY RUN IF USING THE LAST WORKER
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
