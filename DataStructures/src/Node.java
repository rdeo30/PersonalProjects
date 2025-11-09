

public class Node<E> {
	private E element;
	private Node<E> next;
	
	public Node(E e) {
		element = e;
	}
	
	
	public String toString() {
		return element.toString();
	}
	
	public Node<E> getNext() {
		return next;
		
	}
	
	public void setNext(Node<E> next) {
		this.next = next;
		
	}
	
	public E getData() {
		return element;
		
	}



}
