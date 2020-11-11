package philosophersProblem;

/**
 * @author cristian.chilipirea
 * 
 */
public class Philosopher implements Runnable {
	Object leftFork, rightFork;
	int id;

	public Philosopher(int id, Object leftFork, Object rightFork) {
		this.leftFork = leftFork;
		this.rightFork = rightFork;
		this.id = id;
	}

	private void sleep() {
		try {
			Thread.sleep(100);
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
	}

	@Override
	public void run() {
		//unul dintre filozofi ia intai furculita stanga, apoi pe cea dreapta
		if (id == 0) {
			synchronized (leftFork) {
				sleep(); // delay added to make sure the dead-lock is visible
				synchronized (rightFork) {
					System.out.println("Philosopher " + id + " is eating");
				}
			}
		}
		else {//toti ceilalti filozofi iau mai intai furculita dreapta, apoi pe cea stanga
			synchronized (rightFork) {
				sleep(); // delay added to make sure the dead-lock is visible
				synchronized (leftFork) {
					System.out.println("Philosopher " + id + " is eating");
				}
			}
		}
	}
}
