

import java.lang.InterruptedException;
import java.util.concurrent.CyclicBarrier;

public class Main {

	public static int[] v;
	public static int N = 10;
	public static int NR_THREADS = Runtime.getRuntime().availableProcessors();
	public static int suma;
	
	public static int[] partial_sum = new int[NR_THREADS];
	public static CyclicBarrier barrier;
	
	
	public static void main(String[] args) {
		int i;
		Thread[] threads = new Task[NR_THREADS];
		barrier = new CyclicBarrier(NR_THREADS);
		v = new int[N];
		
		for (i = 0; i < N; i++) {
			v[i] = 10;
			System.out.print(v[i] + " ");
		}
		System.out.println("");
		
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
        System.out.println("Execution time (ns) = " + (end - start));
		
	}

}
