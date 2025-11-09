
//LIFO stack

public class ArrayStack<E> implements MyStack<E>{

	private E[] data;
	private int index;
	
	@SuppressWarnings("unchecked")
	public ArrayStack(int capacity) {
		data = (E[]) new Object[capacity];
	}
	
	public ArrayStack() {
		this(10);
	}
	
	@Override
	public E push(E item) {
		if(index == data.length)
			expandCapacity();
		data[index++] = item;
		return item;
		
	}

	@Override
	public E pop() {
		if(empty()) 
			return null;
		return data[--index];
	}

	@Override
	public E peek() {
		if(empty()) 
			return null;
		int temp = index - 1;
		return data[temp];
	}

	@Override
	public boolean empty() {
		return index == 0;
	}

	@Override
	public void clear() {
		index = 0;
	}
	
	@SuppressWarnings("unchecked")
	private void expandCapacity() {
		E[] temp = (E[]) new Object[data.length * 2];
		for (int i = 0; i < index; ++i) {
			temp[i] = data[i];
		}
		data = temp;
	}
	
	
	

}
