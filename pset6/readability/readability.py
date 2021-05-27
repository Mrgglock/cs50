from cs50 import get_string

def main():
    text = get_string("Text: ")
    charCount = 0
    # The last word does not have a space after it.
    wordCount = 1
    sentenceCount = 0
    for character in text:
        if character.isalpha():
            charCount += 1
        elif character == " ":
            wordCount += 1
        elif character in [".", "!", "?"]:
            sentenceCount += 1
    L = charCount / wordCount * 100
    S = sentenceCount / wordCount * 100
    index = 0.0588 * L - 0.296 * S - 15.8
    print(output(index))
    
def output(index):
    rounded = round(index)
    if rounded < 1:
        return "Before Grade 1"
    elif rounded >= 16:
        return "Grade 16+"
    else:
        return f"Grade {rounded}"
    
main()