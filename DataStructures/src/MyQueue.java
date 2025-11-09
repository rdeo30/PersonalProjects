

public interface MyQueue<E>{
	
	boolean add(E element) throws IllegalStateException;
	
	E peek();
	
	E remove();
	
	int size();
	
	boolean isEmpty();
	
	

}
