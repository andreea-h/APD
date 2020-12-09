package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

public class Railroad implements Intersection {

    CyclicBarrier barrier;
    Semaphore sem0;
    Semaphore sem1;
    ArrayBlockingQueue<Integer> syncList0Side;
    ArrayBlockingQueue<Integer> syncList1Side;

    public void initBarrier() {
        barrier = new CyclicBarrier(Main.carsNo);
        sem0 = new Semaphore(0, true);
        sem1 = new Semaphore(0, true);
        syncList0Side = new ArrayBlockingQueue<Integer>(Main.carsNo);
        syncList1Side = new ArrayBlockingQueue<Integer>(Main.carsNo);
    }

    public void action(Car car) {
        System.out.println("Car " + car.getId() + " from side number " +
                car.getStartDirection() + " has stopped by the railroad");

        if (car.getStartDirection() == 0) {
            syncList0Side.add(car.getId());
        }
        else {
            syncList1Side.add(car.getId());
        }

        try {
            barrier.await();
        }
        catch(BrokenBarrierException | InterruptedException e) {
            e.printStackTrace();
        }

        if (car.getId() == 0) {
            System.out.println("The train has passed, cars can now proceed");
        }

        /**toate thread-urile continua executia doar dupa ce s-a afisat mesajul cu trecerea trenului**/
        try {
            barrier.await();
        }
        catch(BrokenBarrierException | InterruptedException e) {
            e.printStackTrace();
        }

        if (car.getStartDirection() == 0) {
            /**primul element din lista de asteptare ar trebui sa fie cel cu id-ul curent**/
            /**busy waiting pana cand masina curenta ajunge sa fie cea care trebuie sa treaca **/

            while (syncList0Side.peek() != car.getId()) {
                //System.out.println("Primul in lista de sens 0 este " + syncList0Side.get(0));
            }

            System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has started driving");
            syncList0Side.poll();
        }
        else {
            while (syncList1Side.peek() != car.getId()) {
               // System.out.println("Primul in lista de sens 1 este " + syncList1Side.get(0));
            }

            System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has started driving");
            syncList1Side.poll();
        }

    }
}