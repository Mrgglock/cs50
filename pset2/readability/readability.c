#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int colemanLieuIndex(string paragraph);

int main(void) {
    string paragraph = get_string("Text: ");
    int index = colemanLieuIndex(paragraph);
    if (index < 1) {
        printf("Before Grade 1\n");
    } else if (index >= 16) {
        printf("Grade 16+\n");
    } else {
        printf("Grade ");
        printf("%i\n", index);
    }
}

int colemanLieuIndex(string paragraph) {
    int numberOfSentences = 0;
    int numberOfWords = 0;
    int numberOfLetters = 0;
    // Loop through each character of the paragraph.
    for (int i = 0, n = strlen(paragraph); i < n; i++) {
        char character = paragraph[i];
        if (character == '.' || character == '!' || character == '?') {
            // If a sentence ending character is found
            numberOfSentences++;
        } else if (character == ' ') {
            numberOfWords++;
        } else if ((character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z')) {
            numberOfLetters++;
        }
    }
    // Account for the final word.
    numberOfWords++;
    float L = numberOfLetters / (float) numberOfWords * 100;
    float S = numberOfSentences / (float) numberOfWords * 100;
    float decimalIndex = 0.0588*L - 0.296*S - 15.8;
    return round(decimalIndex);
}