package oneProducerOneConsumer;
/**
 * @author cristian.chilipirea
 *
 */


import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.Semaphore;

public class Buffer {
    
    Queue<Integer> queue;
    Semaphore plin;
    Semaphore gol;

    public Buffer() {
        queue = new LinkedList();
        plin = new Semaphore(0);
        gol = new Semaphore(10);
    }

	void put(int value) {
        try {
            gol.acquire();
        } catch(InterruptedException e) {
            System.out.println(e);
        }
        synchronized(queue) {
            queue.add(value);
        }
        
        plin.release();       
	}

	int get() {
        int rez;
        try {
            plin.acquire();
        } catch(InterruptedException e) {
            System.out.println(e);
        }
        synchronized(queue) {
			rez = (int)queue.remove();
        }
        gol.release();
        return rez;
	}
}
