import random
import ConnectionGroups
from ConnectionGroups import yellowSet
from ConnectionGroups import greenSet
from ConnectionGroups import blueSet
from ConnectionGroups import purpleSet

blueWords = blueSet()
purpleWords = purpleSet()
yellowWords = yellowSet()
greenWords = greenSet()

blueWords1 = blueWords[:4] #extract only the words
purpleWords1 = purpleWords[:4]
yellowWords1 = yellowWords[:4]
greenWords1 = greenWords[:4]


genNum = 16
allWords = blueWords1 + purpleWords1 + yellowWords1 + greenWords1

random.shuffle(allWords)

matrix = [allWords[i:i+4] for i in range(0, genNum, 4)]

def generateNewMatrix():
    global matrix
    random.shuffle(allWords)
    remaingWords = [allWords[i:i+4] for i in range(0, genNum, 4)]
    matrix = [blueWords1]*bg + [purpleWords1]*pg + [yellowWords1]*yg + [greenWords1]*gg + remaingWords


bg = 0
pg = 0
gg = 0
yg = 0
lives = 4
while lives > 0:
    alreadyCaught = False
    for row in matrix:
        print(row)
    guess = input("Enter your guess. You have " + str(lives) + " lives left." + " Enter words in a comma separated list")
    words = [word.strip() for word in guess.split(",")]
    o = 0
    af = 0
    ad = 0
    d = 0
    deduct = True
    if bg == 0: # bluewords
       try:
           for i in range(4):
               if words[i] in blueWords:
                   o += 1
               if o == 4:
                   print("CORRECT: \u001B[34m" + blueWords[4] + "\x1b[0m")
                   bg = 1
                   genNum -= 4
                   allWords = [item for item in allWords if item not in blueWords]  # removes words from pool
                   generateNewMatrix()
                   deduct = False
               elif o == 3 and i == 3:
                   print("One Away!")
       except IndexError:
           if alreadyCaught == False:
               print("\u001B[31mEnter a valid input\x1b[0m")
               alreadyCaught = True
           deduct = False

    if pg == 0: # purpleWords
        try:
            for i in range(4):
                if words[i] in purpleWords:
                    af += 1
                if af == 4:
                    print("CORRECT: \u001B[35m" + purpleWords[4] + "\x1b[0m")
                    pg = 1
                    genNum -= 4
                    allWords = [item for item in allWords if item not in purpleWords]
                    generateNewMatrix()
                    deduct = False
                elif af == 3 and i == 3:
                    print("One Away!")
        except IndexError:
            if alreadyCaught == False:
                print("\u001B[31mEnter a valid input\x1b[0m")
                alreadyCaught = True
            deduct = False


    if gg == 0:  # greenWords
        try:
            for i in range(4):
                if words[i] in greenWords:
                    ad += 1
                if ad == 4:
                    print("CORRECT: \u001B[32m" + greenWords[4] + "\x1b[0m")
                    gg = 1
                    genNum -= 4
                    allWords = [item for item in allWords if item not in greenWords]
                    generateNewMatrix()
                    deduct = False
                elif ad == 3 and i == 3:
                    print("One Away!")
        except IndexError:
            if alreadyCaught == False:
                print("\u001B[31mEnter a valid input\x1b[0m")
                alreadyCaught = True
            deduct = False

    if yg == 0:  # yellowWords
        try:
            for i in range(4):
                if words[i] in yellowWords:
                    d += 1
                if d == 4:
                    print("CORRECT: \u001B[33m" + yellowWords[4] + "\x1b[0m")
                    yg = 1
                    genNum -= 4
                    allWords = [item for item in allWords if item not in yellowWords]
                    generateNewMatrix()
                    deduct = False
                elif d == 3 and i == 3:
                    print("One Away!")
        except IndexError:
            if alreadyCaught == False:
                print("\u001B[31mEnter a valid input\x1b[0m")
                alreadyCaught = True
            deduct = False

    if deduct:
        lives -=1

if lives == 0:
    print("You ran out of lives!\n")
    print(purpleWords)
    print(blueWords)
    print(greenWords)
    print(yellowWords)
