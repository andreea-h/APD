
import java.lang.InterruptedException;

public class Main {

	public static int NR_THREADS = 8;
	public static MyConcurrentArray<Integer> t;
	
	public static void main(String[] args) {
		t = new MyConcurrentArray<Integer>(8);
		
		Thread[] threads = new Thread[NR_THREADS];
		int i;
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
			};
		}	
		
		for (i = 0; i < NR_THREADS; i++) {
			System.out.println(t.get(i));
		}
	}

}
