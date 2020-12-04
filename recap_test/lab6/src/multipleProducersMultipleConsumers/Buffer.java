package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	ArrayBlockingQueue q;
	
	public Buffer() {
		q = new ArrayBlockingQueue<Integer>(15);
	}
	
	void put(int value) {
		try {
			q.put(value);
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
	}

	int get() {
		int rez = 0;
		try {
			rez = (int)q.take();
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
		return rez;
	}
}