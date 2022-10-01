#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "lib/LodePNG/lodepng.h"

# define M_PI 3.14159265358979323846

void IIRBlur(unsigned char *pixels, int w, int h, float fc /*Cutoff freq*/)
{
    float *Y = (float*)calloc(w*h*4, sizeof(float));
    float *X = (float*)calloc(w*h*4, sizeof(float));

    for(int i = 0; i < h*w*4; i++)
    {
        X[i] = 2.f*(float)pixels[i]/255.f - 1.f;
        Y[i] = X[i];
    }

    // Sample rate
    float fs = 200.f;

    // Q factor
    float Q = 0.7f;

    float Wc = (M_PI*2.f*fc)/fs;
    float wS = sinf(Wc);
    float wC = cosf(Wc);
    float a = wS/(2.f*Q);

    float b0 = (1.f - wC)/2.f;
    float b1 = 1.f - wC;
    float b2 = b0;

    float a0 = 1.f + a;
    float a1 = -2.f*wC;
    float a2 = 1.f - a;

    float x_n_0, x_n_1, x_n_2, y_n_1, y_n_2;
    int i,j;

    // H pass
    for(int y = 0; y < h; y++)
    {
        int row = w*y*4;

        for(int x = 2; x < w; x++)
        {

            //R
            i = row + x*4;

            x_n_0 = X[i];
            x_n_1 = X[i-4];
            x_n_2 = X[i-8];
            y_n_1 = Y[i-4];
            y_n_2 = Y[i-8];
            Y[i] = (b0*x_n_0 + b1*x_n_1 + b2*x_n_2 - a1*y_n_1 - a2*y_n_2)/a0;

            //G
            i += 1;
            x_n_0 = X[i];
            x_n_1 = X[i-4];
            x_n_2 = X[i-8];
            y_n_1 = Y[i-4];
            y_n_2 = Y[i-8];
            Y[i] = (b0*x_n_0 + b1*x_n_1 + b2*x_n_2 - a1*y_n_1 - a2*y_n_2)/a0;

            //B
            i += 1;
            x_n_0 = X[i];
            x_n_1 = X[i-4];
            x_n_2 = X[i-8];
            y_n_1 = Y[i-4];
            y_n_2 = Y[i-8];
            Y[i] = (b0*x_n_0 + b1*x_n_1 + b2*x_n_2 - a1*y_n_1 - a2*y_n_2)/a0;
        }
    }

    memcpy(X,Y,w*h*4*sizeof(float));

    float max = 0.000000001f;

    // V pass
    for(int x = 0; x < w; x++)
    {
        for(int y = 2; y < h; y++)
        {

            //R
            j = w*4;
            i = j*y + x*4;
            x_n_0 = X[i];
            x_n_1 = X[i-j];
            x_n_2 = X[i-j*2];
            y_n_1 = Y[i-j];
            y_n_2 = Y[i-j*2];
            Y[i] = (b0*x_n_0 + b1*x_n_1 + b2*x_n_2 - a1*y_n_1 - a2*y_n_2)/a0;

            if(fabs(Y[i]) > max)
                max = fabs(Y[i]);

            //G
            i += 1;
            x_n_0 = X[i];
            x_n_1 = X[i-j];
            x_n_2 = X[i-j*2];
            y_n_1 = Y[i-j];
            y_n_2 = Y[i-j*2];
            Y[i] = (b0*x_n_0 + b1*x_n_1 + b2*x_n_2 - a1*y_n_1 - a2*y_n_2)/a0;

            if(fabs(Y[i]) > max)
                max = fabs(Y[i]);

            //B
            i += 1;
            x_n_0 = X[i];
            x_n_1 = X[i-j];
            x_n_2 = X[i-j*2];
            y_n_1 = Y[i-j];
            y_n_2 = Y[i-j*2];
            Y[i] = (b0*x_n_0 + b1*x_n_1 + b2*x_n_2 - a1*y_n_1 - a2*y_n_2)/a0;

            if(fabs(Y[i]) > max)
                max = fabs(Y[i]);

        }
    }


    for(int z = 0; z < h*w*4; z++)
    {
        if((z+1) % 4 != 0)
            pixels[z] = 255.f*(Y[z]/max + 1.f)/2.f;
    }

    free(Y);
    free(X);
}

int main(int argc, char **argv)
{
    unsigned int error;
    unsigned char *image;
    unsigned int width, height;

    // Load PNG
    error = lodepng_decode32_file(&image, &width, &height, argv[1]);

    if(error)
        printf("Error %u: %s\n", error, lodepng_error_text(error));

    // Apply filter
    IIRBlur(image, width, height, atof(argv[2]));

    // Save result
    error = lodepng_encode32_file("Result.png", image, width, height);

    if(error)
        printf("Error %u: %s\n", error, lodepng_error_text(error));

    free(image);

    return 0;
}
