package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.atomic.AtomicInteger;

public class CrossWalk implements Intersection {


    public void action(Car car) {
        System.out.println(Main.pedestrians.isFinished());
    }

}
