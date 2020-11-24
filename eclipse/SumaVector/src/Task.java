
import java.util.ArrayList;
import java.util.Random;

public class Task extends Thread {
	private int id;
	
	public Task(int id) {
		this.id = id;
	}
	
	public void run() {
		int j;
		//genereaza aleator un numar k din [2, 10] //cate perechi sunt generate de threadul curent
		 Random rand = new Random();

		 // randomNum = rand.nextInt((max - min) + 1) + min;
		 
		 ArrayList<Integer> key_values = new ArrayList<Integer>();
		 
		 int k = rand.nextInt(10 - 2 + 1) + 2;
		 //System.out.println("De generat " + k + " perechi");
		 for (j = 0; j < k; j++) {
			 //genereaza o pereche (k, v)
			 int i = rand.nextInt(10 - 1 + 1) + 1;
			 int v = rand.nextInt(100 - 1 + 1) + 1;
			 System.out.println("[Thread " + id +"] Am generat perechea (" + i + "," + v + ")");
			 //citeste valoarea maxima anterioara pentru intrarea i
			 int old_value = Main.map.get(i - 1);
			 try {
				 Thread.sleep(1000);
			 }
			 catch(InterruptedException e) {
				 e.printStackTrace();
			 }
			 
			 synchronized(Main.object) {
				 if (v > old_value) {
					 Main.map.put(i - 1, v);
				 }
			 }
			 	 
		 }
		 
		 
	}
}

