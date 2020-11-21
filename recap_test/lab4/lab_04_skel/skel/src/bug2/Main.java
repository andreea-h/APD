package bug2;
/**
 * @author cristian.chilipirea
 *
 *         DO NOT MODIFY
 */
public class Main {

	public static void main(String[] args) {
		Thread thread = new Thread(new MyThread());
		thread.start();
		Thread thread1 = new Thread(new MyThread());
		thread1.start();
		try {
			thread.join();
			thread1.join();
			
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		System.out.println(MyThread.i);
	}

}
