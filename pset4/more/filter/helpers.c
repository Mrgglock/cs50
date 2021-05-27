#include "helpers.h"
#include <stdbool.h>
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            RGBTRIPLE pixel = image[i][j];
            int blue = pixel.rgbtBlue;
            int green = pixel.rgbtGreen;
            int red = pixel.rgbtRed;
            float average = (red + green + blue) / (float) 3;
            average = (BYTE) round(average);
            image[i][j].rgbtRed = average;
            image[i][j].rgbtGreen = average;
            image[i][j].rgbtBlue = average;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++) {
        for (int j = 0, halfway = width / 2; j < halfway; j++) {
            // swap pixel j with pixel length - 1 - j
            int complement = width - 1 - j;
            RGBTRIPLE tmp = image[i][j];
            image[i][j] = image[i][complement];
            image[i][complement] = tmp;
        }
    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    
    // Create new temp image to calculate averages from
    RGBTRIPLE tmpimage[height][width];
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            tmpimage[h][w] = image[h][w];
        }
    }
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            // neighbours are
            // [i - 1][j - 1], [i - 1][j], [i - 1][j + 1]
            // [i]    [j - 1], [i]    [j], [i]    [j + 1]
            // [i + 1][j - 1], [i + 1][j], [i + 1][j + 1]
            // check if on left edge, top edge, bottom edge and right edge.
            bool onLeftEdge = false;
            bool onTopEdge = false;
            bool onBottomEdge = false;
            bool onRightEdge = false;
            if (i == 0) {
                onTopEdge = true;
            }
            if (j == 0) {
                onLeftEdge = true;
            }
            if (i == height - 1) {
                onBottomEdge = true;
            }
            if (j == width - 1) {
                onRightEdge = true;
            }
            
            // get averages
            int totalNeighbours = 0;
            float red = 0;
            float blue = 0;
            float green = 0;
            for (int iShift = -1; iShift <= 1; iShift++) {
                for (int jShift = -1; jShift <= 1; jShift++) {
                    if (iShift == -1 && onTopEdge) continue;
                    if (iShift == 1 && onBottomEdge) continue;
                    if (jShift == -1 && onLeftEdge) continue;
                    if (jShift == 1 && onRightEdge) continue;
                    totalNeighbours++;
                    RGBTRIPLE pixel = tmpimage[i + iShift][j + jShift];
                    red += pixel.rgbtRed;
                    blue += pixel.rgbtBlue;
                    green += pixel.rgbtGreen;
                }
            }
            red = (BYTE) round(red / totalNeighbours);
            blue = (BYTE) round(blue / totalNeighbours);
            green = (BYTE) round(green / totalNeighbours);
            image[i][j].rgbtRed = red;
            image[i][j].rgbtGreen = green;
            image[i][j].rgbtBlue = blue;
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // create a temporary image
    RGBTRIPLE tmpimage[height][width];
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            tmpimage[h][w] = image[h][w];
        }
    }
    
    // Kernel setups
    int gxKernel[3][3] = {{-1,  0,  1},
                          {-2,  0,  2},
                          {-1,  0,  1}};
    int gyKernel[3][3] = {{-1, -2, -1},
                          {0,  0,  0},
                          {1,  2,  1}};
    
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            RGBTRIPLE pixel = tmpimage[i][j];
            // edge detection
            bool onLeftEdge = false;
            bool onTopEdge = false;
            bool onRightEdge = false;
            bool onBottomEdge = false;
            if (i == 0) {
                onTopEdge = true;
            }
            if (j == 0) {
                onLeftEdge = true;
            }
            if (i == height - 1) {
                onBottomEdge = true;
            }
            if (j == width - 1) {
                onRightEdge = true;
            }
            // set up similar kernels for each colour channel
            // log down red, blue and green on gx and gy.
            
            int redKernel[3][3];
            int greenKernel[3][3];
            int blueKernel[3][3];
            // go through every neighbour
            for (int iShift = -1; iShift <= 1; iShift++) {
                for (int jShift = -1; jShift <= 1; jShift++) {
                    // if there is no neighbour, 0 0 0.
                    if ( (iShift == -1 && onTopEdge   ) ||
                         (iShift ==  1 && onBottomEdge) ||
                         (jShift == -1 && onLeftEdge  ) ||
                         (jShift ==  1 && onRightEdge ) ) 
                    {
                        redKernel[iShift + 1][jShift + 1] = 0;
                        greenKernel[iShift + 1][jShift + 1] = 0;
                        blueKernel[iShift + 1][jShift + 1] = 0;
                    } else {
                        // there is a neighbour.
                        RGBTRIPLE neighbour = tmpimage[i + iShift][j + jShift];
                        redKernel[iShift + 1][jShift + 1] = neighbour.rgbtRed;
                        greenKernel[iShift + 1][jShift + 1] = neighbour.rgbtGreen;
                        blueKernel[iShift + 1][jShift + 1] = neighbour.rgbtBlue;
                    }
                }
            }
            // redkernel greenkernel and bluekernel have been setup as the neighbours for this pixel.
            // multiply by gx and gy arrays now.
            // and calculate running sum of that.
            float redGxSum = 0.0;
            float greenGxSum = 0.0;
            float blueGxSum = 0.0;
            float redGySum = 0.0;
            float greenGySum = 0.0;
            float blueGySum = 0.0;
            for (int a = 0; a < 3; a++) {
                for (int b = 0; b < 3; b++) {
                    redGxSum += redKernel[a][b] * gxKernel[a][b];
                    greenGxSum += greenKernel[a][b] * gxKernel[a][b];
                    blueGxSum += blueKernel[a][b] * gxKernel[a][b];
                    redGySum += redKernel[a][b] * gyKernel[a][b];
                    greenGySum += greenKernel[a][b] * gyKernel[a][b];
                    blueGySum += blueKernel[a][b] * gyKernel[a][b];
                }
            }
            // calculate a final rgb value for this pixel
            int finalRed = round(sqrt(pow(redGxSum, 2) + pow(redGySum, 2)));
            int finalGreen = round(sqrt(pow(greenGxSum, 2) + pow(greenGySum, 2)));
            int finalBlue = round(sqrt(pow(blueGxSum, 2) + pow(blueGySum, 2)));
            if (finalRed > 255) finalRed = 255;
            if (finalGreen > 255) finalGreen = 255;
            if (finalBlue > 255) finalBlue = 255;
            // finally update the pixel of the image
            image[i][j].rgbtRed = finalRed;
            image[i][j].rgbtGreen = finalGreen;
            image[i][j].rgbtBlue = finalBlue;
        }
    }
    return;
}
