#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define N 50           
#define PRODUCERS 2 
#define CONSUMERS 3 

int main() 
{
    // I Crearea pipe-ului

    // Un array de 2 int-uri care are rolul salvarii file descriptorilor
    // aferenti citirii si scrierii in pipe
    int fd[2];

    // Cerem kernel-ului sa faca intern un pipe si sa puna in array-ul initial
    // file descriptorii aferenti capetelor pipe ului, fd[0] - citire,
    // fd[1] scriere
    // Daca a esuat cererea, oprim programul si dam un mesaj de eroare
    if(pipe(fd) == -1) 
    {
        printf("A avut loc o eroare la crearea pipe-ului!");
        exit(1);
    }

    
    
    return 0;
}