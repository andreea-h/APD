package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;

public class SimpleMaintenance implements Intersection {

    int x; /**cate masini vor circula pe o singura banda**/
    Semaphore semaphore0Side;
    Semaphore semaphore1Side;
    CyclicBarrier barrier;

    public void action(Car car) {
        System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has reached the bottleneck");

        if (car.getStartDirection() == 0) {
            try {
                semaphore0Side.acquire();
            }
            catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has passed the bottleneck");
            semaphore0Side.release();
        }
        else {
            try {
                semaphore1Side.acquire();
            }
            catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has passed the bottleneck");
            semaphore1Side.release();
        }

    }
    public void setX(int x) {
        this.x = x;
        semaphore0Side = new Semaphore(x);
        semaphore1Side = new Semaphore(x);
    }
}
