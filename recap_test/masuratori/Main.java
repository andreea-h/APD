public class Main {
    private final static int P = 8;

    public static void main(String[] args) {
        Thread[] threads = new Thread[P];

        /*
           un mod indicat de a masura scalabilitatea unui program este
           sa masuram timpul de executie al functiei (sau al functiilor)
           executata / executate de catre thread-uri
           este de recomandat sa punem un timestamp fix inainte de crearea si
           de lansarea thread-urilor (variabila start de mai jos), care sa fie unul de start,
           si inca un timestamp imediat dupa join-ul thread-urilor (un timestamp de stop - variabila end)
           si sa facem diferenta dintre cele doua timestamps (end - start)
         */

        // timestamp de start
        long start = System.nanoTime();
        for (int i = 0; i < P; i++) {
            threads[i] = new MyThread(i);
            threads[i].start();
        }

        for (int i = 0; i < P; i++) {
            try {
                threads[i].join();
                /*
                * facem downcast de la Thread (caci avem array de obiecte Thread
                * la MyThread (fiecare obiect e instantiat ca MyThread)
                * ca sa putem apela metoda getThread(), pentru a putea
                * accesa rezultatul produs in metoda run() de catre thread
                */
                System.out.println("Result = " + ((MyThread)threads[i]).getThreadId());
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        // timestamp de stop
        long end = System.nanoTime();
        System.out.println("Execution time (ns) = " + (end - start));
    }
}

class MyThread extends Thread {
    private int id;

    public MyThread(int id) {
        this.id = id;
    }

    public int getThreadId() {
        // intoarcem rezultatul operatiei din run()
        return id;
    }

    @Override
    public void run() {
        // facem o operatie
        id++;
    }
}