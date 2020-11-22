
import java.lang.InterruptedException;

public class Main {

	public static int NR_THREADS = 4;
	public static int a = 2;
	public static Object lock; //obiectul pe care fac lock
	
	public static Inc inc = new Inc();
	
	public static void main(String[] args) {
		//Task[] threads = new Task[NR_THREADS];
		//sau 
		Thread[] threads = new Thread[NR_THREADS];
		
		lock = new Object();
		int i;
		for (i = 0; i < NR_THREADS; i++) {
			threads[i] = new Task(i, inc);
			
			threads[i].start();
		}
		
		for (i = 0; i < NR_THREADS; i++) {
			try {
				threads[i].join();
				//System.out.println("Thread-ul a intors rezultatul " + ((Task)threads[i]).getResult());
				
			}
			catch(InterruptedException e) {
				e.printStackTrace();
			}
		}
		System.out.println("Valoare lui a este " + Inc.a);
	
	}

}
