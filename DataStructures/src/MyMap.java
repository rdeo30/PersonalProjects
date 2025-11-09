
public interface MyMap<K, V> {
	
	V get(K key);
	
	V remove(K key);
	
	boolean containsKey(K key);
	
	boolean containsValue(V value);
	
	V put(K key, V value);
	
	boolean isEmpty();
	
	
	
	

}
