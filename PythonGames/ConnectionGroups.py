import random

#Module containing the sets for connections game. Provides methods to extract a set and its categories

set1 = [
    ["affiliate", "associate", "equate", "relate", "THINK OF TOGETHER"],
    ["bait", "chum", "fly", "lure", "WAYS TO ATTRACT FISH"],
    ["liberty", "storm", "sun", "wings", "W.N.B.A TEAMS"],
    ["bow", "fellow", "horn", "legs", "LONG__"]
]

set2 = [
    ["flair", "gift", "instinct", "nose", "APTITUDE"],
    ["face", "front", "outside", "surface", "EXTERIOR"],
    ["amazon", "apple", "intuit", "oracle", "TECH COMPANIES"],
    ["brim", "lime", "rhine", "yellow", "__STONE"]
]

set3 = [
    ["carmela", "junior", "meadow", "tony", "SOPRANOS"],
    ["cuz", "grammy", "mummy", "pop", "FAMILIAL NICKNAMES"],
    ["cookie", "count", "oscar", "snuffy", "\"SESAME STREET\" CHARACTERS"],
    ["cece", "edie", "emmy", "katie", "NAMES THAT SOUND LIKE TWO LETTERS"]
]

set4 = [
    ["haven", "port", "retreat", "shelter", "SANCTUARY"],
    ["cleats", "helmet", "jersey", "pads", "FOOTBALL GEAR"],
    ["charms", "kinder", "wonka", "york", "CANDY NAMES"],
    ["bird", "fallin'", "solo", "willy", "FREE__"]
]

set5 = [
    ["jazz", "modern", "swing", "tap", "DANCE STYLES"],
    ["lift", "palm", "pocket", "swipe", "STEAL"],
    ["ace", "crush", "nail", "rock", "PERFORM WELL ON"],
    ["curve", "fast", "knuckle", "screw", "__BALL PITCHES"]
]

set6 = [
    ["perplex", "puzzle", "stump", "vex", "BAFFLE"],
    ["spell","pox","jinx","hex", "CURSE"],
    ["buzz","hamm","rexx","slinky", "“TOY STORY” CHARACTERS, FAMILIARLY"],
    ["core", "gate", "mania", "pilled", "COLLOQUIAL SUFFIXES"]
]

set7 = [
    ["singer", "violin", "tenor", "composer", "MUSICAL ROLES"],
    ["spade", "sword", "club", "heart", "CARD SUITS"],
    ["breeze", "wind", "storm", "cyclone", "WEATHER PHENOMENA"],
    ["president", "governor", "senator", "mayor", "GOVERNMENT OFFICIALS"]
]

set8 = [
    ["plumber", "electrician", "mechanic", "carpenter", "TRADES"],
    ["curry", "sushi", "paella", "tacos", "FOOD CUISINES"],
    ["mercury", "uranus", "neptune", "venus", "PLANETS"],
    ["proton", "neutron", "electron", "boson", "PARTICLES"]
]

sets = {
    "set1": set1,
    "set2": set2,
    "set3": set3,
    "set4": set4,
    "set5": set5,
    "set6": set6,
    "set7": set7,
    "set8": set8
}

def select_random_set():
    set_name = random.choice(list(sets.keys()))
    return sets[set_name]

pickedSET = select_random_set()

def yellowSet():
   return pickedSET[0]

def greenSet():
   return pickedSET[1]

def blueSet():
   return pickedSET[2]

def purpleSet():
   return pickedSET[3]