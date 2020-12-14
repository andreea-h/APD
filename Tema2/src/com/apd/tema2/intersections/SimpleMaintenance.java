package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

public class SimpleMaintenance implements Intersection {

    int x; /**cate masini vor circula pe o singura banda**/
    Semaphore semaphore0Side;
    Semaphore semaphore1Side;
    public final Object obj = new Object();
    public final Object obj1 = new Object();
    AtomicInteger nr0Side; /**numarul de masini care au trecut pe sensul 0**/
    AtomicInteger nr1Side;

    public void action(Car car) {
        System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has reached the bottleneck");

        if (car.getStartDirection() == 0) {
            /**este permisa doar trecerea alternativa a masinilor dintr-un anumit sens**/
            /**este permisa trecerea pe sensul 0 daca au trecut cele x masini de pe sensul 1**/
            synchronized (obj1) {
                try {
                    if (nr1Side.get() % x != 0 && nr1Side.get() != 0) {
                        obj1.wait();
                    }
                }
                catch(InterruptedException e) {
                    e.printStackTrace();
                }
            }
            try {
                semaphore0Side.acquire();
                /**atunci cand se face trecerea pe un sens, celalalt sens trebuie blocat**/
                /**masinile de pe sensul 1 trebuie sa intre in busy-waiting**/
            }
            catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has passed the bottleneck");
            nr0Side.addAndGet(1);
             if (nr0Side.get() % x == 0) {
                 synchronized (obj) {
                     obj.notifyAll();
                 }
                 semaphore1Side.release(x);
            }
        }
        else {
            /** asteapta trecerea tuturor celor x masini de pe sensul 0 **/
            synchronized (obj) {
                try {
                    if (nr0Side.get() % x != 0) {
                        obj.wait();
                    }
                }
                catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            try {
                semaphore1Side.acquire();
            }
            catch (InterruptedException e) {
                e.printStackTrace();
            }

            System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has passed the bottleneck");
            nr1Side.addAndGet(1);
            if (nr1Side.get() % x == 0) {
                synchronized (obj1) {
                    obj1.notifyAll();
                }
                semaphore0Side.release(x);
            }
        }
    }

    public void setX(int x) {
        this.x = x;
        semaphore0Side = new Semaphore(x);
        semaphore1Side = new Semaphore(0);
        nr0Side = new AtomicInteger(0);
        nr1Side = new AtomicInteger(0);
    }
}
