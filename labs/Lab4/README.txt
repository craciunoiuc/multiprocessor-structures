Laborator 4 - Craciunoiu Cezar 343C1

Hardware

In partea de hardware s-a realizat schema circuitului dat in Falstad. A fost
inclus un fisier ce contine configuratia, ce trebuie incarcata pe site.
Deoarece, schema logicii secventiale ale unitatii de comanda nu a fost destul
de sugestiva pentru a fi transpusa in circuit, semnalele acci si reqi au fost
tratate drept intrari. Acestea pot fi controlate precum restul intrarilor,
prin switch-urile de pe figura.
Au fost incluse mai multe chenare cu etichete pentru a se evidentia
fiecare modul din circuit.

Software

In partea de software s-a realizat un algoritm de gasire a unei cai intre
un punct de start si unul de stop. Pe scurt, acesta suna astfel:
 - Punctul din stanga avanseaza pana la jumatate numai pe conexiuni directe,
 deoarece, oriunde ar ajunge la jumatate, inca se poate intalni cu punctul din
 partea dreapta.
 - La jumatate se verifica cel mai semnificativ bit al celor 2 numere. Daca
 este comun atunci conexiunea este directa, altfel inversa.
 - Se porneste din sens opus cu punctul de "finish". Pe fiecare nivel se
 verifica daca atunci cand s-a facut algerea pentru cel de "start" s-a trecut
 in reteaua superioara sau inferioara. Se alege in functie de asta conexiunea,
 pentru a incerca cele 2 sa se intalneasca.
 - Cand se ajunge in reteaua de 4x4 se verifica paritatea in acea retea,
 deoarece cele 2 rezultate mai difera printr-un singur bit.
 La fiecare etapa se aplica functia de shuffle, care realizeaza shuffle-ul pe
 diferite nivele ale retelei.
 Algoritmul de mai sus functioneaza a fost testat pentru retele pana in 16x16
 inclusiv (nu pentru toate valorile). Pentru retele mai mari nu am gasit schema
 pentru a verifica daca se comporta corespunzator, dar, avand in vedere
 caracterul general al acestuia, ar trebui sa functioneze.
 