package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

/**intersectie in care pot intra x masini din fiecare directie la un moment de timp**/
public class SimpleStrict_X_CarRoundabout implements Intersection {
    int noDirections; /**numarul de directii din care pot veni masinile**/
    int T; /**timpul cat sta o masina in intersectie**/
    int x; /**numarul de masini dintr-o directie permise in sensul giratoriu**/

    Semaphore[] semaphores; /**tine controlul asupra numarului de masini care pot intra in intersectie din fiecare directie**/
    CyclicBarrier barrier;
    CyclicBarrier barrier1;
    static final Object obj = new Object();
    AtomicInteger nrCars = new AtomicInteger(0);

    public void action(Car car) {
        System.out.println("Car " + car.getId() + " has reached the roundabout, now waiting...");
        /**inainte de a trece mai departe trebuie ca toate masinile sa faca reached**/
        try {
            barrier.await();
        }
        catch(InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        /**inainte de a face selectia unei masini trebuie ca toate masinile din runda precedenta sa fie iesite din intersectie**/
        try {
            semaphores[car.getStartDirection()].acquire();
            synchronized (obj) {
                if (nrCars.get() != 0) {
                    obj.wait();
                }
            }
            System.out.println("Car " + car.getId() + " was selected to enter the roundabout from lane " + car.getStartDirection());
        }
        catch(InterruptedException e) {
            e.printStackTrace();
        }

        try {
            barrier1.await();
        }
        catch(InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        System.out.println("Car " + car.getId() + " has entered the roundabout from lane " + car.getStartDirection());
        try {
            Thread.sleep(T);
        }
        catch(InterruptedException e) {
            e.printStackTrace();
        }

        synchronized (obj) {
            System.out.println("Car " + car.getId() + " has exited the roundabout after " + (int)T/1000 + " seconds");
            /**permite unei alte masini orientata pe aceeasi directie sa intre in intersectie**/
            semaphores[car.getStartDirection()].release();
            nrCars.getAndAdd(1);
            if (nrCars.get() == x * noDirections) {
                obj.notifyAll();
                nrCars.set(0);
            }
        }
    }

    public void setNoDirections(int noDirections) {
        this.noDirections = noDirections;
        semaphores = new Semaphore[noDirections];
        int i;
        for (i = 0; i < noDirections; i++) {
            semaphores[i] = new Semaphore(x);
        }
        barrier = new CyclicBarrier(Main.carsNo);
        barrier1 = new CyclicBarrier(noDirections * x);
    }

    public void setTime(int T) {
        this.T = T;
    }

    public void setX_Value(int x) {
        this.x = x;
    }

}

