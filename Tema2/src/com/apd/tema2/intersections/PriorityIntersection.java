package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

public class PriorityIntersection implements Intersection {

     static public class ModifiedSempahore extends Semaphore {

         int permits;
         public ModifiedSempahore(int permits) {
             super(permits);
             this.permits = permits;
         }

         public synchronized void resize(int delta) {
             this.reducePermits(Math.abs(delta));
             this.permits = permits - delta;
         }

     }

     public Object obj = new Object(); /**calculeaza numarul de masini cu prioritate aflate in intersectie**/
     AtomicInteger nrPriorityCars = new AtomicInteger(0);
     Object objs[] = new Object[Main.carsNo];
     TargetNotify object = new TargetNotify(Main.carsNo);
     ModifiedSempahore sem;
     CyclicBarrier barrier;
     int highPrioCars; /**numarul de masini cu prioritate care ajung la intersectie**/
     int lowPrioCars;

     public void setHighPrioCars(int highPrioCars) {
        this.highPrioCars = highPrioCars;
     }

     public void setLowPrioCars(int lowPrioCars) {
        this.lowPrioCars = lowPrioCars;
     }


     public void setNotif() {
         int i;
         for (i = 0 ; i < Main.carsNo; i++) {
             objs[i] = new Object();
         }
         sem = new ModifiedSempahore(1);
         barrier = new CyclicBarrier(highPrioCars);
     }

    public void action(Car car) {
        /**o masina cu prioritate intra in intersectie la orice moment de timp**/
        if (car.getPriority() != 1) {
            System.out.println("Car " + car.getId() + " with high priority has entered the intersection");
            sem.resize(1);

            try {
                Thread.sleep(2000); /**masina cu prioritate are nevoie de 2 sec sa paraseasca intersectia**/
            }
            catch(InterruptedException e) {
                e.printStackTrace();
            }
            System.out.println("Car " + car.getId() + " with high priority has exited the intersection");

            sem.release();

        }
        else { /**masinile fara prioritate trebuie sa astepte toate masinile cu prioritate sa iasa din intersectie**/
            /**afisarea mesajului aferent inainte ca o masina cu prioritate mica sa intre in intersectie**/
            if (car.getPriority() == 1) {
                System.out.println("Car " + car.getId() + " with low priority is trying to enter the intersection...");
            }

            try {
                sem.acquire();
            }
            catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Car " + car.getId() + " with low priority has entered the intersection");
            sem.release();
        }
    }

}
