package bug2;
/**
 * @author cristian.chilipirea
 * 
 *         Why does this code not block? We took the same lock twice!
 */
public class MyThread implements Runnable {
	static int i;

	@Override
	public void run() { //acelasi thread poate lua monitorul de 2 ori
		synchronized (this) { //pune monitor lock-ul pe instanta clasei MyThread pe care apelez functia run()
			//adica pe obiectul thread din Main
			synchronized (this) //instanta curenta de clasa deja devine monitor lock-ul
			{
				i++;
				System.out.println(i);
			}
		}
	}
}
