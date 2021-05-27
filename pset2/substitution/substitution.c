#include <cs50.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <string.h>

int main(int argCount, string arguments[]) {
    // Check if the usage is correct
    if (argCount != 2) {
        printf("Usage: ./substitution key\n");
        return 1;
    };
    // Check if the key has 26 characters
    string keyString = arguments[1];
    int keyLength = strlen(keyString);
    if (keyLength != 26) {
        printf("Key must contain 26 characters.\n");
        return 1;
    };
    // Check if the key is containing unique characters. 
    // Fill it with nulls, because apparently it isn't.
    static char lettersFound[25];
    // Also check if they are alphabets, or it's not allowed.
    for (int i = 0; i < 26; i++) {
        if (!isalpha(keyString[i])) {
            printf("Key must be alphabetical.\n");
            return 1;
        }
    }
    for (int i = 0; i < 26; i++) {
        // Find letter at start filling checklist array @char {lettersFound[]}
        char letter = keyString[i];
        letter = tolower(letter);
        int letterIndex = letter - 'a';
        // Check the checklist if it is there
        if (lettersFound[letterIndex] != 0) {
            printf("Duplicate letter found: %c\n", letter);
            return 1;
        };
        // It is not there. Fill it with something.
        lettersFound[letterIndex] = 1;
        // Standardise to lowercase.
        keyString[i] = letter;
    }
    // No duplicates found. Good to go.
    // Query plaintext
    string plaintext = get_string("plaintext: ");
    // Loop through plaintext and start checking against key
    // Prepare to print the cipher text out.
    printf("ciphertext: ");
    for (int i = 0, n = strlen(plaintext); i < n; i++) {
        char character = plaintext[i];
        if (!isalpha(character)) {
            printf("%c", character);
            continue;
        }
        int characterIndex = character;
        if (isupper(character)) {
            characterIndex -= 'A';
            // Match to key
            char newLower = keyString[characterIndex];
            // Print Upper
            printf("%c", newLower - 'a' + 'A');
        } else { // If it is lowercase
            characterIndex -= 'a';
            printf("%c", keyString[characterIndex]);
        }
    }
    printf("\n");
    return 0;
}