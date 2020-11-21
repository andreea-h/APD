
public class Task extends Thread {
    private int id;
    private int result;

    public Task(int id) {
        this.id = id;
    }

    public void run() {
        result = id * id;
    }

    public int getResult() {
        return result;
    }

}