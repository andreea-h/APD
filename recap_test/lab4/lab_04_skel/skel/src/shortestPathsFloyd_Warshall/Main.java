package shortestPathsFloyd_Warshall;
/**
 * @author cristian.chilipirea
 *
 */

import java.util.concurrent.CyclicBarrier;

public class Main {

	public static int M = 9;
	public static int N = 5;
	public static int P = 4;
	
	public static int graph1[][] = { { 0, 1, M, M, M }, 
	          { 1, 0, 1, M, M }, 
	          { M, 1, 0, 1, 1 }, 
	          { M, M, 1, 0, M },
	          { M, M, 1, M, 0 } };
	
	public static CyclicBarrier barrier;
	
	public static void main(String[] args) {
		barrier = new CyclicBarrier(P);
		
		int graph[][] = { { 0, 1, M, M, M }, 
				          { 1, 0, 1, M, M }, 
				          { M, 1, 0, 1, 1 }, 
				          { M, M, 1, 0, M },
				          { M, M, 1, M, 0 } };
		
		// Parallelize me (You might want to keep the original code in order to compare)
		for (int k = 0; k < 5; k++) {
			for (int i = 0; i < 5; i++) {
				for (int j = 0; j < 5; j++) {
					graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
				}
			}
		}

		
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				System.out.print(graph[i][j] + " ");
			}
			System.out.println();
		}
		
		System.out.println("-------------------");
		
		MyThread[] threads = new MyThread[P];
		int i;
		for (i = 0; i < P; i++) {
			threads[i] = new MyThread(i);
			threads[i].start();
		}
		
		for (i = 0; i < P; i++) {
			try {
				threads[i].join();
			}
			catch(InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		for (i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				System.out.print(graph1[i][j] + " ");
			}
			System.out.println();
		}
		boolean check = true;
		for (i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				if (graph1[i][j] != graph[i][j]) {
					check = false;
					break;
				}
			}
		}
		if (check == false) {
			System.out.println("Gresit");
		}
		else {
			System.out.println("Corect");
		}
		
	}
}
