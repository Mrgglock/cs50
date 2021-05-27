import csv
import sys

def main():
    # Check if the usage is correct
    # [dna.py, data.csv sequence.txt]
    if (len(sys.argv) != 3):
        sys.exit("Usage: python dna.py data.csv sequence.txt")

    # Read into the database
    database = []
    listOfSTRs = []
    with open(sys.argv[1]) as csvfile:
        reader = csv.DictReader(csvfile)
        # Go through each person and read into database
        
        for row in reader:
            # Turn the numbers into ints for each of the STRs
            database.append(intifySTRs(row))
        
        # Find the list of STRs to keep an eye out for
        headers = database[0]
        for key in headers:
            if (key != "name"):
                listOfSTRs.append(key)
    
    # Go through the persons profile and start searching for STRs. 
    # Build a profile off of this.
    profile = {}
    with open(sys.argv[2]) as f:
        dna = f.read()
        # For every STR sequence, find the highscore
        for STR in listOfSTRs:
            profile[STR] = longestChain(dna, STR)
    
    # Compare this profile against the database
    # For each possible model person
    unknown = True
    for model in database:
        # Check if their STR match with my profile
        STRsMatch = True
        for STR in listOfSTRs:
            if model[STR] != profile[STR]:
                STRsMatch = False
                break
        # If the STRs match, this model is indeed my person
        if (STRsMatch):
            unknown = False
            print(model["name"])
            break
    # None of the STRs match
    if (unknown):
        print("No Match")
            

def intifySTRs(person):
    for key in person:
        if (key != "name"):
            person[key] = int(person[key])
    return person
    
def longestChain(dna, STR):
    # Loop through every pos of dna and search it with the helper function, searching for the highest number.
    highscore = 0
    for i in range(len(dna)):
        score = search(dna, STR, i)
        if score > highscore:
            highscore = score
    return highscore

def search(dna, STR, pos):
    # find the longest chain of STRs at pos n given the dna.
    # Set boundaries for the STR
    pointer = pos
    endPointer = pos + len(STR)
    chainCounter = 0
    # investigate chunk by chunk
    while True:
        chunk = dna[pointer:endPointer]
        if chunk == STR:
            chainCounter += 1
            pointer += len(STR)
            endPointer += len(STR)
        else:
            return chainCounter
            
            

main()