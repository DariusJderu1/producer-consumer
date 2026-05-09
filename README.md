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