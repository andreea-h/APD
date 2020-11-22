
import java.util.concurrent.Semaphore;

class Something {
 
    static int resource = 0;
}
 
class MySemaphore extends Thread {
 
	    private int id;
	    private Semaphore sem;
	 
	    public MySemaphore(int id, Semaphore sem) {
	        this.id = id;
	        this.sem = sem;
	    }
	 
	    public static void main(String args[]) {
	        Semaphore sem = new Semaphore(1);  //un singur thread se afla in zona critica la un moment dat         
	        MySemaphore mt1 = new MySemaphore(0, sem); 
	        MySemaphore mt2 = new MySemaphore(1, sem); 
	 
	        mt1.start(); 
	        mt2.start(); 
	 
	        try { 
	            mt1.join();
	            mt2.join(); 
	        } catch (InterruptedException ex) {
	            ex.printStackTrace();
	        }
	 
	        System.out.println("count: " + Something.resource); 
	    }
	 
	    @Override
	    public void run() {
	        switch (this.id) {
	            case 0:
	                System.out.println("Starting thread " + id);
	                try {
	                    System.out.println("Thread " + id + " is waiting for a permit.");
	                    sem.acquire();
	                    System.out.println("Thread " + id + " gets a permit.");
	 
	                    for (int i = 0; i < 5; i++) {
	                        Something.resource++;
	                        System.out.println("Thread " + id + ": " + Something.resource);
	                        Thread.sleep(10);
	                    }
	                } catch (InterruptedException exc) {
	                    System.out.println(exc);
	                }
	 
	                System.out.println("Thread " + id + " releases the permit.");
	                sem.release();
	                break;
	            case 1:
	                System.out.println("Starting thread " + id);
	                try {
	                    System.out.println("Thread " + id + " is waiting for a permit.");
	                    sem.acquire();
	                    System.out.println("Thread " + id + " gets a permit.");
	 
	                    for (int i = 0; i < 5; i++) {
	                        Something.resource--;
	                        System.out.println("Thread " + id + ": " + Something.resource);
	                        Thread.sleep(10);
	                    }
	                } catch (InterruptedException exc) {
	                    System.out.println(exc);
	                }
	 
	                // Release the permit. 
	                System.out.println("Thread " + id + " releases the permit.");
	                sem.release();
	                break;
	        }
	    }
	 
	    public int getThreadId() {
	        return this.id;
	    }
	 
}	