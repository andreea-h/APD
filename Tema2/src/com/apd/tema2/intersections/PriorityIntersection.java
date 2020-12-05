package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.atomic.AtomicInteger;

public class PriorityIntersection implements Intersection {

    public Object obj = new Object(); /**calculeaza numarul de masini cu prioritate aflate in intersectie**/
     AtomicInteger nrPriorityCars = new AtomicInteger(0);
     Object objs[] = new Object[Main.carsNo];
     TargetNotify object = new TargetNotify(Main.carsNo);

     public void setNotif() {
         int i;
         for (i = 0 ; i < Main.carsNo; i++) {
             objs[i] = new Object();
         }
     }


    public void action(Car car) {
        /**o masina cu prioritate intra in intersectie la orice moment de timp**/
        if (car.getPriority() != 1) {
            System.out.println("Car " + car.getId() + " with high priority has entered the intersection");
            nrPriorityCars.addAndGet(1);

            try {
                Thread.sleep(2000); /**masina cu prioritate are nevoie de 2 sec sa paraseasca intersectia**/
            }
            catch(InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Car " + car.getId() + " with high priority has exited the intersection");
            nrPriorityCars.addAndGet(-1);

            synchronized (obj) {
                if (nrPriorityCars.get() == 0) {
                    obj.notifyAll();
                }
            }
        }
        else { /**masinile fara prioritate trebuie sa astepte toate masinile cu prioritate sa iasa din intersectie**/
            /**afisarea mesajului aferent inainte ca o masina cu prioritate mica sa intre in intersectie**/
            if (car.getPriority() == 1) {
                System.out.println("Car " + car.getId() + " with low priority is trying to enter the intersection...");
            }
            synchronized (obj) {
                try {
                    while(nrPriorityCars.get() != 0) {
                        obj.wait();
                    }
                }
                catch(InterruptedException e) {
                    e.printStackTrace();
                }
            }
            System.out.println("Car " + car.getId() + " with low priority has entered the intersection");
        }
    }

}
