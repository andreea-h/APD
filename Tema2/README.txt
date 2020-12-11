Cerinta 8: Simple Maintenance
Pentru implementarea sincronizarii am folosit 2 variabile de tipul Semaphore, una
prin care se asigura faptul ca pe banda 0 pot circula x masini la un moment dat, respectiv un semafor care controleaza circulatia a x masini pe banada 1 de mers (a.i. nu va fi depasit numarul de x masini care mers la un mmoment dat pe o anume banda). La initializare, semaforul aferent sensului 0 este initializat cu valoarea x, iar semaforul aferent sensului 1 este initializat cu valoarea 0 
(primele masini pentru care se permie trecerea sunt cele de pe sensul 0). 
Am folosit 2 varibile de tipul AtomicInteger care au afisat "passed the bottleneck", cate una pentru fiecare sens de mers. Atunci cand numarul de masini 
care au trecut, mergand pe unul dintre sensuri, devine multiplu al lui x, sunt 
notificate masinile de pe sensul opus, aflate la acel moment in asteptare. Dupa ce ies din asteptare, masinile verifica daca pot face acquire() pe variabila semafor aferenta sensul corespunzator de mers (a.i. sa nu se depasesca numarul de x maximi care merg pe un sens la un moment dat). Daca s-a putut face acquire(), masina afiseaza mesajul aferent de trecere si incrementeaza variabila 
care contorizeaza numarul de masini de pe sensul aferent care au realizat trecerea. Daca numarul de masini care au realizat trecerea pe sensul curent devine multiplu al lui x, se face release cu valoarea x pe semaforul aferent sensului opus de mers a.i. sa se permita doar circulatia in mod alternativ a cate exact x masini pe o singura banda.
