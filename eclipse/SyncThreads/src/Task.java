
public class Task extends Thread {
	
	private int id;
	private int result;
	

	public Task(int id) {
		this.id = id;
	}
	
	public void run() {
		Main.t.set(id, id * id);
	}
	
	public int getResult() {
		return result;
	}
}
