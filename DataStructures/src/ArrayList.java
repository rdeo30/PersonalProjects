import java.util.Iterator;
import java.util.NoSuchElementException;

public class ArrayList<E> implements MyList<E> {
	
	private E[] data;
	private int size;

	@SuppressWarnings("unchecked")
	public ArrayList(int initialSize) {
		data = (E[]) new Object[initialSize];
		size = 0;
	}
	
	public ArrayList() {
		this(10);
	}
	

	@Override
	public boolean add(E element) {
		if(size == data.length) {
			expandCapacity();
		}
		data[size++] = element;
		return true;
	}
	
	public void add(E element, int index) {
		
		if(index < 0 || index > size) 
			throw new IllegalArgumentException("Invalid index");
		
		shiftElementsRight(index);
		data[index] = element;
		
	}
	
	@SuppressWarnings("unchecked")
	private void expandCapacity() {
		E[] temp = (E[]) new Object[data.length * 2];
		for (int i = 0; i < size; ++i) {
			temp[i] = data[i];
		}
		data = temp;
	}

	@Override
	public boolean remove(E element) {
		boolean found = false;
		for(int i = 0; i < size; i++) {
			if(data[i] == element) {
				shiftElementsLeft(i);
				found = true;
			}
		}
		
		return found;
	}
	
	private void shiftElementsLeft(int index) {
		for(int i = index; i < size - 1; i++) {
			data[i] = data[i+1];
		}
		data[size-1] = null;
		size--;
	}
	
	private void shiftElementsRight(int index) {
	    for (int i = size - 1; i >= index; i--) {
	        data[i + 1] = data[i];
	    }
	    data[index] = null;
	    size++;
	}

	@Override
	public boolean contains(E element) {
		for(int i = 0; i < size; i++) {
			if(data[i] == element)
				return true;
		}
		return false;
	}

	@Override
	public boolean isEmpty() {
		return size == 0;
	}

	@Override
	public int size() {
		return size;
	}
	
	public E get(int index) {
		
		if(index < 0 || index >= size) 
			throw new IllegalArgumentException("Invalid index");
		
		return data[index];
		
		
	}
	
	
	
	@Override
	public Iterator<E> iterator() {
		return new ArrayListIterator();
	}
	
	private class ArrayListIterator implements Iterator<E> {
		private int position = 0;
		@Override
		public boolean hasNext() {
			return position < size; 
		}

		@Override
		public E next() {
			if(!hasNext()) {
				throw new NoSuchElementException();
			}
			
			return data[position++];
		}
		
	}

	
	

}
