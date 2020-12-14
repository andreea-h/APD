package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.atomic.AtomicInteger;

public class CrossWalk implements Intersection {

    public static final Object green = new Object();
    public static final Object red = new Object();
    private boolean check = false;

    public void action(Car car) {
        while (!(Main.pedestrians.isFinished())) {
            check = false;
            synchronized (green) {
                try {
                    /**cat timp nu au ajuns toti pietonii**/
                    if (!(Main.pedestrians.isPass()) && !(Main.pedestrians.isFinished())) {
                        System.out.println("Car " + car.getId() + " has now green light");
                        check = true;
                        green.wait();
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            synchronized (red) {
                try {
                    /**cat timp trec pietoni**/
                    if (Main.pedestrians.isPass() && !(Main.pedestrians.isFinished())) {
                        check = false;
                        System.out.println("Car " + car.getId() + " has now red light");
                        red.wait();
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
        if (!check) {
            System.out.println("Car " + car.getId() + " has now green light"); /**are sens doar dupa ce sem a for red**/
        }
    }
}
