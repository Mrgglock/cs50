#include <stdio.h>
#include <cs50.h>

string layer(int number, int max);

int main(void) {
    int height;
    
    // Query the user for their height.
    do {
        height = get_int("Height: ");
    } while (height < 1 || height > 8);
    
    // Generate the pyramid layer by layer
    for (int i = 0; i < height; i++) {
        layer(i + 1, height);
        printf("\n");
    }
}

// Pyramid Layer Function
string layer(int number, int max) {
    int spaces = max - number;
    // Leading whitespace
    for (int i = 0; i < spaces; i++) {
        printf("%s", " ");
    };
    // Bricks
    for (int i = 0; i < number; i++) {
        printf("%s", "#");
    };
    // Gap
    printf("%s", "  ");
    // Bricks
    for (int i = 0; i < number; i++) {
        printf("%s", "#");
    };
    return 0;
}