package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import javax.management.ObjectName;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;

public class ComplexMaintenance implements Intersection {

    int x; /**cate masini vor circula pe o singura banda**/
    int initialLines; /**numar benzi vechi**/
    int freeLines; /**numar benzi noi**/
    CyclicBarrier barrier;
    List<ConcurrentHashMap<Integer, List<Integer>>> hash_map;
    Semaphore sem;

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
        synchronized (this) {
            System.out.println("Car " + car.getId() + " has come from the lane number " + car.getStartDirection());
            int newLane = getNewBand(car.getStartDirection());
            ConcurrentHashMap<Integer, List<Integer>> h_map = hash_map.get(newLane);

            List<Integer> elems = hash_map.get(newLane).get(car.getStartDirection()); //lista din hashmap asociata directiei
            if (elems == null) {
                elems = Collections.synchronizedList(new ArrayList<>());;
            }
            elems.add(car.getId());
            h_map.put(car.getStartDirection(), elems);
            hash_map.set(newLane, h_map);
        }

        try {
            barrier.await();
        }
        catch (InterruptedException | BrokenBarrierException e) {
            e.printStackTrace();
        }

        if (car.getId() == 0) {
            int i;
            for (i = 0; i < freeLines; i++) {
                ConcurrentHashMap<Integer, List<Integer>> h1_map = hash_map.get(i);
                System.out.println("pentru lan " + i + ": " + h1_map);
            }
        }
        try {
            barrier.await();
        }
        catch (BrokenBarrierException | InterruptedException e) {
            e.printStackTrace();
        }
        int newBand = getNewBand(car.getStartDirection());
        List<Integer> elems = hash_map.get(newBand).get(car.getStartDirection());

        while ((int)hash_map.get(newBand).keySet().toArray()[0] != car.getStartDirection()) {

        }

        while (elems.get(0) != car.getId()) {
        }

        synchronized (this) {
            System.out.println("Car " + car.getId() +" from the lane " + car.getStartDirection() +" has entered lane number " + newBand);
            ConcurrentHashMap<Integer, List<Integer>> h_map1 = hash_map.get(newBand);
            List<Integer> elems1 = hash_map.get(newBand).get(car.getStartDirection());
            elems1.remove(0);
            h_map1.put(car.getStartDirection(), elems);
            hash_map.set(newBand, h_map1);

            if (elems1.size() == 0) {
                System.out.println("The initial lane " + car.getStartDirection() + " has been emptied and removed from the new lane queue");
                int i;
                for (i = 0; i < freeLines; i++) {
                    ConcurrentHashMap<Integer, List<Integer>> h1_map = hash_map.get(i);
                    System.out.println("pentru lan " + i + ": " + h1_map);
                }
            }
            else {
                System.out.println("The initial lane " + car.getStartDirection() + " has no permits and is moved to the back of the new lane queue");
                ConcurrentHashMap<Integer, List<Integer>> h_map2 = hash_map.get(newBand);
                List<Integer> elems2 = hash_map.get(newBand).get(car.getStartDirection());
                hash_map.get(newBand).remove(car.getStartDirection());
                h_map2.remove(car.getStartDirection());
                h_map2.put(car.getStartDirection(), elems2);
                hash_map.set(newBand, h_map2);

                int i;
                for (i = 0; i < freeLines; i++) {
                    ConcurrentHashMap<Integer, List<Integer>> h1_map = hash_map.get(i);
                    System.out.println("pentru lan " + i + ": " + h1_map);
                }
            }
        }
    }

    public void setValues(int x, int freeLines, int initialLines) {
        this.x = x;
        this.initialLines = initialLines;
        this.freeLines = freeLines;
        barrier = new CyclicBarrier(Main.carsNo);
        hash_map = Collections.synchronizedList(new ArrayList<>());
        int i;
        for (i = 0; i < freeLines; i++) {
            hash_map.add(new ConcurrentHashMap<Integer, List<Integer>>());
        }
        sem = new Semaphore(x);
    }
}