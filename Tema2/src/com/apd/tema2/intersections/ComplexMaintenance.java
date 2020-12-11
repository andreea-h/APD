package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;
import java.util.concurrent.atomic.AtomicInteger;

public class ComplexMaintenance implements Intersection {

    int x; /**cate masini vor circula pe o singura banda**/
    int initialLines; /**numar benzi vechi**/
    int freeLines; /**numar benzi noi**/
    CyclicBarrier barrier;
    Semaphore[] semaphores; /**vector de semafoare care controleaza ca masinile sa porneasca in ordinea in care au ajuns de pe o banda veche**/
    Semaphore[] semaphores1;
    AtomicInteger[] nrCars;
    List<ArrayBlockingQueue> carsInOrder; /**masinile retinute pe fiecare sens in ordinea sosirii**/
    Object obj;

    /**intoarce valoarea 0 daca banda testBand trebuie redirectata catre banda newBand**/
    public int getSpecificBand(int newBand, int testBand) {
        int i;
        int start = newBand * (int) Math.ceil((double) initialLines / freeLines);
        int end = Math.min(initialLines, (newBand + 1) * (int) Math.floor((double) initialLines / freeLines));
        if (testBand >= start && testBand < end) {
            return 0;
        }
        return -1;
    }

    public int getNewBand(int oldBand) {
        int i;
        for (i = 0; i < freeLines; i++) {
            if (getSpecificBand(i, oldBand) == 0) {
                return i;
            }
        }
        return freeLines - 1;
    }

    public void action(Car car) {
        System.out.println("Car " + car.getId() + " has come from the lane number " + car.getStartDirection());
        ArrayBlockingQueue<Integer> elems = carsInOrder.get(car.getStartDirection());
        elems.add(car.getId());
        carsInOrder.set(car.getStartDirection(), elems);

        /**asteapta pana toate masinile s-au incolonat**/
        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        int band = car.getStartDirection();

        /**determina banda noua asociata benzii vechi de pe care vine masina**/

        while ((int)carsInOrder.get(car.getStartDirection()).peek() != (int)(car.getId())) {
        }
        /**maxim x masini pot face 'entered' pe cate unul dintre cele free_lines sensuri**/
        int newBand = getNewBand(band);
        System.out.println("Car " + car.getId() + " from the lane " +
                car.getStartDirection() + " has entered lane number " + newBand);
        carsInOrder.get(car.getStartDirection()).poll();

        /**nu mai trebuie sa vina nicio masina de pe acea banda**/
        /*if (semaphores1[car.getStartDirection()].availablePermits() == 0) {
            System.out.println("The initial lane " + car.getStartDirection() +" has been emptied and removed from the new lane queue");
        }
        else {
            System.out.println("The initial lane " + car.getStartDirection() + " has no permits and is moved to the back of the new lane queue");
        }*/
    }

    public void setValues(int x, int freeLines, int initialLines) {
        this.x = x;
        this.initialLines = initialLines;
        this.freeLines = freeLines;
        barrier = new CyclicBarrier(Main.carsNo);
        semaphores = new Semaphore[initialLines];
        semaphores1 = new Semaphore[initialLines];
        nrCars = new AtomicInteger[initialLines];
        obj = new Object();
        int i;

        for (i = 0; i < initialLines; i++) {
            nrCars[i] = new AtomicInteger(0);
        }

        for (i = 0; i < initialLines; i++) {
            semaphores[i] = new Semaphore(0, true);
            semaphores1[i] = new Semaphore(x, true);
        }
        carsInOrder = Collections.synchronizedList(new ArrayList<>());
        for (i = 0; i < initialLines; i++) {
            carsInOrder.add(new ArrayBlockingQueue<Integer>(Main.carsNo));
        }
    }
}
