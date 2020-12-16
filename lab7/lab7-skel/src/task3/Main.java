package task3;


import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

class QueensTask implements Runnable {
    int step;
    ExecutorService tpe;
    int[] sol;

    public QueensTask(ExecutorService tpe, int step, int[] sol) {
        this.tpe = tpe;
        this.step = step;
        this.sol = sol;
    }

    public void run() {
        if (Main.N == step) {
            Main.printQueens(sol);
            tpe.shutdown();
            return;
        }
        for (int i = 0; i < Main.N; ++i) {
            int[] newGraph = sol.clone();
            newGraph[step] = i;

            if (Main.check(newGraph, step)) {
                tpe.submit(new QueensTask(tpe, step + 1, newGraph));
            }
        }
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

    static int[] graph = new int[N];

    public static void main(String[] args) {
       // queens(Main.graph, 0);

        ExecutorService tpe = Executors.newFixedThreadPool(4);
        tpe.submit(new QueensTask(tpe, 0, graph));
    }
}
