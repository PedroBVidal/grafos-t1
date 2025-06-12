#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"



//-----------------------------------
// Função auxiliar: cria cópia da string
char *copia_str(const char *s) {
    char *c = malloc(strlen(s) + 1);
    if (c == NULL) {
        fprintf(stderr, "Erro ao alocar string\n");
        exit(1);
    }
    strcpy(c, s);
    return c;
}
//-----------------------------------
// Função auxiliar: busca um vértice na lista de adjacência
lista_adjacencia *encontrar_vertice(grafo *g, const char *nome) {
    for (lista_adjacencia *v = g->l; v != NULL; v = v->proxima) {
        if (strcmp(v->nome_nodo_ref, nome) == 0)
            return v;
    }
    return NULL;
}

//-----------------------------------
// Função auxiliar: adiciona vértice, se não existir
lista_adjacencia *adicionar_vertice(grafo *g, const char *nome) {
    lista_adjacencia *v = encontrar_vertice(g, nome);
    if (v != NULL) return v;

    lista_adjacencia *novo = malloc(sizeof(lista_adjacencia));
    novo->nome_nodo_ref = copia_str(nome);
    novo->lista_vizinhos = NULL;
    novo->proxima = g->l;
    g->l = novo;
    return novo;
}

//-----------------------------------
// Função auxiliar: adiciona vizinho ao vértice
void adicionar_vizinho(lista_adjacencia *v, const char *vizinho_nome, int peso) {
    vizinho *novo = malloc(sizeof(vizinho));
    novo->nome_nodo = copia_str(vizinho_nome);
    novo->peso = peso;
    novo->prox_vizinho = v->lista_vizinhos;
    v->lista_vizinhos = novo;
}

//-----------------------------------
// Função principal: adiciona uma aresta
void adicionar_aresta(grafo *g, char *v1, char *v2, int peso) {
    lista_adjacencia *vert1 = adicionar_vertice(g, v1);
    lista_adjacencia *vert2 = adicionar_vertice(g, v2);
    
    adicionar_vizinho(vert1, v2, peso);
    adicionar_vizinho(vert2, v1, peso);
}

void dfs(lista_adjacencia *v, char **visitados, grafo *g) {
    if (v == NULL) return;

    for (int i = 0; visitados[i] != NULL; i++) {
        if (strcmp(visitados[i], v->nome_nodo_ref) == 0)
            return; // já visitado
    }

    // Marcar como visitado
    int i;
    for (i = 0; visitados[i] != NULL; i++);
    visitados[i] = v->nome_nodo_ref;

    // Recursivamente visitar os vizinhos
    for (vizinho *vz = v->lista_vizinhos; vz != NULL; vz = vz->prox_vizinho) {
        lista_adjacencia *viz = encontrar_vertice(g, vz->nome_nodo);
        dfs(viz, visitados, g);
    }
}

int indice_vertice(grafo *g, const char *nome) {
    int i = 0;
    for (lista_adjacencia *v = g->l; v != NULL; v = v->proxima, i++) {
        if (strcmp(v->nome_nodo_ref, nome) == 0)
            return i;
    }
    return -1;
}


grafo *le_grafo(FILE *f){
  grafo *g = malloc(sizeof(struct grafo));
  g->l = NULL;
  //inicializar_grafo(g);
   
  char linha[TAMANHO_MAX_LINHA];
  char nome_grafo[TAMANHO_MAX_LINHA];
  int primeira_linha = 1;
  while (fgets(linha, sizeof(linha), f)) {
      // Remove quebra de linha
      linha[strcspn(linha, "\n")] = 0;
      
      // Ignora comentários e linhas vazias
      if (linha[0] == '/' && linha[1] == '/' || linha[0] == '\0') {
          continue;
      }
      
      // A primeira linha não comentada é o nome do grafo
      if (primeira_linha) {
          g->nome = copia_str(linha); // <--- esta linha é essencial
          //printf("Nome do grafo: %s\n", g->nome);
          primeira_linha = 0;
          continue;
      }

      // Analisa aresta

      char vertice1[TAMANHO_MAX_LINHA], vertice2[TAMANHO_MAX_LINHA]; 
      int peso = 0;
      // Tenta ler dois vértices e um peso
      if (sscanf(linha, "%s -- %s %d", vertice1, vertice2, &peso) == 3) {
          //printf("Aresta: %s -- %s, Peso: %d\n", vertice1, vertice2, peso);
          adicionar_aresta(g, vertice1, vertice2, peso);
      }
      // Tenta ler apenas dois vértices (peso opcional)
      else if (sscanf(linha, "%s -- %s", vertice1, vertice2) == 2) {
          //printf("Aresta: %s -- %s, Peso: %d\n", vertice1, vertice2, peso);
          adicionar_aresta(g, vertice1, vertice2, 0);
      } 
      // Vértice isolado
      else {
        //printf("Vértice isolado: %s\n", linha);
        adicionar_vertice(g, linha);
      }
  } 
  return g;

}

//------------------------------------------------------------------------------
// desaloca toda a estrutura de dados alocada em g
//
// devolve 1 em caso de sucesso e 0 em caso de erro

unsigned int destroi_grafo(grafo *g) {
    if (g == NULL) return 0;

    lista_adjacencia *v = g->l;
    while (v != NULL) {
        lista_adjacencia *prox_v = v->proxima;

        // Libera lista de vizinhos
        vizinho *vz = v->lista_vizinhos;
        while (vz != NULL) {
            vizinho *prox_vz = vz->prox_vizinho;
            free(vz->nome_nodo);
            free(vz);
            vz = prox_vz;
        }

        free(v->nome_nodo_ref);
        free(v);
        v = prox_v;
    }

    free(g->nome);
    free(g);

    return 1;
}

//------------------------------------------------------------------------------
// devolve o nome de g

char *nome(grafo *g) {
    return g->nome;
}


//------------------------------------------------------------------------------
// devolve 1 se g é bipartido e 0 caso contrário

#include <stdbool.h>

unsigned int bipartido(grafo *g) {
    int n = n_vertices(g);
    int *cores = malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) cores[i] = -1;

    // Vetor com ponteiros para acesso rápido por índice
    lista_adjacencia **vertices = malloc(n * sizeof(lista_adjacencia *));
    int idx = 0;
    for (lista_adjacencia *v = g->l; v != NULL; v = v->proxima) {
        vertices[idx++] = v;
    }

    // BFS para cada componente
    for (int i = 0; i < n; i++) {
        if (cores[i] != -1) continue;

        int *fila = malloc(n * sizeof(int));
        int ini = 0, fim = 0;

        fila[fim++] = i;
        cores[i] = 0;

        while (ini < fim) {
            int atual = fila[ini++];
            lista_adjacencia *v = vertices[atual];

            for (vizinho *vz = v->lista_vizinhos; vz != NULL; vz = vz->prox_vizinho) {
                int j = indice_vertice(g, vz->nome_nodo);
                if (cores[j] == -1) {
                    cores[j] = 1 - cores[atual];
                    fila[fim++] = j;
                } else if (cores[j] == cores[atual]) {
                    // conflito de cor
                    free(cores);
                    free(vertices);
                    free(fila);
                    return 0;
                }
            }
        }

        free(fila);
    }

    free(cores);
    free(vertices);
    return 1;
}


//------------------------------------------------------------------------------
// devolve o número de vértices em g

unsigned int n_vertices(grafo *g) {
    unsigned int count = 0;
    for (lista_adjacencia *v = g->l; v != NULL; v = v->proxima) {
        count++;
    }
    return count;
}

//------------------------------------------------------------------------------
// devolve o número de arestas em g

unsigned int n_arestas(grafo *g) {
    unsigned int count = 0;
    for (lista_adjacencia *v = g->l; v != NULL; v = v->proxima) {
        for (vizinho *vz = v->lista_vizinhos; vz != NULL; vz = vz->prox_vizinho) {
            count++;
        }
    }
    return count / 2;
}

//------------------------------------------------------------------------------
// devolve o número de componentes em g

unsigned int n_componentes(grafo *g) {
    unsigned int count = 0;

    // Max número de vértices = quantidade de vértices
    int max = n_vertices(g);
    char **visitados = calloc(max + 1, sizeof(char *)); // null-terminated

    for (lista_adjacencia *v = g->l; v != NULL; v = v->proxima) {
        // Verifica se já foi visitado
        int ja_foi = 0;
        for (int i = 0; visitados[i] != NULL; i++) {
            if (strcmp(visitados[i], v->nome_nodo_ref) == 0) {
                ja_foi = 1;
                break;
            }
        }

        if (!ja_foi) {
            dfs(v, visitados, g);
            count++;
        }
    }

    free(visitados);
    return count;
}


//------------------------------------------------------------------------------
// devolve uma "string" com os diâmetros dos componentes de g separados por brancos
// em ordem não decrescente

char *diametros(grafo *g){
  return 1;
}

//------------------------------------------------------------------------------
// devolve uma "string" com os nomes dos vértices de corte de g em
// ordem alfabética, separados por brancos

char *vertices_corte(grafo *g){
  return 1;
}

//------------------------------------------------------------------------------
// devolve uma "string" com as arestas de corte de g em ordem alfabética, separadas por brancos
// cada aresta é o par de nomes de seus vértices em ordem alfabética, separadas por brancos
//
// por exemplo, se as arestas de corte são {z, a}, {x, b} e {y, c}, a resposta será a string
// "a z b x c y"

char *arestas_corte(grafo *g){
  return 1;
}


