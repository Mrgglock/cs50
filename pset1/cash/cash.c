#include <stdio.h>
#include <cs50.h>
#include <math.h>

int main(void) {
    // Get money from user
    float money;
    do {
        money = get_float("Change owed: ");
    } while (money < 0);
    int cents = round(money * 100);
    
    // Counting algorithm
    int numberOfCoins = 0;
    while (cents != 0) {
        if (cents >= 25) {
            cents -= 25;
        } else if (cents >= 10) {
            cents -= 10;
        } else if (cents >= 5) {
            cents -= 5;
        } else {
            cents -= 1;
        };
        numberOfCoins++;
    }
    printf("%i\n", numberOfCoins);
}