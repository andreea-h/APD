package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;
import java.util.concurrent.Semaphore;

/**
 * @author Gabriel Gutu <gabriel.gutu at upb.ro>
 *
 */

public class Buffer {
    
    Queue<Integer> queue;
    Semaphore plin;
    Semaphore gol;

    public Buffer(int size) {
        queue = new LimitedQueue(size); //size-limita buffer-ului
        plin = new Semaphore(0);
        gol = new Semaphore(size); 
    }

	void put(int value) {
        try {
            gol.acquire();//decrementeaza gol
        } catch(InterruptedException e) {
            System.out.println(e);
        }
        synchronized(queue) {
            queue.add(value);
        }
        
        plin.release(); //incrementeaza plin  
	}

	int get() {
        int rez;
        try {
            plin.acquire();//decrementeaza plin
        } catch(InterruptedException e) {
            System.out.println(e);
        }
        synchronized(queue) {
			rez = (int)queue.remove();
        }
        gol.release();//incrementeaza gol
        return rez;
	}
}
