
public interface MySet<E> extends Iterable<E>{
	
	boolean isEmpty();
	
	boolean add(E item);
	
	boolean contains(E item);
	
	boolean remove(E item);

	
}
