package parallelBinarySearch;

import java.util.*;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;

public class ParallelBinarySearch extends Thread {

    int start, end;
    int id;

    public ParallelBinarySearch(int id) {
        this.id = id;
        start = (int)(id * (double)Main.N / Main.P);
        end = (int) (Math.min((id + 1) * (double)Main.N / Main.P, Main.N));
    }

    @Override
    public void run() {
        while ((Main.i0 < Main.in - 1) && !Main.found) {
            //daca elementul cautat este in afara intervalului de valori al lui v
            if (Main.x < Main.v[0] || Main.x > Main.v[Main.N - 1]) {
                return;
            }
            try {
                Main.barrier.await();
            } catch (BrokenBarrierException | InterruptedException e) {
                e.printStackTrace();
            }

            //s-a identificat elementul in zona aferenta threadului curent
            //el cautat in v[start], v[end - 1]
            if (Main.x > Main.v[start] && Main.x < Main.v[end - 1]) {
                //modifica aria de interes (i0, in)
                Main.i0 = start;
                Main.in = end - 1;
            }
            if (Main.x == Main.v[start]) {
                Main.found = true;
                Main.pos = start;
            }
            if (Main.x == Main.v[end - 1]) {
                Main.found = true;
                Main.pos = end - 1;
            }
            
            try {
                Main.barrier.await();
            } catch (BrokenBarrierException | InterruptedException e) {
                e.printStackTrace();
            }
           
            //recalculeaza domeniul aferent threadului in functie de noul interval de interes [i0, in]
            start = (int)(Main.i0 + id * (double)(Main.in - Main.i0) / Main.P);
            end = Main.i0 + (int)Math.min((id + 1) * (double)(Main.in - Main.i0) / Main.P, Main.in - Main.i0) + 1;
            
        }
    }

}