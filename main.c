#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
       
#define PRODUCERS 2 
#define CONSUMERS 3 
#define ITEMS_PER_PRODUCER 10

// Produce item in pipe
void produce_item(int* fd, int i, int j)
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
// Creaza procesele copil care sunt producatori
void producers(int* fd)
{
    // Fac un for, care apartine procesului principa,
    // care sa parcurga numarul de producatori (care sunt procese) si ii creeaza
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

            // Parcurg numarul valori pe care le va da producatorul
            for(int j = 0; j < ITEMS_PER_PRODUCER; ++j)
                produce_item(fd, i, j);

            // Inchid capatul de scriere, intrucat am terminat cu el
            close(fd[1]);

            // Afisez un mesaj pentru a demonstra ca fiecare producator si-a terminat treaba
            printf("Producatorul %d a terminat de scris!\n", i+1);

            // Opresc complet procesul, dupa ce a terminat de scris
            // Fac asta ca sa nu mai continue fara motiv sa execute codul
            exit(0);
        }
    }
}

// Consuma item din pipe
void consume_item(ssize_t bytes_read, int i, int item)
{
    // Daca nu s a citit un int intreg opresc proces si afisez mesaj sugestiv
    if(bytes_read != sizeof(int)) 
    {
        printf("Un consumer nu a reusit sa citeasca un int intreg din pipe!\n");
        exit(1);
    }

    // Semnalez ca consumatorul a preluat valoarea din pipe
    printf("Consumatorul %d a citit valoarea %d din pipe.\n", i+1, item);
}
// Creeaza procesele copil pentru consumatori
void consumers(int* fd)
{
    // Fac un for, care apartine procesului principa,
    // care sa parcurga numarul de consumatori (care sunt procese) si ii creeaza
    for(int i = 0; i < CONSUMERS; ++i) 
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

            // Inchid capatul de scriere
            close(fd[1]);

            // Declarare item de citit
            int item;
            
            // Declar variabila care salveaza return ul lui read
            ssize_t bytes_read;

            // Cat timp citirea din pipe este executata cu succes
            while((bytes_read = read(fd[0], &item, sizeof(int))) > 0) 
                consume_item(bytes_read, i, item);

            // Daca a esuat vreun read sys call, opresc proces si afisez mesaj sugestiv
            if(bytes_read == -1) 
            {
                perror("A aparut o eroare la citirea din pipe");
                exit(1);
            }

            // Kernel-ul stie cand nu mai exista niciun proces care are deschis in tabela sa de descriptori capatul de scriere al pipe-ului,
            // de asta read la un moment dat nu-si mai foloseste caracterul "blocant" si returneaza 0
            // Semnalez ca un anumit consumator nu mai are cum si ce sa mai citeasca
            printf("Consumatorul %d si-a terminat treaba! Nu mai exista date de consumat sau vreun proces producator cu capatul de scriere deschis.\n", i+1);

            // Inchid capatul de citire, intrucat am terminat cu el
            close(fd[0]);

            // Opresc complet procesul, dupa ce a terminat de scris
            // Fac asta ca sa nu mai continue fara motiv sa execute codul
            exit(0);
        }
    } 
}

// Colectare status procese copil
void terminare_procese_copil()
{
    // Parcurg numarul de procese create in total si 
    // folosesc wait sys call ca sa comunic kernelului
    // ca parintele a colectat statusul copilului terminat,
    // ca sa poata sa fie scos din tabela de procese definitiv
    // wait blocheaza un proces pana se termina UN COPIL
    for(int i = 0; i < PRODUCERS + CONSUMERS; ++i)
        wait(NULL);
}

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
    producers(fd);
    
    // =====================================================
    // III Crearea consumatorilor
    consumers(fd);

    // =====================================================
    // IV Inchidere capete pipe
    // Inchid cele doua capete ale pipe ului, mai ales pentru fd[1]
    // este important, ca read sa recunoasca ca nu mai poate citi nimic
    // pentru ca nimic nu mai poate fi adaugat
    close(fd[0]);
    close(fd[1]);

    // =====================================================
    // V Colectare status code procese copil
    // Colectez status copiilor pentru a evita procesele zombie
    terminare_procese_copil();

    return 0;
}

// In implementarea cu procese si pipe-uri, buffer-ul partajat este 
// buffer-ul intern al pipe-ului, administrat de kernel. 
// Procesele nu il accesează direct, ci doar prin read() si write().