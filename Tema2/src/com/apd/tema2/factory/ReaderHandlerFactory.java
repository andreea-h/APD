package com.apd.tema2.factory;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Pedestrians;
import com.apd.tema2.entities.ReaderHandler;
import com.apd.tema2.intersections.*;

import java.io.BufferedReader;
import java.io.IOException;

/**
 * Returneaza sub forma unor clase anonime implementari pentru metoda de citire din fisier.
 */
public class ReaderHandlerFactory {

    public static ReaderHandler getHandler(String handlerType) {
        // simple semaphore intersection
        // max random N cars roundabout (s time to exit each of them)
        // roundabout with exactly one car from each lane simultaneously
        // roundabout with exactly X cars from each lane simultaneously
        // roundabout with at most X cars from each lane simultaneously
        // entering a road without any priority
        // crosswalk activated on at least a number of people (s time to finish all of them)
        // road in maintenance - 1 lane 2 ways, X cars at a time
        // road in maintenance - N lanes 2 ways, X cars at a time
        // railroad blockage for T seconds for all the cars
        // unmarked intersection
        // cars racing
        return switch (handlerType) {
            case "simple_semaphore" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) {
                    // Exemplu de utilizare:
                   Main.intersection = IntersectionFactory.getIntersection("simpleIntersection");
                }
            };
            case "simple_n_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    // To parse input line use:
                    Main.intersection = IntersectionFactory.getIntersection("simple_n_Intersection");
                    String[] line = br.readLine().split(" ");
                    int n = Integer.parseInt(line[0]);/**nr maxim de masini care se pot fi intrate in intersectie la un moment dat**/
                    int T = Integer.parseInt(line[1]);
                    ((Simple_N_Intersection)Main.intersection).setN(n);
                    ((Simple_N_Intersection)Main.intersection).setT(T);
                }
            };
            case "simple_strict_1_car_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_strict_1_car_roundabout");
                    String[] line = br.readLine().split(" ");
                    int noDirections = Integer.parseInt(line[0]); /**numarul de directii din care pot veni masinile**/
                    int T = Integer.parseInt(line[1]);/**timpul cat sta in sensul giratoriu o masina***/
                    ((SimpleStrict_1_CarRoundabout)Main.intersection).setNoDirections(noDirections);
                    ((SimpleStrict_1_CarRoundabout)Main.intersection).setTime(T);
                }
            };
            case "simple_strict_x_car_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_strict_x_car_roundabout");
                    String[] line = br.readLine().split(" ");
                    int noDirections = Integer.parseInt(line[0]); /**numarul de directii din care pot veni masinile**/
                    int T = Integer.parseInt(line[1]); /**timpul cat sta in sensul giratoriu o masina***/
                    int x = Integer.parseInt(line[2]); /**numarul maxim de masini permise intr-o directie in sensul giratoriu**/
                    ((SimpleStrict_X_CarRoundabout)Main.intersection).setX_Value(x);
                    ((SimpleStrict_X_CarRoundabout)Main.intersection).setNoDirections(noDirections);
                    ((SimpleStrict_X_CarRoundabout)Main.intersection).setTime(T);
                }
            };
            case "simple_max_x_car_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_max_x_car_roundabout");
                    String[] line = br.readLine().split(" ");
                    int noDirections = Integer.parseInt(line[0]); /**numarul de directii din care pot veni masinile**/
                    int T = Integer.parseInt(line[1]); /**timpul cat sta in sensul giratoriu o masina***/
                    int x = Integer.parseInt(line[2]); /**numarul maxim de masini permise intr-o directie in sensul giratoriu**/
                    ((SimpleMax_X_CarRoundabout)Main.intersection).setX_Value(x);
                    ((SimpleMax_X_CarRoundabout)Main.intersection).setNoDirections(noDirections);
                    ((SimpleMax_X_CarRoundabout)Main.intersection).setTime(T);
                }
            };
            case "priority_intersection" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("priority_intersection");
                    String[] line = br.readLine().split(" ");
                    int high = Integer.parseInt(line[0]);
                    int low = Integer.parseInt(line[1]);
                    ((PriorityIntersection)Main.intersection).setHighPrioCars(high);
                    ((PriorityIntersection)Main.intersection).setLowPrioCars(low);
                    ((PriorityIntersection)Main.intersection).setNotif();
                }
            };
            case "crosswalk" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("crosswalk");
                    String[] line = br.readLine().split(" ");
                    int time = Integer.parseInt(line[0]); /**timpul cat inca vin pietoni**/
                    int nrPedestrians = Integer.parseInt(line[1]); /**numarul de pietoni de la care trebuie sa se permita trecerea**/
                    Main.pedestrians = new Pedestrians(time, nrPedestrians);
                }
            };
            case "simple_maintenance" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_maintenance");
                    String[] line = br.readLine().split(" ");
                    int x = Integer.parseInt(line[0]); /**cate masini circula pe o singura banda**/
                    ((SimpleMaintenance)Main.intersection).setX(x);
                }
            };
            case "complex_maintenance" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    
                }
            };
            case "railroad" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    
                }
            };
            default -> null;
        };
    }

}
