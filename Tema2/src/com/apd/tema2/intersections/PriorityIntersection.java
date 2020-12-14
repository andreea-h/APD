package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.Semaphore;

public class PriorityIntersection implements Intersection {

     static public class ModifiedSempahore extends Semaphore {

         int permits;
         public ModifiedSempahore(int permits) {
             super(permits);
             this.permits = permits;
         }

         /**scade availablePermits pentru semaforul pentru care se apeleaza metoda**/
         public synchronized void resize(int delta) {
             this.reducePermits(Math.abs(delta));
             this.permits = permits - delta;
         }
     }

     ModifiedSempahore sem;
     int highPrioCars; /**numarul de masini cu prioritate diff de 1 care ajung la intersectie**/
     int lowPrioCars; /**numarul de masini cu prioritate 1 care ajung la intersectie**/

     public void setHighPrioCars(int highPrioCars) {
        this.highPrioCars = highPrioCars;
     }

     public void setLowPrioCars(int lowPrioCars) {
        this.lowPrioCars = lowPrioCars;
     }

     public void setNotif() {
         sem = new ModifiedSempahore(1);
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
        else { /**masinile fara prioritate trebuie sa astepte ca toate masinile cu prioritate sa iasa din intersectie**/
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
