package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import javax.management.ObjectName;
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
    CyclicBarrier barrier1;
    Semaphore[] semaphores; /**vector de semafoare care controleaza ca masinile sa porneasca in ordinea in care au ajuns de pe o banda veche**/
    Semaphore[] semaphores1;
    List<AtomicInteger> nrCars;
    List<AtomicInteger> nrCarsIn;
    AtomicInteger counter;
    Semaphore directions;
    AtomicInteger newBand;

    List<ArrayBlockingQueue> lanes; /**lista de lanuri**/
    List<Integer> nrCarsNewLane;
    List<ArrayBlockingQueue> carsInOrder; /**masinile retinute pe fiecare sens in ordinea sosirii**/
    public static final Object obj = new Object();
    Semaphore semaphore2;
    Semaphore semaphore3[];
    Object[] objs;

    /**intoarce valoarea 0 daca banda testBand trebuie redirectata catre banda newBand**/
    public int getSpecificBand(int newBand, int testBand) {
        int i;
        int start = newBand * /*(int) Math.ceil((double)*/ (initialLines / freeLines);
        int end = Math.min(initialLines, (newBand + 1) * /*(int) Math.floor((double)*/ (initialLines / freeLines));
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
        nrCars.get(car.getStartDirection()).addAndGet(1);

        /**adauga banda veche in lista benzii noi corespunzatoare**/
        synchronized (obj) {
            int band = car.getStartDirection();
            newBand.set(getNewBand(band));
            if (lanes.get(newBand.get()).contains(car.getStartDirection()) == false) {
                lanes.get(newBand.get()).add(car.getStartDirection());
            }
        }

        /**asteapta pana toate masinile s-au incolonat**/
        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        if (car.getId() == 0) {
            int i;
            for (i = 0; i < initialLines; i++) {
                if (nrCars.get(i).get() < x) {
                    try {
                        semaphores[i].acquire(x); /**prin semaphores se retine numarul de masini care vin de pe fiecare banda initiala la o iteratie**/
                        semaphores[i].release(nrCars.get(i).get());
                    }
                    catch(InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }

            for (i = 0; i < freeLines; i++) {
                System.out.println("Benzile pt lane " + i + ": " + lanes.get(i));
            }
        }

        try {
            barrier.await();
        } catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////////////

        /*newBand.set(getNewBand(car.getStartDirection()));
        System.out.println("Banda noua accesata: " + newBand.get());
        while((int)lanes.get(newBand.get()).peek() != car.getStartDirection()) {
        }*/


        /**asigura ca ordinea masinilor la trecere este aceeasi cu ordinea sosirii lor**/
        while ((int) carsInOrder.get(car.getStartDirection()).peek() != (int) (car.getId())) {
        }

        try {
            semaphores[car.getStartDirection()].acquire();
        }
        catch (InterruptedException e) {
            e.printStackTrace();
        }
/*
        try {
            semaphore2.acquire();
        }
        catch (InterruptedException e) {
            e.printStackTrace();
        }*/

        System.out.println("Masina intrata este: " + car.getId());
        nrCarsNewLane.set(newBand.get(), nrCarsNewLane.get(newBand.get()) + 1); //numarul de masini care au trecut pe lanul nou curent

        if (nrCarsNewLane.get(newBand.get()).equals(0)) {
            while ((int)lanes.get(newBand.get()).peek() != car.getStartDirection()) {

            }
        }
        else {
            synchronized (obj) {
                try {
                    newBand.set(getNewBand(car.getStartDirection()));
                    while ((int)lanes.get(newBand.get()).peek() != car.getStartDirection() && !(nrCarsNewLane.get(newBand.get()).equals(0))) {
                        obj.wait();
                    }
                }
                catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }


        //asteapta pana cand banda veche a masinii este prima in lista de astepare
        /**maxim x masini pot face 'entered' pe cate unul dintre cele free_lines sensuri**/
        newBand.set(getNewBand(car.getStartDirection()));

        System.out.println("Car " + car.getId() + " from the lane " +
                car.getStartDirection() + " has entered lane number " + newBand);
        carsInOrder.get(car.getStartDirection()).poll();

        nrCars.get(car.getStartDirection()).addAndGet(-1);
        nrCarsIn.get(car.getStartDirection()).addAndGet(1);

        if (nrCarsIn.get(car.getStartDirection()).get() % x == 0) {
            if (nrCars.get(car.getStartDirection()).get() == 0) {
                synchronized (obj) {
                    newBand.set(getNewBand(car.getStartDirection()));
                    System.out.println("The initial lane " + car.getStartDirection() +" has been emptied and removed from the new lane queue");
                    lanes.get(newBand.get()).poll();
                    System.out.println("Banda la stergere este: " + lanes.get(newBand.get()));
                    obj.notify();
                }
            }
            else {
                synchronized (obj) {
                    newBand.set(getNewBand(car.getStartDirection()));
                    System.out.println("The initial lane " + car.getStartDirection() + " has no permits and is moved to the back of the new lane queue");
                    lanes.get(newBand.get()).poll();
                    if (lanes.get(newBand.get()).contains(car.getStartDirection()) == false) {
                        lanes.get(newBand.get()).add(car.getStartDirection());
                    }
                    System.out.println("Banda permutata este: " + lanes.get(newBand.get()));
                    obj.notify();
                }
                semaphores[car.getStartDirection()].release(x);
                if (nrCars.get(car.getStartDirection()).get() < x) {
                    try {
                        semaphores[car.getStartDirection()].acquire(x);
                    }
                    catch(InterruptedException e) {
                        e.printStackTrace();
                    }
                    semaphores[car.getStartDirection()].release(nrCars.get(car.getStartDirection()).get());
                }
            }
        }
        if (semaphore2.availablePermits() == 0) {
            semaphore2.release(x * freeLines);
        }
    }

    public void setValues(int x, int freeLines, int initialLines) {
        this.x = x;
        this.initialLines = initialLines;
        this.freeLines = freeLines;
        barrier = new CyclicBarrier(Main.carsNo);
        semaphores = new Semaphore[initialLines];
        semaphores1 = new Semaphore[initialLines];
        nrCars = Collections.synchronizedList(new ArrayList<AtomicInteger>());
        nrCarsIn = Collections.synchronizedList(new ArrayList<AtomicInteger>());

        int i;

        for (i = 0; i < initialLines; i++) {
            nrCars.add(new AtomicInteger(0));
            nrCarsIn.add(new AtomicInteger(0));
        }

        for (i = 0; i < initialLines; i++) {
            semaphores[i] = new Semaphore(x, true);
            semaphores1[i] = new Semaphore(x, true);
        }
        carsInOrder = Collections.synchronizedList(new ArrayList<>());

        lanes = Collections.synchronizedList(new ArrayList<>(freeLines));
        for (i = 0; i < freeLines; i++) {
            lanes.add(new ArrayBlockingQueue<Integer>(Main.carsNo));
        }

        for (i = 0; i < initialLines; i++) {
            carsInOrder.add(new ArrayBlockingQueue<Integer>(Main.carsNo));
        }
        semaphore2 = new Semaphore(freeLines * x);
        semaphore3 = new Semaphore[initialLines];
        for (i = 0; i < initialLines; i++) {
            semaphore3[i] = new Semaphore(x); /**contoleaza cate masini vin de pe fiecare din cele freeLines sensuri la o iteratie**/
        }
        counter = new AtomicInteger(0);
        directions = new Semaphore(freeLines * x);
        newBand = new AtomicInteger(0);
        barrier1 = new CyclicBarrier(x * freeLines);
        nrCarsNewLane = Collections.synchronizedList(new ArrayList<>());
        for (i = 0; i < freeLines; i++) {
            nrCarsNewLane.add(0);
        }
        objs = new Object[freeLines];
        for (i = 0; i < freeLines; i++) {
            objs[i] = 0;
        }
    }
}
