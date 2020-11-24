package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.Semaphore;

/**
 * @author Gabriel Gutu <gabriel.gutu at upb.ro>
 *
 */
public class Buffer {
    /*
	ArrayBlockingQueue queue;
    
    public Buffer(int size) {
        queue = new ArrayBlockingQueue(size);
    }

	void put(int value) {
		try {
			queue.put(value); 
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
               
	}

	int get() {
		int rez = 0;
		try {
			rez = (int)queue.take();
		}
        catch(InterruptedException e) {
        	e.printStackTrace();
        }
        return rez;
	}
	
	*/
	
	Queue queue;
	Semaphore plin;
	Semaphore gol;
    
    public Buffer(int size) {
        queue = new LimitedQueue(size);
        plin = new Semaphore(0);
        gol = new Semaphore(size);
    }

	void put(int value) {
		try {
			gol.acquire();
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
		
		synchronized(queue) {
			queue.add(value); 
		}
		
		plin.release();
	}

	int get() {
		try {
			plin.acquire();
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
		int rez = 0;
		synchronized(queue) {
			rez = (int)queue.remove();
		}
		
		gol.release();
		return rez;
	}
}






