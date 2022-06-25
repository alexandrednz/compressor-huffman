#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HEOF 256

typedef struct arv Arv;

struct arv{
    int simbolo;
    int freq;

    struct arv* esq;
    struct arv* dir;
};

Arv* arv_cria(int c, int freq, Arv* sae, Arv* sad) {
    Arv* p = (Arv*) malloc(sizeof(Arv));
    p->simbolo = c;
    p->freq = freq;
    p->esq = sae;
    p->dir = sad;
    
    return p;
}

void arv_imprime (Arv* a){
    if(a != NULL){
        printf("<%c %d> ", a->simbolo, a->freq);
        arv_imprime(a->esq);
        arv_imprime(a->dir);
    }
}

void troca(Arv **a, Arv **b){
    Arv *aux;
    aux = *a;
    *a = *b;
    *b = aux;
}

int indiceFilhoEsquerdo(int i){
    return (i * 2) + 1;
}

int indiceFilhoDireito(int i){
    return (i * 2) + 2;
}

void desce(Arv *v[], int n, int i){
    int esquerdo = indiceFilhoEsquerdo(i);
    int direito = indiceFilhoDireito(i);
    int menor = i;

    if (esquerdo >= 0 && esquerdo < n && v[esquerdo]->freq < v[i]->freq){
        menor = esquerdo;
    }

    if (direito >= 0 && direito < n && v[direito]->freq < v[menor]->freq){
        menor = direito;
    }

    if (menor != i){
        troca(&v[i], &v[menor]);
        desce(v, n, menor);
    }
}

void constroiMinHeap(Arv *v[], int n){
    for (int i = (n - 2)/2; i >= 0; i--){
        desce(v, n, i); 
    }
}

void insere_heap(Arv *h[], int *n, Arv* novo){
    h[*n] = novo;
    *n = *n + 1;
    constroiMinHeap(h, *n);
}

Arv* extrai_min(Arv *h[], int *n){
    Arv* min = h[0];
    troca(&h[0], &h[*n-1]);
    *n = *n - 1;
    desce(h, *n, 0);

    return min;
}

Arv* cria_arv_huffman(Arv *h[], int *n){
    Arv* novo;
    while(*n > 1){
        novo = (Arv*) malloc(sizeof(Arv));
        novo->esq = extrai_min(h, n);
        novo->dir = extrai_min(h, n);
        novo->freq = (novo->esq->freq) + (novo->dir->freq);
        insere_heap(h, n, novo); 
    }
    return extrai_min(h, n);
}

// argv[1] = c ou d (compactar ou descompactar)
// argv[2] = arquivo de entrada
// argv[3] = arquivo de saida

int main(int argc, char *argv[]){
    
    if(strcmp(argv[1], "c") == 0){
        FILE *f;
        char c; // caractere lido do arquivo
        int freq[257]; // tabela de frequencias

        f = fopen(argv[2], "r");

        // construindo a tabela de frequencias
        for(int i = 0; i < 257; i++){
            freq[i] = 0;
        }

        do{
            c = fgetc(f);

            if(!feof(f)){
                freq[c] = freq[c] + 1;
            }
        } while(!feof(f));

        // construindo a minheap a partir da tabela de frequencias
        Arv *minheap[257];
        int n; // tamanho da minheap
        minheap[0] = arv_cria(HEOF, 0, NULL, NULL); // adicionando EOF artificialmente a minheap
        n = 1;

        for(int i = 0; i < 257; i++){
            if(freq[i] != 0){
                minheap[n] = arv_cria(i, freq[i], NULL, NULL);
                n++;
            }
        }

        constroiMinHeap(minheap, n);

        // construindo arvore de huffman utilizando a minheap
        Arv *huffman;
        huffman = cria_arv_huffman(minheap, &n);

        arv_imprime(huffman);
        
   









    }
    else if(strcmp(argv[1], "d") == 0){
        printf("argv[1] eh d");
    }

    return 0;
}