package shortestPathsFloyd_Warshall;

import java.util.concurrent.BrokenBarrierException;

public class MyThread extends Thread {
	
	private int id;
	
	public MyThread(int id) {
		this.id = id;
	}
	
	public void run() {
		
		int start = (int) (id * (double)(Main.N) / (Main.P));
		int end = (int) Math.min((id + 1) * (double)(Main.N) / (Main.P), Main.N);
		
		
		for (int k = 0; k < 5; k++) {
			for (int i = start; i < end; i++) {
				for (int j = 0; j < 5; j++) {
					Main.graph1[i][j] = Math.min(Main.graph1[i][k] + Main.graph1[k][j], Main.graph1[i][j]);
				}
			}
			//bariera
			//inainte de a trece la k+1 trebuie sa stiu ca s-au calculat toate elementele pentru k
			try {
				Main.barrier.await();//pentru a putea calcula [i][j] trebuie calculate toate elementele de forma [i][k] si [k][j]
			}
			catch(InterruptedException e) {
				e.printStackTrace();
			}
			catch(BrokenBarrierException e) {
				e.printStackTrace();
			};
			
		}
		
		
		
	}

}
