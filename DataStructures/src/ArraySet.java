import java.util.Iterator;
import java.util.NoSuchElementException;

public class ArraySet<E> implements MySet<E>{

	private E[] data;
	private int size;
	
	public ArraySet() {
		this(5);
		
	}
	
	@SuppressWarnings("unchecked")
	public ArraySet(int initialCapacity) {
		data = (E[]) new Object[initialCapacity];
		size = 0;
	}
	
	@Override
	public boolean isEmpty() {
		return size == 0;
	}

	@Override
	public boolean add(E item) {
		if (contains(item)) return false;
		if (size == data.length) expandCapacity();
		data[size++] = item;
		return true;
	}

	private void expandCapacity() {
		E[] temp = (E[]) new Object[data.length * 2];
		for (int i = 0; i < size; ++i) {
			temp[i] = data[i];
		}
		data = temp;
	}

	@Override
	public boolean contains(E item) {
		for(int i = 0; i < size; i++) {
			if (data[i].equals(item)) {
				return true;
			}
		}
		
		return false;
	}

	@Override
	public boolean remove(E item) {
		for(int i = 0; i < size; i++) {
			if(data[i].equals(item)) {
				data[i] = data[--size];
				data[size] = null;
				return true;
			}
		}
		return false;
	}
	
	@Override
	public String toString() {
		String result = String.format("set size/capacity: %d/%d | <", size,data.length);
		for(int i = 0; i < size; i++) {
			result += data[i];
			if(i < size- 1 ) {
				result += " ";
			}
		}
		result += ">";
		return result;
	}

	@Override
	public Iterator<E> iterator() {
		return new ArraySetIterator(); 
	}
	
	private class ArraySetIterator implements Iterator<E> {
		private int position = 0; //eager initialization
		@Override
		public boolean hasNext() {
			return position < size;
			
		}

		@Override
		public E next() {
			if (!hasNext()) 
				throw new NoSuchElementException("There are no more elements in the set!");
			return data[position++];
		}
		
	}
	
	
	

}
