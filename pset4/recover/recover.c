#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

void incrementImageName(void);
typedef uint8_t BYTE;
bool startOfJPG(BYTE byte0, BYTE byte1, BYTE byte2, BYTE byte3);

// Set up the name of the file to write into ###.jpg
// This may be incremented by calling the function incrementImageName()
int imageNumber = 0;
char imageName[7] = {'0', '0', '0', '.', 'j', 'p', 'g'};

int main(int argc, char *argv[])
{
    // 1 Command Line Argument, namely card.raw
    if (argc != 2) {
        printf("Usage: ./recover *.raw\n");
        return 1;
    }
    // Try to open card.raw
    FILE *input = fopen(argv[1], "r");
    if (input == NULL) {
        printf("Input file is invalid.\n");
        return 1;
    }

    // Start looping

    BYTE block[512];
    // Start reading in blocks of 512 bytes, which is 512 * 1 bytes.
    FILE *output = NULL;
    while (fread(block, sizeof(BYTE), 512, input)) {
        // Is it the start of a new JPG?
        if (startOfJPG(block[0], block[1], block[2], block[3])) {
            // Open a new JPG file to write to.
            output = fopen(imageName, "w");
            incrementImageName();
        }
        if (output == NULL) continue;
        // write to this output file
        fwrite(block, sizeof(BYTE), 512, output);
    }
    return 0;
}

void incrementImageName(void) {
    // Increment number to analyse
    imageNumber++;
    int firstDigit = 0;
    int secondDigit = 0;
    int thirdDigit = 0;
    firstDigit = imageNumber / 100;
    secondDigit = (imageNumber / 10) % 10;
    thirdDigit = imageNumber % 10;
    sprintf(imageName, "%i%i%i.jpg", firstDigit, secondDigit, thirdDigit);
}

bool startOfJPG(BYTE byte0, BYTE byte1, BYTE byte2, BYTE byte3) {
    if (byte0 != 0xff) return false;
    if (byte1 != 0xd8) return false;
    if (byte2 != 0xff) return false;
    if (byte3 >= 0xe0 && byte3 <= 0xef) return true;
    return false;
}