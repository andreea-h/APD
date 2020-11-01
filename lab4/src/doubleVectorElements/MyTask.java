package doubleVectorElements;

public class MyTask extends Thread {

    private int start;
    private int end;

    public MyTask(int id) {
        this.start = id * (Main.N) / (Main.P);
        this.end = Math.min((id + 1) * (Main.N) / (Main.P), (Main.N));
    }

    public void run() {
        int i;
        for (i = start; i < end; i++) {
            int val = Main.v[i];
            Main.v[i] = 2 * val;
        }
    }
}