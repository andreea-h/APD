package com.apd.tema2.intersections;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Intersection;

import java.util.concurrent.atomic.AtomicInteger;

public class CrossWalk implements Intersection {

    public static Object obj = new Object();
    public static Object obj1 = new Object();
    public static Object obj3 = new Object();
    private boolean check = false;

    public void action(Car car) {

           // if (Main.pedestrians.isPass() == false) { /**nu s-au strans suficienti pietoni**/
           //     System.out.println("Car " + car.getId() + " has how green light");
           // }

        while (Main.pedestrians.isFinished() == false) {
            check = false;
            synchronized (obj) {
                try {
                    while (Main.pedestrians.isPass() == false && Main.pedestrians.isFinished() == false) { /**cat timp nu au ajuns toti pietonii**/
                        System.out.println("Car " + car.getId() + " has now green light");
                        check = true;
                        obj.wait();
                    }
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            //}

            synchronized (obj1) {
                try {
                    while (Main.pedestrians.isPass() == true && Main.pedestrians.isFinished() == false) { /**cat timp trec pietoni**/
                        check = false;
                        System.out.println("Car " + car.getId() + " has now red light");
                        obj1.wait();
                    }

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

        }
        if (check == false) {
            System.out.println("Car " + car.getId() + " has how green light"); //are sens doar dupa red
        }

    }

}
