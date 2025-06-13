#include <stdbool.h>
#ifndef GRAFO_H
#define GRAFO_H
#define TAMANHO_MAX_LINHA 2048

//------------------------------------------------------------------------------
// estrutura de dados para representar um grafo

typedef struct vizinho {
  char *nome_nodo;
  int  peso;
  struct vizinho *prox_vizinho;
} vizinho;

typedef struct lista_adjacencia {
  char *nome_nodo_ref;
  struct lista_adjacencia * proxima;
  vizinho *lista_vizinhos; 
} lista_adjacencia;

typedef struct grafo {
  char *nome;
  lista_adjacencia *l; 
} grafo;

//------------------------------------------------------------------------------
// Protótipos auxiliares definidos em grafo.c
char *copia_str(const char *s);
lista_adjacencia *encontrar_vertice(grafo *g, const char *nome);
lista_adjacencia *adicionar_vertice(grafo *g, const char *nome);
void adicionar_vizinho(lista_adjacencia *v, const char *vizinho_nome, int peso);
void adicionar_aresta(grafo *g, char *v1, char *v2, int peso);
void dfs(lista_adjacencia *v, char **visitados, grafo *g);
int indice_vertice(grafo *g, const char *nome);
int bfs_distancias(grafo *g, lista_adjacencia *inicio, int *dist, int *visitado, lista_adjacencia **v_array, int n);
void dfs_tarjan(grafo *g, lista_adjacencia **v_array, int *disc, int *low, int *pai, bool *ap, int u, int n);

//------------------------------------------------------------------------------
// Funções principais da API

grafo *le_grafo(FILE *f);
unsigned int destroi_grafo(grafo *g);
char *nome(grafo *g);
unsigned int bipartido(grafo *g);
unsigned int n_vertices(grafo *g);
unsigned int n_arestas(grafo *g);
unsigned int n_componentes(grafo *g);
char *diametros(grafo *g);
char *vertices_corte(grafo *g);
char *arestas_corte(grafo *g);

#endif
