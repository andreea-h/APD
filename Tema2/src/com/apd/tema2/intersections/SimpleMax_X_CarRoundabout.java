package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;


/**intersectie in care pot intra x masini din fiecare directie la un moment de timp**/
public class SimpleMax_X_CarRoundabout implements Intersection {
    int noDirections; /**numarul de directii din care pot veni masinile**/
    int T; /**timpul cat sta o masina in intersectie**/
    int x; /**numarul de masini dintr-o directie permise in sensul giratoriu**/
    Semaphore[] semaphores; /**tine controlul asupra numarului de masini care pot intra in intersectie din fiecare directie**/

    public void action(Car car) {
        System.out.println("Car " + car.getId() + " has reached the roundabout from lane " + car.getStartDirection());
        try {
            semaphores[car.getStartDirection()].acquire();
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
        System.out.println("Car " + car.getId() + " has exited the roundabout after " + (int)T/1000 + " seconds");
        /**permite unei alte masini orientata pe aceeasi directie sa intre in intersectie**/
        semaphores[car.getStartDirection()].release();
    }

    public void setNoDirections(int noDirections) {
        this.noDirections = noDirections;
        semaphores = new Semaphore[noDirections];
        int i;
        for (i = 0; i < noDirections; i++) {
            semaphores[i] = new Semaphore(x);
        }
    }

    public void setTime(int T) {
        this.T = T;
    }

    public void setX_Value(int x) {
        this.x = x;
    }

}

