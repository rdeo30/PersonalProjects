
public class ArrayMap<K, V> implements MyMap<K, V> {
	
	private KVPair<K,V>[] table;
	private int size;
	
	@SuppressWarnings("unchecked")
	public ArrayMap(int capacity) {
		table = (KVPair<K,V>[]) new KVPair[capacity];
	}
	
	public ArrayMap() {
		this(10);
	}
	

	@Override
	public V get(K key) {
		for(int i = 0; i < size; i++) {
			if(key.equals(table[i].key)) {
				return table[i].value;
			}
		}
		return null;
	}

	@Override
	public V remove(K key) {
		for(int i = 0; i < size; i++) {
			if(table[i].key.equals(key)) {
				V value = table[i].value;
				table[i] = table[--size];
				table[size] = null;
				return value;
			}
		}
		return null;
	}

	@Override
	public boolean containsKey(K key) {
		
		for(int i = 0; i < size; i++) {
			if(table[i].key.equals(key))
				return true;
		}
		
		return false;
		
	}
	

	@Override
	public boolean containsValue(V value) {
		for(int i = 0; i < size; i++) {
			if(value.equals(table[i].value)) 
				return true;
		}
		
		return false;
	}
	
	@Override
	public V put(K key, V value) {
		
		if(this.containsKey(key)) {
			for(int i = 0; i < size; i++) {
				if(key.equals(table[i].key)) {
					table[i].value = value;
					return value;
				}
			}
		}
		
		if(size == table.length)
			expandCapacity();
		
		KVPair<K,V> toAdd = new KVPair<>(key, value);
		table[size++] = toAdd;
		
		return value;
	}
	
	@SuppressWarnings("unchecked")
	private void expandCapacity() {
		KVPair<K,V>[] temp = (KVPair<K,V>[]) new KVPair[table.length*2];
		for(int i = 0; i < size; i++) {
			temp[i] = table[i];
		}
		table = temp;
	}

	
	
	@Override
	public boolean isEmpty() {
		return size == 0;
	}
	
	
	@SuppressWarnings("hiding")
	private class KVPair<K, V>{
		K key;
		V value;
		
		KVPair(K key, V value){
			this.key = key;
			this.value = value;
		}
		
		
	}


	
	
	
	
	
	
	
}
