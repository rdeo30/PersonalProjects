
public class MainADT {

	public static void main(String[] args) {
		ArraySet<Integer> set = new ArraySet<>();
		
		LinkedList<Integer> SLL = new LinkedList<>();
		
		
		ArrayList<Integer> ALS = new ArrayList<>();
		
		ArrayMap<Integer, Integer> MMP = new ArrayMap<>();
		
		ArrayStack<String> ASS = new ArrayStack<>();
		
		ArrayQueue<Integer> AQU = new ArrayQueue<>();
		
		AQU.add(190);
		AQU.add(982);
		AQU.add(823);
		AQU.add(2390);
		AQU.add(6969);
		
		System.out.println(AQU.peek());
		
		System.out.println(AQU.size());
		
		System.out.println(AQU.remove());
		
		System.out.println(AQU.peek());
		
		System.out.println(AQU.size());
		
		System.out.println(AQU.remove());
		System.out.println(AQU.remove());
		System.out.println(AQU.remove());
		
		System.out.println(AQU.size());
		System.out.println(AQU.remove());
				
		
		
	}
	
}
 