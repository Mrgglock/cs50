from cs50 import get_int

def main():
    height = 0;
    # Prompt user for height of pyramid.
    while True:
        height = get_int("Height: ")
        if (height > 0 and height <= 8):
            break
    # Loop through every layer number, 1, 2, 3 up to `height`.
    for i in range(1, height + 1, 1):
       layer(i, height)

def layer(current, maximum):
    spaces = (maximum - current)
    # Print the layer, spaces, blocks, 2-gap, blocks
    print(" " * spaces + "#" * current + "  " + "#" * current)

main()