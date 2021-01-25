Laborator 3 SM - Craciunoiu Cezar 343C1

Hardware
In partea de hardware s-a reprezentat circuitul folosind falstad
https://www.falstad.com/circuit/circuitjs.html
Pentru a se incarca fisierul se copiaza continutul sau si se introduce la
File->Import from Text.
S-au urmat figurile din pdf si s-au imbinat cele 3 circuite.
Nu s-au mai realizat si verificarile + tabelele de verificare pentru restul de
0.5 puncte.

Software
In partea de software s-a implementat un algoritm care sa simuleze reteaua
omega de tip 2^k. Programul citeste de la tastatura dimensiunea retelei, adica
pe k din 2^k. Apoi afiseaza posibilitatile de intrari si iesiri pentru
realizare a shuffle-ului. Utilizatorul este apoi rugat sa aleaga un punct
de start si stop si este afisat pentru fiecare pas daca etajul de comutatie
va realiza o conexiune directa sau inversa.

S-a ales utilizarea lui k in loc de n ca data de intrare, pentru a restrictiona
utilizatorul la puteri ale lui 2, fara a face aproximari. De asemenea, prin
utilizarea doar a puterilor lui 2, utilizatorul nu mai este nevoie sa introduca
toata lista de puncte de start si stop.

Programul este destul de direct, singurele dificultati intampinate au fost la
prelucrarea datelor si formatare, acesta fiind scris in C. In final s-a ajuns
la o versiune care sa indeplineasca cerintele initiale.