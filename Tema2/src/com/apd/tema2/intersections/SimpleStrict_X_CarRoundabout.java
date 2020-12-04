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
    CyclicBarrier barrier1;
    Semaphore exitSempahore;
    Object obj = new Object();
    Object obj1 = new Object();
    AtomicInteger nrCars = new AtomicInteger(0);
    AtomicInteger nrCars1 = new AtomicInteger(0);

    public void action(Car car) {
        /**inainte de a trece mai departe trebuie ca toate masinile sa faca reached**/
        System.out.println("Car " + car.getId() + " has reached the roundabout, now waiting...");
        try {
            barrier1.await();
        }
        catch(InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        /**inainte de a face selectia unei masini trebuie ca toate masinile din runda precedenta sa fie iesite din intersectie**/

        try {
            semaphores[car.getStartDirection()].acquire();
            synchronized (obj) {

                //while (nrCars.get() != x * noDirections && nrCars.get() != 0)
                if (nrCars.get() != 0) {
                    obj.wait();
                }
            }
            System.out.println("Car " + car.getId() + " was selected to enter the roundabout from lane " + car.getStartDirection());
        }
        catch(InterruptedException e) {
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
            /**inainte de a trece mai departe trebuie ca toate masinile sa paraseasca sensul giratoriu**/

            /**permite unei alte masini orientata pe aceeasi directie sa intre in intersectie**/
            semaphores[car.getStartDirection()].release();
            nrCars.getAndAdd(1);
            //System.out.println("Nr de masini detectat dupa ce a iesit masina " + car.getId() + " este " + nrCars);
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
        barrier1 = new CyclicBarrier(Main.carsNo);
        exitSempahore = new Semaphore(x * noDirections);
    }

    public void setTime(int T) {
        this.T = T;
    }

    public void setX_Value(int x) {
        this.x = x;
    }

}

