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

// Convert image to sepia
void sepia(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            RGBTRIPLE pixel = image[i][j];
            int blue = pixel.rgbtBlue;
            int green = pixel.rgbtGreen;
            int red = pixel.rgbtRed;
            float sepiaRed = 0.393 * red + 0.769 * green + 0.189 * blue;
            if (sepiaRed > 255) {
                sepiaRed = 255;
            }
            float sepiaGreen = 0.349 * red + 0.686 * green + 0.168 * blue;
            if (sepiaGreen > 255) {
                sepiaGreen = 255;
            }
            float sepiaBlue = 0.272 * red + 0.534 * green + 0.131 * blue;
            if (sepiaBlue > 255) {
                sepiaBlue = 255;
            }
            sepiaRed = (BYTE) round(sepiaRed);
            sepiaBlue = (BYTE) round(sepiaBlue);
            sepiaGreen = (BYTE) round(sepiaGreen);
            image[i][j].rgbtRed = sepiaRed;
            image[i][j].rgbtGreen = sepiaGreen;
            image[i][j].rgbtBlue = sepiaBlue;
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
