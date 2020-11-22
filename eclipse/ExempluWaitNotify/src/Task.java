
public class Task extends Thread {
	private int id;
	
	public Task(int id) {
		this.id = id;
	}
	
	public void run() {
		
		if (id == 0) { //threadul 0 notifica thread-ul 1 la modificarea valorii lui a
			synchronized(Task.class) //sau pot sa pun monitor lock-ul pe un obiect oarecare din main
			{
				System.out.println("Hello din thread-ul 0");
				Main.a = 2;
				//Main.obj.
				Task.class.notify();
			}
				
		}
		else {
			synchronized(Task.class) 
			{
				try {
					while(Main.a == 0) {
						//Main.obj.
						Task.class.wait(); //asteapta actualizarea valorii lui a 
					}
				}
				catch(InterruptedException e) {
					e.printStackTrace();
				}
				System.out.println("Valoarea lui a este " + Main.a);
			}
			
		}	
	}
}

