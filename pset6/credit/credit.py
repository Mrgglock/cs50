from cs50 import get_int

def main():
    # Prompt user for credit card number
    number = get_int("Number: ")
    # Convert to a string to do some analysis
    string = str(number)
    length = len(string)
    firstTwoDigits = int(string[0:2])
    # Loop through every number from the back
    runningSum = 0
    isSpecial = False
    while number > 0:
        lastDigit = number % 10
        # Check for special algorithm, then flip the logic for the next number, and reduce that number.
        if (isSpecial):
            runningSum += calcSpecial(lastDigit)
        else:
            runningSum += lastDigit
        isSpecial = not isSpecial
        number = (number - lastDigit) / 10

    print(typeOfCard(runningSum, firstTwoDigits, length))

def calcSpecial(digit):
    digit *= 2
    if (digit >= 10):
        firstDigit = (digit - digit % 10) / 10
        secondDigit = digit % 10
        return firstDigit + secondDigit
    else:
        return digit

def typeOfCard(checksum, firstTwo, length):
    # If the checksum is wrong, it is not a card
    if (checksum % 10 != 0):
        return "INVALID"
    # If it is 15-digit 34xxx or 37xxx, it is AMEX
    if (length == 15 and (firstTwo == 34 or firstTwo == 37)):
        return "AMEX"
    # If it is 16-digit 51xx 52xx 53xx 54xx 55xx, it is MasterCard
    if (length == 16 and firstTwo >= 51 and firstTwo <= 55):
        return "MASTERCARD"
    # If it is 13 or 16 digit 4xxxxxxx it is VISA
    if ((length == 13 or length == 16) and firstTwo >= 40 and firstTwo <= 49):
        return "VISA"
    # If all those checks failed, no cardtype exists for this.
    return "INVALID"

main()