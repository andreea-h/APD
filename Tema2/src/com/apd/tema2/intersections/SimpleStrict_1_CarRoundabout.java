package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;


/**intersectie in care poate intra o singura masina din fiecare directie la un moment de timp**/
public class SimpleStrict_1_CarRoundabout implements Intersection {
    int noDirections; /**numarul de directii din care pot veni masinile**/
    int T; /**timpul cat sta o masina in intersectie**/

    Semaphore[] semaphores; /**tine controlul asupra numarului de masini care pot intra in intersectie din fiecare directie**/
    CyclicBarrier barrier; /**sincronizeaza masinile a.i. ele nu intra in intersectie pana cand nu intra excat una din fiecare directie**/
    Semaphore directionsSemaphore; /**controleaza ca fiecare directie din intersectie sa fie ocupata la un moment de timp**/


    public void action(Car car) {
        System.out.println("Car " + car.getId() + " has reached the roundabout");
        /**daca masina vine dintr-o directie care este deja 'ocupata' in sensul giratoriu,
         * va astepta pana la eliberarea sensului giratoriu pe acea directie**/
        /**in intersectie trebuie sa intre obligatoriu cate o masina din fiecare directie**/
        try {
            /**pentru a intra in intersectie, fiecare masina asteapta sa primeasca accesul in intersectie cate o masina pe fiecare directie**/
            try {
                barrier.await();
            }
            catch(InterruptedException | BrokenBarrierException e) {
                e.printStackTrace();
            }
            semaphores[car.getStartDirection()].acquire();
            System.out.println("Car " + car.getId() + " has entered the roundabout from lane " + car.getStartDirection());
        }
        catch(InterruptedException e) {
            e.printStackTrace();
        }
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
            semaphores[i] = new Semaphore(1);
        }
        //directionsSemaphore = new Semaphore(noDirections);
        barrier = new CyclicBarrier(noDirections);
    }

    public void setTime(int T) {
        this.T = T;
    }


}

