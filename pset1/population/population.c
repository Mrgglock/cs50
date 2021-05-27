#include <stdio.h>
#include <cs50.h>

int main(void) {
    int populationSize;
    do {
        populationSize = get_int("Population Size: ");
    } while (populationSize < 9);
    int endingPopulationSize;
    do {
        endingPopulationSize = get_int("Ending Population Size: ");
    } while (endingPopulationSize < populationSize);
    int numberOfYears = 0;
    while (populationSize < endingPopulationSize) {
        int born = populationSize / 3;
        int die = populationSize / 4;
        populationSize += born;
        populationSize -= die;
        numberOfYears++;
    };
    printf("%s", "Years: ");
    printf("%i\n", numberOfYears);
}