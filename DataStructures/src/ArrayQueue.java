import java.util.NoSuchElementException;

//FIFO queue

public class ArrayQueue<E> implements MyQueue<E>{
		
	private E[] data;
	private int index;
	
	@SuppressWarnings("unchecked")
	public ArrayQueue(int capacity) {
		data = (E[]) new Object[capacity];
	}
	
	public ArrayQueue() {
		this(10);
	}

	@Override
	public boolean add(E element) throws IllegalStateException {
		
		if(index == data.length)
			throw new IllegalStateException("Queue is full.");
		
		data[index++] = element;
		
		return true;
	}


	@Override
	public E peek() {
		return data[0];
	}

	@Override
	public E remove() {
		if(data[0] == null) {
			throw new NoSuchElementException("Queue is empty.");
		}
		
		E toReturn = data[0];
		shiftElementsLeft();
		
		return toReturn;
	}
	
	private void shiftElementsLeft() {
		for(int i = 0; i < index - 1; i++) {
			data[i] = data[i+1];
		}
		data[index-1] = null;
		index--;
	}
	


	@Override
	public int size() {
		return index;
	}

	@Override
	public boolean isEmpty() {
		return index == 0;
	}
	

}
