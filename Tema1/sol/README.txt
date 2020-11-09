Tema1 APD

Student: Horovei Andreea-Georgiana
Grupa: 335CC

Generarea paralela de fractali folosind multimile Mandelbrot si Julia

	Pentru a reprezenta cele 2 multimi, Julia si Mandelbrot, am definit doua 
matrici ca variabile globale, alaturi de numarul de linii si de coloane pentru 
fiecare matrice. Inainte de a porni cele P thread-uri care se ocupa de realizarea
calculelor si scrierea rezultatelor, am calculat in main dimensiunile matricelor
asociate multimilor conform formulelor din enunt, am alocat matricile si am facut 
operatia de intializare a unei bariere folosita pentru sincronizarea operatiilor 
de calcul si scriere a multimilor in functia de thread.

	In cadrul functiei de thread prima operatie realizata este calcularea indicilor
de start si end care sunt asociati fiecarui thread, o pereche de indici pentru
multimea Julia si alta pentru Mandelbrot. 
	Calculul matricii care reprezinta o multime (result_julia respectiv result_man)
este paralelizat astfel incat fiecarui thread ii este 'destinata' o sectiune de 
coloane din matricea finala. Dupa ce sunt calculate dimensiunile matricelor asociate
celor 2 multimi (in functia main), in functia de thread sunt calculati indicii 
coloanelor de care se va ocupa fiecare thread: cele width_julia, respectiv 
width_man coloane, se impart intr-un mod aproximativ egal la P thread-uri, tinand
cont de faptul ca fiecare thread are un id de la 0 la P-1.
	Astfel, fiecare thread va itera pe cate o sectiune din matrice, data de indicii 
de coloana, fara a afecta operatiile celorlalte thread-uri. Pentru a calcula indicii 
de start si end pentru fiecare thread am folosit formulele din laborator.
	
	Thread-urile isi continua executia cu calculul multimii Julia, care se realizeaza 
la fel ca in implementarea secventiala, dar acum doar pentru coloanele care ii sunt 
asociate fiecarui thread. Cand toate thread-urile au executat calculele pentru coloanele 
din matrice care ii sunt asociate fiecaruia, rezultatul este scris in fisier de catre 
un singur thread (inainte de scriere in fisier am folosit o bariera pentru a astepta ca 
toate thread-urile sa execute operatiile prin care calculeaza elementele multimii). 
	In mod similar se face si calculul multimii Mandelbrot, respectand ordinea 
de calcul a elementelor multimii pe coloane si, apoi, operatia de scriere in fisier
a rezultatului dupa ce toate thread-urile au facut calculele pe sectiunea aferenta 
din matrice.

	In ceea ce priveste transformarea rezultatului din coordonate matematice in
coordonate ecran, am implementat generarea multimilor astfel incat rezultatul sa
fie dat direct in coordonate ecran, plasand elementul calculat pentru linia h si 
coloana w la pozitia [height - h - 1][w] in matrice.
	Toate cele P thread-uri create la inceput sunt folosite pentru a realiza 
calculele multimilor in paralel (mai intai calculul multimii Julia, apoi
Mandelbrot). La final, in main, se distruge bariera si se executa 
pthread_exit(NULL).

	Intr-o alta varianta de implementare la care m-am gandit si pe care o 
incarcasem si pe checker (ma refer la cea de-a patra submisie), am incercat sa 
realizez in paralel si alocarea matricilor care retin multimile, dar am realizat 
ulterior ca solutia era gresita intrucat la salvarea rezultatului in matrice 
(direct in coordonate ecran), se accesau zone de memorie care nu erau alocate inca
(ceea ce cred ca a generat seg fault, si rularea nu s-a putut executa cu succes).
De aceea, in varianta finala am realizat alocarea celor 2 matrici care retin 
rezultatul direct in functia main, folosind functia din schelet allocate_memory, 
dupa calcularea dimensiunilor matricelor, inainte de a porni cele P threaduri 
care se ocupa de calculul multimilor.
