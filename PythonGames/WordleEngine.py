
import nltk
import os

parentDirectory = os.path.dirname(os.path.abspath(__file__))
nltk.data.path.append(parentDirectory)

try:
        nltk.data.find('corpora/words.zip')
except LookupError:
        nltk.download('words', download_dir=parentDirectory)

from nltk.corpus import words
wordList = set(words.words())
wordList = [word.lower() for word in wordList]

def verifyWord(word):
        return word in wordList


def wordle(cpu, user):
        parse = ""
        lc = 0
        for i in range(5):
                if user[i] == cpu[i]:
                        parse += "\u001B[42m" + user[i] + "\x1b[0m"
                        lc += 1
                        cpu = cpu.replace(cpu[i], ';', 1)
                elif cpu.__contains__(user[i]):
                         ao = False
                         for j in range(i+1, 5):
                                 if user[i] == user[j]:
                                         ao = not ao
                         if ao:
                                 parse += user[i]
                         else:
                                 parse += "\u001B[43m" + user[i] + "\x1b[0m"
                else:
                        parse += user[i]

        if lc == 5:
                print(parse)
                print("Correct!")
                exit()
        return parse

cpuWord = "radio"

lives = 5

while lives >= 1:
        cc = False
        userIn = ""
        while not cc:
                userIn = input("Enter your five letter word." + " You have " + str(lives) + " lives remaining")
                userIn = str(userIn.lower())
                if len(userIn) == 5:
                        if verifyWord(userIn):
                                cc = True
                        else:
                                print("Enter a valid word")
                else:
                        print("Word must be 5 letters")
        print(wordle(cpuWord,userIn))
        lives-=1
print("\u001B[31m Game Over. No lives left.\x1b[0m")
exit()





