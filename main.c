#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define HEOF 256

typedef struct arv Arv;

struct arv{
    int simbolo;
    int freq;
    int ehFolha;

    struct arv* pai;
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

void arv_imprime (Arv* a, int alfabeto[], int* k){
    if(a != NULL){
        if(a->esq == NULL && a->dir == NULL) {
            printf("1\n");
            alfabeto[(*k)] = a->simbolo;
            (*k)++;
            printf("<%c %d>\n", a->simbolo, a->freq);
        } else {
            printf("0\n");
        }
        arv_imprime(a->esq, alfabeto, k);
        arv_imprime(a->dir, alfabeto, k);
    }
}

void arv_imprime_2(Arv* a) {
    if(a != NULL) {
        if(a->ehFolha) {
            printf("%c\n", a->simbolo);
        }

        arv_imprime_2(a->esq);
        arv_imprime_2(a->dir);
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

void reconstroi_arv_huffman(Arv** a, Arv* n, Arv** x) {
    if(*a == NULL) {
        *a = n;

        if(!n->ehFolha) {
            *x = n;
        }
    } else {
        if(n->ehFolha) {
            if((*x)->esq == NULL) {
                (*x)->esq = n;
                n->pai = *x;
                printf("Inserido: ***%c\n", (*x)->esq->simbolo);
            } else if((*x)->dir == NULL) {
                (*x)->dir = n;
                n->pai = *x;
                printf("Inserido: ***%c\n", (*x)->dir->simbolo);
            } else {
                *x = (*x)->pai;
                printf("Chamando recursão...\n");
                reconstroi_arv_huffman(a, n, x);
                printf("Finalizando recursão...\n");
            }
        } else {
            if((*x)->esq == NULL) {
                (*x)->esq = n;
                n->pai = *x;
                *x = n;
            } else if((*x)->dir == NULL) {
                (*x)->dir = n;
                n->pai = *x;
                *x = n;
            } else {
                *x = (*x)->pai;
                printf("Chamando recursão...\n");
                reconstroi_arv_huffman(a, n, x);
                printf("Finalizando recursão...\n");
            }
        }
    }
}

int busca_codigo(Arv** a, Arv** p, int b) {
    if(b == 0) {
        *p = (*p)->esq;
    } else if (b == 1) {
        *p = (*p)->dir;
    }

    if((*p)->ehFolha) {
        int simbolo = (*p)->simbolo;
        *p = *a;
        return simbolo;
    }
    
    return -1;
}

void gera_codigo_huffman(Arv* a, char b[], int* n, char c[][256]) {
    if(a != NULL){
        if(a->dir == NULL && a->esq == NULL) {
            b[++(*n)] = '\0';

            if(b[0] == '\0') {
                strcpy(c[(int) a->simbolo], "0");
            } else {
                strcpy(c[(int) a->simbolo], b);
            }

            printf("<%c %d> : %s\n", a->simbolo, a->freq, c[(int) a->simbolo]);
        }

        if(a->esq != NULL) {
            b[++(*n)] = '0';
        }

        gera_codigo_huffman(a->esq, b, n, c);
        
        if(a->dir != NULL) {
            b[(*n)] = '1';
        }

        gera_codigo_huffman(a->dir, b, n, c);

        --(*n);
    }
}

void insere_percurso_preordem(Arv* a, char* x, int* c, FILE* h) {
    if(a != NULL){
        if(a->esq == NULL && a->dir == NULL) {
            if(*c >= 0) {
                *x = *x | (1 << *c);
                (*c)--;
            } else {
                fwrite(x, 1, 1, h);

                *x = *x << 8;
                *c = 7;

                *x = *x | (1 << *c);
                (*c)--;
            }
        } else {
            if(*c >= 0) {
                (*c)--;
            } else {
                fwrite(x, 1, 1, h);

                *x = *x << 8;
                *c = 7;

                (*c)--;
            }
        }

        insere_percurso_preordem(a->esq, x, c, h);
        insere_percurso_preordem(a->dir, x, c, h);
    }
}

void insere_texto_codificado(int caracter, char* x, int* c, char codigos[][256], FILE* h) {
    char codigo[256];
    strcpy(codigo, codigos[caracter]);

    for(int i = 0; i < strlen(codigo); i++) {
        if(*c >= 0) {
            if(codigo[i] == '1') {
                *x = *x | (1 << *c);
                (*c)--;
            } else {
                (*c)--;
            }
        } else {
            fwrite(x, 1, 1, h);
            *x = *x << 8;
            *c = 7;

            if(codigo[i] == '1') {
                *x = *x | (1 << *c);
                (*c)--;
            } else {
                (*c)--;
            }
        }
    }
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
                freq[(int) c] = freq[(int) c] + 1;
            }
        } while(!feof(f));

        fclose(f);

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

        unsigned int tamanho_alfabeto = n;

        // construindo arvore de huffman utilizando a minheap
        Arv *huffman;
        huffman = cria_arv_huffman(minheap, &n);

        int alfabeto[257];
        int k = 0;
        arv_imprime(huffman, alfabeto, &k);

        printf("\n");

        char codigo[256];
        int posicao_codigo = -1;
        char codigos[257][256];
        
        gera_codigo_huffman(huffman, codigo, &posicao_codigo, codigos);
   
        FILE *h;
        h = fopen(argv[3], "w");

        fwrite(&tamanho_alfabeto, 2, 1, h);

        for(int i = 0; i < k; i++)
            fwrite(&alfabeto[i], 2, 1, h);

        char preordem_texto;
        preordem_texto = preordem_texto << 8;

        int posicao_preordem_texto = 7;

        insere_percurso_preordem(huffman, &preordem_texto, &posicao_preordem_texto, h);

        f = fopen(argv[2], "r");

        char caracter_lido;

        while(!feof(f)) {
            caracter_lido = fgetc(f);
            insere_texto_codificado((int) caracter_lido, &preordem_texto, &posicao_preordem_texto, codigos, h);
        }

        insere_texto_codificado(HEOF, &preordem_texto, &posicao_preordem_texto, codigos, h);

        fclose(f);

        if(posicao_preordem_texto >= -1)
            fwrite(&preordem_texto, 1, 1, h);

        fclose(h);
    }
    else if(strcmp(argv[1], "d") == 0){
        FILE* f;
        FILE* o;

        f = fopen(argv[2], "r");
        o = fopen(argv[3], "w");

        int tamanho_alfabeto;
        fread(&tamanho_alfabeto, 2, 1, f);

        short int alfabeto[tamanho_alfabeto];

        for(int i = 0; i < tamanho_alfabeto; i++) {
            fread(&alfabeto[i], 2, 1, f);
        }

        for(int i = 0; i < tamanho_alfabeto; i++) {
            printf(">>>>%d\n", (int)alfabeto[i]);
        }

        printf(">>>>%d\n", alfabeto[0]);

        int posicao_no_byte, quantidade_de_uns = 0;
        unsigned int bit;
        char byte;
        Arv* huffman = NULL;
        Arv* aux = NULL;

        while(quantidade_de_uns < tamanho_alfabeto) {
            posicao_no_byte = 7;

            fread(&byte, 1, 1, f);

            while((posicao_no_byte >= 0) && (quantidade_de_uns < tamanho_alfabeto)) {
                bit = (byte >> posicao_no_byte) & 1;

                Arv* novo;
                novo = (Arv*) malloc(sizeof(Arv));
                novo->pai = NULL;
                novo->esq = NULL;
                novo->dir = NULL;
                
                if(bit == 1) {
                    novo->simbolo = alfabeto[quantidade_de_uns];
                    printf("++++++++++++%u\n", alfabeto[quantidade_de_uns]);
                    novo->ehFolha = 1;
                    reconstroi_arv_huffman(&huffman, novo, &aux);
                    quantidade_de_uns++;
                } else {
                    novo->ehFolha = 0;
                    reconstroi_arv_huffman(&huffman, novo, &aux);
                }

                printf("%d --> %p\n", posicao_no_byte, (void*) aux);

                posicao_no_byte--;
            }
        }

        Arv* posicao_arvore = huffman;
        int text_end = 0;

        while(!text_end) {
            if(posicao_no_byte < 0) {
                fread(&byte, 1, 1, f);
                posicao_no_byte = 7;
            }
            
            while(posicao_no_byte >= 0) {
                bit = (byte >> posicao_no_byte) & 1;

                int caracter = busca_codigo(&huffman, &posicao_arvore, bit);

                

                if(caracter != -1 && caracter != 256) {
                    fprintf(o, "%c", (char) caracter);
                }   

                if(caracter == 256) {
                    text_end = 1;
                    break;
                }

                posicao_no_byte--;
            }
        }

        // printf("Procurando... achei o %c\n", huffman->dir->simbolo);
        arv_imprime_2(huffman);

        fclose(f);
        fclose(o);
    }

    return 0;
}