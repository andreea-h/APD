package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;

public class Simple_N_Intersection implements Intersection {
    public Semaphore s;
    public int N, T;

    public void action(Car car) {
        System.out.println("Car " + car.getId() + " has reached the roundabout, now waiting...");

        try {
            s.acquire();
            System.out.println("Car " + car.getId() + " has entered the roundabout");
        }
        catch(InterruptedException e) {
            e.printStackTrace();
        }
        try {
            Thread.sleep(T);
        }
        catch (InterruptedException e) {
            e.printStackTrace();
        }
        System.out.println("Car " + car.getId() + " has exited the roundabout after " + (int)T/1000 + " seconds");
        s.release();
    }

    public void setN(int N) {
        this.N = N;
        s = new Semaphore(N); /** numarul maxim de masini care se pot afla in sensul giratoriu **/
    }

    public void setT(int T) {
        this.T = T; /**timpul cat dureaza ca fiecare masina sa paraseasca sensul giratoriu**/
    }


}
