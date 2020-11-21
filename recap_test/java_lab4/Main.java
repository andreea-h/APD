public class Main {
    public static int nr_threads = 5;
    public static void main(String args[]) {
        Thread[] threads = new Thread[nr_threads]; //sau declarate ca vector cu elemente tipul Task
        int i;
        for(i = 0; i < nr_threads; i++) {
            threads[i] = new Task(i);
            threads[i].start();
        }

        for(i = 0; i < nr_threads; i++) {
            try {
                threads[i].join();
                System.out.println("Thread-ul " + i + " a calculat: " + ((Task)threads[i]).getResult());
            }
            catch(InterruptedException e) {
                e.printStackTrace();
            }
        }

    }

}