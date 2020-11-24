

import java.util.HashMap;
import java.lang.InterruptedException;

public class Main {

	public static HashMap<Integer, Integer> map;
	public static Object object = new Object();
	
	public static int NR_THREADS = 4;

	public static void main(String[] args) {
		int i;
		Thread[] threads = new Task[NR_THREADS];
		
		map = new HashMap<Integer, Integer>(10);
		for (i = 0; i < 10; i++) {
			map.put(i, 0);
		}
		
		
		long start = System.nanoTime();
		for (i = 0; i < NR_THREADS; i++) {
			threads[i] = new Task(i);
			threads[i].start();
		}
		
		for (i = 0; i < NR_THREADS; i++) {
			try {
				threads[i].join();
			}
			catch(InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		long end = System.nanoTime();
	    System.out.println("Execution time (ns) = " + (double)(end - start)/1000000000);
		
		//afiseaza dictionarul
		for (i = 0; i < 10; i++) {
			System.out.println((i + 1) + ", " + map.get(i));
		}
		
		
	}

}
