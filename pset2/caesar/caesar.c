#include <stdio.h>
#include <cs50.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

char scramble(char character, int key);

int main(int argC, string args[]) {
    // Expecting a positive int as an arg for the key.
    if (argC != 2) {
        // Command Line isn't in the expected form.
        printf("Error: Wrong command line format.\n");
        return 1;
    }
    // 2 Command Line Arguments. Check if arg is a key.
    string keyCommandLine = args[1];
    int length = strlen(keyCommandLine);
    for (int i = 0; i < length; i++) {
        if (!isdigit(keyCommandLine[i])) {
            printf("Error: Not a number.\n");
            return 1;
        }
    }
    // It is a key if this line is reached.
    // Convert it to a number.
    int key = 0;
    long place = 1;
    for (int i = length - 1; i >= 0; i--) {
        // Offset to true number by pos of char '0' in ASCII
        int number = keyCommandLine[i] - '0';
        key += number * place;
        place = place * 10;
    }
    key = key % 26;
    // Get plaintext
    string plaintext = get_string("plaintext: ");
    // Start scramble process.
    printf("ciphertext: ");
    for (int i = 0, n = strlen(plaintext); i < n; i++) {
        char character = plaintext[i];
        character = scramble(character, key);
        printf("%c", character);
    }
    printf("\n");
    return 0;
}

char scramble(char character, int key) {
    if (!isalpha(character)) return character;
    long ASCIICode = character;
    ASCIICode += key;
    if (islower(character) && ASCIICode > 'z') ASCIICode -= 26;
    if (isupper(character) && ASCIICode > 'Z') ASCIICode -= 26;
    return ASCIICode;
}