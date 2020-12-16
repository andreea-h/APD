package task6;


import example.sampleForkJoinPool.MyTask;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.ForkJoinPool;
import java.util.concurrent.RecursiveTask;

class QueensTask extends RecursiveTask<Void> {
    int step;
    int[] sol;

    public QueensTask(int step, int[] sol) {
        this.step = step;
        this.sol = sol;
    }

    protected Void compute() {
        if (Main.N == step) {
            Main.printQueens(sol);
            return null;
        }
        List<QueensTask> tasks = new ArrayList<>();
        for (int i = 0; i < task3.Main.N; ++i) {
            int[] newGraph = sol.clone();
            newGraph[step] = i;

            if (task3.Main.check(newGraph, step)) {
                QueensTask t = new QueensTask(step + 1, newGraph);
                tasks.add(t);
                t.fork();
            }
            for (QueensTask task: tasks) {
                task.join();
            }
        }
        return null;
    }
}


public class Main {
    public static int N = 4;

    public static boolean check(int[] arr, int step) {
        for (int i = 0; i <= step; i++) {
            for (int j = i + 1; j <= step; j++) {
                if (arr[i] == arr[j] || arr[i] + i == arr[j] + j || arr[i] + j == arr[j] + i)
                    return false;
            }
        }
        return true;
    }

    public static void printQueens(int[] sol) {
        StringBuilder aux = new StringBuilder();
        for (int i = 0; i < sol.length; i++) {
            aux.append("(").append(sol[i] + 1).append(", ").append(i + 1).append("), ");
        }
        aux = new StringBuilder(aux.substring(0, aux.length() - 2));
        System.out.println("[" + aux + "]");
    }

    public static void queens(int[] graph, int step) {
        if (Main.N == step) {
            printQueens(graph);
            return;
        }
        for (int i = 0; i < Main.N; ++i) {
            int[] newGraph = graph.clone();
            newGraph[step] = i;

            if (check(newGraph, step)) {
                queens(newGraph, step + 1);
            }
        }
    }
    public static void main(String[] args) {
        int[] graph = new int[N];
        //queens(graph, 0);
        ForkJoinPool fjp = new ForkJoinPool(4);
        fjp.invoke(new QueensTask(0, graph));
        fjp.shutdown();
    }
}
