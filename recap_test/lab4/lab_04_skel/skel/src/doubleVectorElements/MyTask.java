package doubleVectorElements;

public class MyTask extends Thread {

    private int id;

    public MyTask(int id) {
        this.id = id;
    }

    public void run() {
        int i;
        int start = (int) (id * (double)(Main.N) / (Main.P));
        int end = (int) Math.min((id + 1) * (double) (Main.N) / (Main.P), (Main.N));
        
        for (i = start; i < end; i++) {
            int val = Main.v[i];
            Main.v[i] = 2 * val;
        }
    }
}