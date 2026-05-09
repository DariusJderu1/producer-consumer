# Producer-Consumer cu procese si pipe-uri

Acest proiect implementeaza problema Producer-Consumer folosind procese si un pipe anonim.

Implementarea foloseste:

- `pipe()` pentru crearea canalului de comunicare dintre procese;
- `fork()` pentru crearea proceselor producator si consumator;
- `write()` pentru scrierea valorilor produse in pipe;
- `read()` pentru citirea valorilor din pipe;
- `close()` pentru inchiderea capetelor de pipe nefolosite;
- `wait()` pentru colectarea statusului proceselor copil.

## Structura

Programul creeaza:

- `PRODUCERS` procese producator;
- `CONSUMERS` procese consumator;
- un proces parinte care coordoneaza crearea proceselor si inchiderea pipe-ului.

Pipe-ul este folosit ca buffer comun intre producatori si consumatori. Buffer-ul nu este implementat manual in program, ci este buffer-ul intern al pipe-ului, administrat de kernel.

## Diferenta fata de alte variante de rezolvare

In varianta cu thread-uri, buffer-ul este de obicei implementat manual in program, de exemplu printr-un array circular. Pentru sincronizare sunt necesare mecanisme precum mutex-uri si variabile de conditie, astfel incat producatorii sa nu scrie intr-un buffer plin, consumatorii sa nu citeasca dintr-un buffer gol si mai multe thread-uri sa nu modifice simultan aceleasi date.

In aceasta implementare, sincronizarea este realizata prin pipe. Daca pipe-ul este gol, `read()` blocheaza consumatorul pana apar date sau pana cand nu mai exista niciun capat de scriere deschis. Daca pipe-ul este plin, `write()` poate bloca producatorul pana se elibereaza spatiu. Astfel, cazurile de underflow si overflow sunt tratate de comportamentul blocant al pipe-ului.

De asemenea, procesele nu acceseaza direct acelasi buffer din memoria programului. Ele comunica prin pipe, iar buffer-ul este administrat de kernel. Din acest motiv, nu este necesar un mutex pentru protejarea unui buffer manual.

## Functionare

Consumatorii sunt creati primii si inchid capatul de scriere al pipe-ului. Ei citesc valori din pipe pana cand `read()` returneaza `0`, ceea ce inseamna ca nu mai exista date si niciun proces nu mai are deschis capatul de scriere.

Producatorii inchid capatul de citire al pipe-ului. Fiecare producator scrie `ITEMS_PER_PRODUCER` valori intregi in pipe. Valorile sunt generate astfel incat sa se poata identifica producatorul care le-a scris.

Procesul parinte inchide ambele capete ale pipe-ului dupa crearea copiilor si apoi asteapta terminarea tuturor proceselor copil.

## Compilare

```bash
gcc -Wall -Wextra -pedantic main.c -o producer_consumer
```

## Rulare

```bash
./producer_consumer
```

La rulare, programul afiseaza valorile scrise de producatori si valorile citite de consumatori.