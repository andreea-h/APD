public class Main {
 
    public static void main(String[] args) {
        int NUMBER_OF_THREADS = Runtime.getRuntime().availableProcessors();
        Task[] t = new Task[NUMBER_OF_THREADS];
 
        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            t[i] = new Task(i);
            t[i].start();
        }
 
        for (int i = 0; i < NUMBER_OF_THREADS; ++i) {
            try {
                t[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}