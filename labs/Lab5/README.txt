Laborator 5 - Cezar Craciunoiu 343C1

Pentru a se testa perfomantele intr-o problema cu MPI s-a considerat urmatorul
studiu de caz. S-a calculat in mod paralel urmatorul numar prim pentru un array
de numere. Pentru a se folosi cat mai mult din capacitatile MPI-ului s-a incercat
sa se varieze abordarea.

La inceput s-a trimis prin Broadcast dimensiunea inputului pentru a putea aloca
memoria. Apoi, folosindu-se Scatter si Gather s-au dat elementele catre task-uri
pentru a fi rulate si apoi rezultatele returnate la master. In final, master-ul
afiseaza sau nu output-ul.

Pentru a se configura rularea au fost folosite mai multe define-uri puse la
inceput de fisier.
Makefile-ul si scriptul de rulare sunt configurate pentru cluster-ul fep.

Se observa ca timpul de rulare scade o data cu cresterea numarului de task-uri,
dar, deoarece overhead-ul este mare, dupa o anumita valoare timpul incepe sa
creasca. 