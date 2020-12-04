
//vector de dimeniune fixa thread-safe

import java.util.*;
import java.lang.IndexOutOfBoundsException;
import java.lang.RuntimeException;


public class MyConcurrentArray<T> {
	
	private static int nrInstances = 0;
	private T[] content;
	
	public MyConcurrentArray(int size) {
		if (size > 0) {
			content = (T[])new Object[size];
		}
		else {
			throw new RuntimeException("Negative size provided for MyConcurrentArray instantiation.");
		}
		
		synchronized(MyConcurrentArray.class) {
			nrInstances++;
		}
	}
	
	 public synchronized T get(int index) {
	        if (index < content.length) {
	            return content[index];
	        }
	        throw new IndexOutOfBoundsException(index + " is out of bounds for MyConcurrentArray of size " + content.length);
	 }
	 
	 
	 public void set(int index, T newT) {
	        //Bloc de cod sincronizat ce folosește instanța curentă (this) pe post de zăvor.
	       // synchronized(this) 
		 	    {
	            if (index < content.length) {
	                content[index] = newT;
	            }
	            else {
	            	throw new IndexOutOfBoundsException(index + " is out of bounds for MyConcurrentArray of size " + content.length);
	            }
	            
	        }
	 }
	 
	 public static synchronized int getNumberOfInstances(){
		 return nrInstances;
	 }
	 
	 public int size() {
		 return content.length;
	 }
	
}