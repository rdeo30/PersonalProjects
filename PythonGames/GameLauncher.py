import subprocess
import sys

def main():
    if len(sys.argv) != 2:
        print("select either wordle or connections")
        return
    choice = sys.argv[1].strip().lower()

    if choice == "connections":
        subprocess.run(["python", "ConnectionsEngine.py"])
    elif choice == "wordle":
        subprocess.run(["python", "WordleEngine.py"])
    else:
        print("Invalid. Please specify 'wordle' or 'connections'.")

if __name__ == "__main__":
    main()

