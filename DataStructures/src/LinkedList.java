import java.util.Iterator;
import java.util.NoSuchElementException;

public class LinkedList<E> implements MyList<E>{

	private Node<E> head;
	private Node<E> tail;
	private int size;

	public LinkedList() {
		head = null;
		tail = null;
		size = 0;
	}

	public void add(Node<E> e) {
		
		if(e == null) 
			throw new IllegalArgumentException("Node to insert cannot be null.");
		
		if(size == 0) {
			head = e;
			tail = head;
		} else {
			tail.setNext(e);
			tail = e;
		}

		size++;
	}

	public Node<E> getNext(Node<E> e) {
		return e.getNext();
	}

	public void remove(Node<E> e) {
		Node<E> current = head;

		if(current == e) {
			head = head.getNext();
			size--;
			if(size == 0) {
				tail = null;
			}
		}

		while (current != null) {
			if(current.getNext() == e) {
				current.setNext( current.getNext().getNext());
				if(current.getNext() == null) {
					tail = current;
				}
				size--;
			} 
			current = current.getNext();
		}

	}

	@Override
	public Iterator<E> iterator() {
		return new LinkedListIterator();
	}

	private class LinkedListIterator implements Iterator<E> {
		Node<E> current = head;

		@Override
		public boolean hasNext() {
			return current != null;
		}

		@Override
		public E next() {
			if(!hasNext()) {
				throw new NoSuchElementException();
			}
			E data = current.getData();
			current = current.getNext();
			return data;
		}


	}

	@Override
	public boolean isEmpty() {
		return head == null;
	}

	@Override
	public boolean add(E item) {
		if(item == null) 
			return false;

		Node<E> node = new Node<E>(item);
		this.add(node);
		return true;

	}

	@Override
	public boolean contains(E item) {
		for (E itm : this) {
			if(itm == item) {
				return true;
			}
		}

		return false;
	}

	@Override
	public boolean remove(E item) {

		boolean found = false;
		
		Node<E> current = this.head;

		while(current != null) {
			if(current.getData() == item) {
				remove(current);
				found = true;
			}
			current = current.getNext();
		}

		return found;
	}

	@Override
	public int size() {
		return size;
	}




}
