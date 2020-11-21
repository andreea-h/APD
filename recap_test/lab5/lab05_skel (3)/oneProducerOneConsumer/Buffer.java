package oneProducerOneConsumer;
/**
 * @author cristian.chilipirea
 *
 */
import java.util.concurrent.Semaphore;

public class Buffer {
	int a;
	Semaphore plin;
	Semaphore gol;
	
	public Buffer() {
		plin = new Semaphore(0);
		gol = new Semaphore(1);
	}
	
	void put(int value) {
		try {
			//System.out.println("put asteapta sa primeasca acces");
			gol.acquire();
			//System.out.println("S-a primit acces in put");
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
		//synchronized((Object)a) 
		{
			a = value;
		}
		
		//System.out.println("valoare lui a este: " + a);
		plin.release(); //perminte unui consumator sa ia o valoare, incrementare plin
	}

	int get() {
		int rez;
		try {
			//System.out.println("get asteapta sa primeasca acces");
			plin.acquire();
			//System.out.println("S-a primit acces in get");
		}
		catch(InterruptedException e) {
			e.printStackTrace();
		}
		//synchronized((Object)a) 
		{
			rez = a;
		}
			
		gol.release();
		
		return rez;
	}
}
