package bug3;
/**
 * @author cristian.chilipirea
 * 
 *         Why is value set correct even though we use different locks in
 *         different threads?
 */
public class MyThread implements Runnable {
	static String a = "LOCK1"; //daca a si b au acceesi valoare, atunci au aceeasi referinta in spate
	static String b = "LOCK";
	int id;
	static int value = 0;

	MyThread(int id) {
		this.id = id;
	}

	@Override
	public void run() {
		if (id == 0) { //a si b sunt de fapt aceeasi instanta, deci sincronizarea se face pe aceeasi instanta, de aceea este corect rezultatul
			//a si b sunt aceleasi stringuri, deci sincronizarea functioneaza
			synchronized (a) {
				for (int i = 0; i < Main.N; i++)
					value = value + 3;
			}
		} else {
			synchronized (b) {
				for (int i = 0; i < Main.N; i++)
					value = value + 3;
			}
		}
	}
}