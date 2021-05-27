#include <stdio.h>
#include <cs50.h>

int main(void) {
    // Request a Number
    long creditCardNumber;
    creditCardNumber = get_long("Credit Card Number: ");
    // assumes first digit is not 0 from now

    // Create a Duplicate Number for length investigation.
    long number = creditCardNumber;
    int lengthOfNumber = 0;
    while (number != 0) {
        number = number / 10;
        lengthOfNumber++;
    }

    // Investigate first 2 digits.
    number = creditCardNumber;
    int firstTwoDigits;
    for (int i = 0; i < lengthOfNumber - 2; i++) {
        number = number / 10;
    }
    firstTwoDigits = number;

    // Get first digit for convenience.
    int firstDigit = firstTwoDigits / 10;

    // Check if the card is valid.
    number = creditCardNumber;
    int runningSum = 0;
    // Loop through every number.
    for (int i = 0; i < lengthOfNumber; i++) {
        int lastDigit = number % 10;
        // Check the digit's parity and proceed to perform their respective algorithm.
        if (i % 2 == 0) {
            // If it is even, number is taken as it is.
            runningSum += lastDigit;
        } else {
            // If it is odd, the algorithm is performed.
            lastDigit = lastDigit * 2;
            runningSum += lastDigit % 10;
            runningSum += lastDigit / 10;
        }
        // change lastDigit
        number = number / 10;
    }

    // Check if checksum is valid.
    if (runningSum % 10 == 0) {
        // It is valid.

        // American Express? if not, MasterCard? if not, VISA?
        if (lengthOfNumber == 15 && ( firstTwoDigits == 34 || firstTwoDigits == 37 )) {
            printf("%s\n", "AMEX");
        } else if (lengthOfNumber == 16 && firstTwoDigits > 50 && firstTwoDigits < 56) {
            printf("%s\n", "MASTERCARD");
        } else if ( (lengthOfNumber == 13 || lengthOfNumber == 16) && firstDigit == 4) {
            printf("%s\n", "VISA");
        } else {
            printf("%s\n", "INVALID");
        }
    } else {
        // It is invalid.
        printf("%s\n", "INVALID");
    }
}


