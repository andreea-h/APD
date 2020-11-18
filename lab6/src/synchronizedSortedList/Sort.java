package synchronizedSortedList;

import java.util.Collections;
import java.util.List;
import java.util.concurrent.Semaphore;

public class Sort extends Thread {
    private final List<Integer> list;

    public Sort(List<Integer> list) {
        super();
        this.list = list;
    }

    @Override
    public void run() {
        try {
            Main.s.acquire(3);
        }
        catch(InterruptedException e) {
            e.printStackTrace();
        }
        Collections.sort(list);
    }
}
