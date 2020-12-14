package com.apd.tema2.entities;

import com.apd.tema2.intersections.CrossWalk;
import com.apd.tema2.utils.Constants;

import static java.lang.Thread.sleep;

/**
 * Clasa utilizata pentru gestionarea oamenilor care se strang la trecerea de pietoni.
 */
public class Pedestrians implements Runnable {
    private int pedestriansNo = 0;
    private int maxPedestriansNo;
    private boolean pass = false;
    private boolean finished = false;
    private int executeTime;
    private long startTime;

    public Pedestrians(int executeTime, int maxPedestriansNo) {
        this.startTime = System.currentTimeMillis();
        this.executeTime = executeTime;
        this.maxPedestriansNo = maxPedestriansNo;
    }

    @Override
    public void run() {
        while(System.currentTimeMillis() - startTime < executeTime) {
            try {
                pedestriansNo++;

                sleep(Constants.PEDESTRIAN_COUNTER_TIME); /**a ajuns un pieton la trecere**/
                if(pedestriansNo == maxPedestriansNo) { /**au ajuns toti pietonii la trecere**/
                    pedestriansNo = 0;
                    pass = true; /**s-au strans toti pietonii la trecere->se trece semaforul pe rosu**/

                    /**notifica masinile ca semaforul este rosu**/
                    synchronized (CrossWalk.green) {
                        CrossWalk.green.notifyAll();
                    }
                    sleep(Constants.PEDESTRIAN_PASSING_TIME); /**timpul de trecere al pietonilot*/
                    pass = false; /**semaforul devine din nou verde pt masini**/

                    synchronized (CrossWalk.red) {
                        CrossWalk.red.notifyAll();
                    }
                    /**notifica masinile ca semaforul este verde**/
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        synchronized (CrossWalk.green) {
            CrossWalk.green.notifyAll();
        }
        synchronized (CrossWalk.red) {
            CrossWalk.red.notifyAll();
        }
        finished = true;
    }

    public boolean isPass() {
        return pass;
    }

    public boolean isFinished() {
        return finished;
    }
}
