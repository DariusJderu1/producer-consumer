#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

       
#define PRODUCERS 2 
#define CONSUMERS 3 
#define ITEMS_PER_PRODUCER 10

int main() 
{
    // =====================================================
    // I Crearea pipe-ului

    // Un array de 2 int-uri care are rolul salvarii file descriptorilor
    // aferenti citirii si scrierii in pipe
    int fd[2];

    // Cerem kernel-ului sa faca intern un pipe si sa puna in array-ul initial
    // file descriptorii aferenti capetelor pipe ului, fd[0] - citire, fd[1] scriere
    // Daca a esuat cererea, oprim programul si dam un mesaj de eroare
    if(pipe(fd) == -1) 
    {
        perror("A avut loc o eroare la crearea pipe-ului");
        exit(1);
    }


    // =====================================================
    // II Crearea producatorilor
    
    // Fac un for care sa parcurga numarul de producatori (care sunt procese), ca sa ii creez
    for(int i = 0; i < PRODUCERS; ++i) 
    {
        // Cer kernel-ului sa cloneze procesul curent
        // prin fork si salvez process id ul returnat
        pid_t pid = fork();

        // Daca apare o eroare in clonarea procesului parinte de la apelul fork,
        // afisez un mesaj de eroare si termin programul.
        if(pid == -1)
        {
            perror("A aparut o eroare la clonarea procesului parinte cu fork()");
            exit(1);
        }

        // Daca ne aflam in procesul copil
        if(pid == 0) 
        {
            // Dupa clonarea procesului atat parintele cat si copilul
            // au acces la pipe deoarece la fork copilul are acces la 
            // tabela de file descriptori ai parintelui.

            // Inchid capatul de citire
            close(fd[0]);

            // Parcurg numarul valori pe care le va da fiecare producator
            for(int j = 0; j < ITEMS_PER_PRODUCER; ++j)
            {
                // Creez o valoare de pus in pipe
                int item = (i + 1) * 100 + j;

                // Scriu in pipe si salvez valorea returnata
                ssize_t bytes_written = write(fd[1], &item, sizeof(int));

                // Daca apare o eroare la scrierea in pipe opresc procesul si
                // afisez un mesaj
                if(bytes_written == -1) 
                {
                    perror("A aparut o eroare la scrierea in pipe");
                    exit(1);
                }

                // Daca nu se reuseste sa se scrie un int intreg in pipe,
                // opresc procesul si afisez un mesaj
                if(bytes_written != sizeof(int)) 
                {
                    printf("Un producer nu a reusit sa scrie un int intreg in pipe!\n");
                    exit(1);
                }

                // Afisez un mesaj pentru a urmari evolutia
                printf("Producatorul %d a scris valoarea %d in pipe.\n", i+1, item);

                // Incetinesc procesul pentru a observa mai bine scrierile si citirile
                sleep(1);
            }

            // Inchid capatul de scriere, intrucat am terminat cu el
            close(fd[1]);

            // Afisez un mesaj pentru a demonstra ca fiecare producator si-a terminat treaba
            printf("Producatorul %d a terminat de scris!\n", i+1);

            // Opresc complet procesul, dupa ce a terminat de scris
            exit(0);
        }
    }

    return 0;
}