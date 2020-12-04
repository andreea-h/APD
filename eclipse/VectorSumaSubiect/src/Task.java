import java.util.concurrent.BrokenBarrierException;

public class Task extends Thread {
	private int id;
	
	public Task(int id) {
		this.id = id;
	}
	
	public void run() {
		int start = id * (int) Math.ceil((double) Main.N / Main.NR_THREADS);
		int end = Math.min(Main.N, (id + 1) * (int) Math.ceil((double) Main.N / Main.NR_THREADS));
		
		int i;
		for (i = 0; i < Main.N; i++) {
			Main.partial_sum[id] += Main.v[i];
		}
		
		try {
			Main.barrier.await();
		}
		catch(BrokenBarrierException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}
		
		if (id == 0) {
			for (i = 0; i < Main.NR_THREADS; i++) {
				Main.suma += Main.partial_sum[i];
			}
			System.out.println("Suma este: " + Main.suma);
		}
	}
}
