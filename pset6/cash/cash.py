from cs50 import get_float

def main():
    # Prompt the user for monetary amount
    change = 0
    while True:
        change = get_float("Change owed: ")
        if (change >= 0):
            break
    # Convert to cents because of floating point errors
    changeCents = int(change * 100)
    numberOfCoins = 0
    # Loop through and keep deducting cents until it is okay
    while changeCents > 0:
        changeCents -= deductCents(changeCents)
        numberOfCoins += 1
    print(numberOfCoins)

def deductCents(money):
    if money >= 25:
        return 25
    elif money >= 10:
        return 10
    elif money >= 5:
        return 5
    else:
        return 1
main()