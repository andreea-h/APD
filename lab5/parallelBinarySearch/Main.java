package parallelBinarySearch;

import java.util.concurrent.CyclicBarrier;
import java.util.*;

public class Main {
    public static int N = 100; //numarul de elemente al vectorului
    public static int P = 4; // numarul de threaduri
    public static int x = 9710; //valoarea cautata
    public static boolean found;
    public static int pos;

    public static int i0, in; //definesc zona de interes pt cautare

    public static CyclicBarrier barrier;

    public static int[] v;
    public static void main(String[] args) {
        Thread[] threads = new ParallelBinarySearch[P];
        i0 = 0;
        in = N - 1;//initial zona este [0,N)

        barrier = new CyclicBarrier(P);

        v = new int[N];
        int i;
        for (i = 0; i < N; i++) {
            v[i] = 10 + i * (i + 3);
        }

        int start, end;
        for (i = 0; i < P; i++) {
            //fiecare thread are un 'segment' de N/P elemente
            start = (int)(i * (double) Main.N / Main.P); 
            end = (int)Math.min((i + 1) * (double)(Main.N)/ Main.P, Main.N);
            threads[i] = new ParallelBinarySearch(i);
        }
        
        for (i = 0; i < P; i++) {
            threads[i].start();
        }

        for (i = 0; i < P; i++) {
            try {
                threads[i].join();
            }
            catch(InterruptedException e) {
                System.out.println(e);
            }
        }

        System.out.println(Arrays.toString(v));
        System.out.println("x = " + x);
        if (found == false) {
            System.out.println(x + " nu se gaseste in v");
        }
        else {
            System.out.println("pozitia este " + pos);
        }
        
    }
}