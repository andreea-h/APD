package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

public class SimpleIntersection implements Intersection {
    // Define your variables here.

    public void action(Car car) {
        System.out.println("Car " + car.getId() + " has reached the semaphore, now waiting...");
        try {
            Thread.sleep(car.getWaitingTime());
        }
        catch(InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println("Car " + car.getId() + " has waited enough, now driving...");
    }
}
