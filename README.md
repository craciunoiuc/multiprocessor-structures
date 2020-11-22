# Structuri Multiprocessor - Tema 1&2
## Aplicare de filtru Brightness pe imagini
### Cezar Craciunoiu 343C1

In tema s-au abordat cele 4 metode de implementare diferite cerute (MPI, openMP,
pthread si varianta hibrida MPI + openMP). Fiecare implementare va fi explicata
mai jos in parte. Pentru citirea imaginilor png s-a folosite biblioteca lodePNG,
ce ofera cod simplu pentru citirea si scrierea de imagini png. 

#### OpenMP
Pentru varianta paralela OpenMP s-a implementat mai intai varianta seriala.
S-a observat apoi ca cea mai mare parte din timp este petrecuta in aplicarea
efectiva a filtrului, dar si in codificare/decodificare. Pentru a se creste
viteza de executie, s-au paralelizat buclele for ce se ocupa cu codificarea si
decodificarea, cat si bucla ce se ocupa de aplicarea filtrului de brightness.
Se obtine astfel o rulare eficienta cu o modificare de cod minima.

#### MPI
Pentru varianta MPI, s-a realizat paralelizarea la nivel de numar de imagini,
nu de randuri din imagine. Astfel ca, se evita foarte mult overhead de trimitere
a datelor. Dezavantajul este ca, in unele cazuri, aceasta varianta este mai
inceata decat rularea efectiva intr-un singur task (imagini putine si mari).
In cazurile cu imagini multe si de dimensiune mica (sub 1MB) aceasta
ajunge sa fie aproape la fel de rapida ca restul. Un alt dezavantaj al acestei
implementari este ca nu se poate rula cu un numar mai mare de procese decat
imaginile, dar, in general, acest lucru nu este o problema.

#### Pthread
Pentru varianta Pthread, s-a abordat paralelizarea in aceleasi locuri ca in
cazul OpenMP. Codul este putin mai eficient pe partea de aplicare a filtrului,
dar overhead-ul de copiere a datelor si impartirea a memoriei, etc, face
program-ul sa fie putin mai incet decat varianta OpenMP.

#### Hibrid
Dupa cum s-a observat, codul pentru varianta hibrida este acelasi cu cel MPI.
Diferenta vine la compilare. Codul MPI este compilat fara `-fopenmp`, deci
`#pragma`-urile sunt ignorate. In partea hibrida se paralelizeaza si la nivel
de thread encoding/decoding-ul. Astfel ca un task cu 4 imagini poate sa imparta
cate o imagine la 4 thread-uri. La fel si in partea de aplicare a filtrelor.
In rest codul este la fel ca cel de MPI.

### Bibliografie
LodePNG - https://github.com/lvandeve/lodepng