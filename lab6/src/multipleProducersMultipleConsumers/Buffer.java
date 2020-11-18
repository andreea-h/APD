package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	ArrayBlockingQueue<Integer> a = new ArrayBlockingQueue(5);

	void put(int value) {
		try {
			a.put(value);
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
	}

	int get() {
		Integer rez = 0;
		try {
			rez = a.take();
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
		return rez;
	}
}
