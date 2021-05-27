// Implements a dictionary's functionality

#include <stdbool.h>

#include "dictionary.h"
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// Number of buckets in hash table
// AAA to ZZZ, apos and endofword('\0') counted.
const int startAmount = 3;
const unsigned int N = 21952; // 28 ^ 3

// Hash table
node *table[N];

// Returns true if word is in dictionary, else false
bool checkWord(node *leaf, const char *word);
bool check(const char *word)
{
    int len = strlen(word);
    char loweredWord[46] = {0};
    // to lower the entire word
    for (int i = 0; i < len; i++) {
        loweredWord[i] = tolower(word[i]);
    }
    int hashedNumber = hash(loweredWord);
    return checkWord(table[hashedNumber], loweredWord);
}

bool checkWord(node *leaf, const char *word) {
    if (leaf == NULL) {
        return false;
    } else if (strcmp(leaf->word, word) == 0) {
        return true;
    } else {
        return checkWord(leaf->next, word);
    }
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    char firstThree[3] = {0};
    // Load first three chars
    for (int i = 0; i < 3; i++) {
        firstThree[i] = word[i];
        if (word[i] == '\0') {
            // End of word.
            break;
        }
    }
    // Turn that into a base 28 number to get ready to hash..
    // \0 is 0, A is 1, Z is 26, Apostrophe is 27.
    int firstInts[3];
    for (int i = 0; i < 3; i++) {
        if (firstThree[i] == '\'') {
            // If it is an apostrophe
            firstInts[i] = 27;
        } else if (firstThree[i] == '\0') {
            firstInts[i] = 0;
        } else {
            firstInts[i] = firstThree[i] - 'a' + 1;
        }
    }
    // Turn that into a number.
    int hashedNumber = 0;
    for (int i = 0; i < 3; i++) {
        hashedNumber += firstInts[i] * pow(28, 3 - 1 - i);
    }
    return hashedNumber;
}

void logWord(char *word);
// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    FILE *txt = fopen(dictionary, "r");
    char buffer;
    static char word[LENGTH + 1];
    // counter for where you are in the word
    int counter = 0;
    // Read each char until it's gone to get a word.
    while(fread(&buffer, sizeof(char), 1, txt)) {
        // Check if the char is a letter or apostrophe, to continue the word.
        if ( (buffer >= 'a' && buffer <= 'z') || buffer == '\'') {
            // Continue the word.
            word[counter] = buffer;
            counter++;
        } else {
            // Line break.
            if (counter != 0) {
                // Stop the string with a NUL char
                word[counter] = '\0';
                // Log the word in the dictionary.
                logWord(word);
                // Reset the char array word.
                for (int i = 0; i < LENGTH + 1; i++) {
                    if (word[i] == '\0') break;
                    word[i] = '\0';
                }
            }
            // Reset the counter
            counter = 0;
        }
    }
    fclose(txt);
    return true;
}

// Given a word, load it into memory with the linked lists.
void addWord(node **location, node *pointer, char *word);
void logWord(char *word) {
    int hashedNumber = hash(word);
    // Add the word.
    addWord(&table[hashedNumber], table[hashedNumber], word);
    return;
}

void addWord(node **location, node *pointer, char *word) {
    if (pointer == NULL) {
        // create a new block and link it up
        node *n = malloc(sizeof(node));
        *location = n;
        strcpy(n->word, word);
        n->next = NULL;
    } else {
        addWord(&(pointer->next), pointer->next, word);
    }
    return;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
int countFromNode(node *leaf);
unsigned int size(void)
{
    int totalCounter = 0;
    for (int i = 0; i < N; i++) {
        totalCounter += countFromNode(table[i]);
    }
    return totalCounter;
}

int countFromNode(node *leaf) {
    if (leaf == NULL) {
        return 0;
    } else {
        return countFromNode(leaf->next) + 1;
    }
}

void freeLeaf(node *leaf);
// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    for (int i = 0; i < N; i++) {
        if (table[i] != NULL) {
            freeLeaf(table[i]);
        }
    }
    return true;
}

void freeLeaf(node *leaf) {
    if (leaf == NULL) return;
    node *nextPointer = leaf->next;
    free(leaf);
    freeLeaf(nextPointer);
}