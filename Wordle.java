import java.util.Scanner;

public class Wordle {
	
	public static void main(String[] args) {
		String  cpuWORD = "poppy";
		
		Scanner keyboard = new Scanner(System.in);
		final String GREEN = "\u001B[42m";
		final String YELLOW = "\u001B[43m";
		int lives = 6;

		while(lives > 0) {
			System.out.println("Enter word. Lives remaining: " + lives);
			String userInput = keyboard.next().toLowerCase();
			System.out.println(wordle(cpuWORD, userInput));
			lives--;
		}
		
		System.out.println("game over!");
		System.exit(0);
		keyboard.close();
	}

	public static synchronized String wordle(String cpu, String user) {
		String parse = "";
		char ac = findRepeat(cpu); //allowedCharacter
		char[] u = user.toCharArray();
		int lc = 0; //local
		for(int i = 0; i < cpu.length(); i++) {
			if(cpu.charAt(i) == u[i]) {
				parse += "\u001B[42m" + u[i] + "\u001B[0m";
				lc++;
				cpu = cpu.replaceFirst(Character.toString(cpu.charAt(i)), Character.toString('K'));
			}
			
			else if(cpu.contains(Character.toString(u[i]))) {
				boolean anotherOne = false; //local
				for(int j = i + 1; j < u.length; j++) {
					if(u[j] == u[i]) {
						anotherOne = true;
						break;
					}
				}
				
				if(anotherOne && u[i] != ac) parse += u[i] ; else parse += "\u001B[43m" + u[i] + "\u001B[0m";		
			}
			
			else {
				parse += u[i];
			}
		}
		if(lc == 5) {
			System.out.println(parse);
			System.out.println("COMPLETE");
			System.exit(0);
		}
		return parse;

	}
	
	private static char findRepeat(String cpu) {
		char[] c = cpu.toCharArray();
		for(int i = 0; i < c.length; i++) {
			for(int j = i + 1; j < cpu.length();j++) {
				if(c[i] == c[j]) return c[i];
			}
		}
		
		return 'L';
	}
	

	


}
