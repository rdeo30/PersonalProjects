
public interface MyStack<E> {
	
	E push(E item);
	
	E pop();
	
	E peek();
	
	boolean empty();
	
	void clear();

}
