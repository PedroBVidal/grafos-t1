#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"

char *copia_str(const char *s);
lista_adjacencia *encontrar_vertice(grafo *g, const char *nome);
lista_adjacencia *adicionar_vertice(grafo *g, const char *nome);
void adicionar_vizinho(lista_adjacencia *v, const char *vizinho_nome, int peso);
void adicionar_aresta(grafo *g, char *v1, char *v2, int peso);

//------------------------------------------------------------------------------
int main(void) {

  grafo *g = le_grafo(stdin);
  char *s;

  printf("grafo: %s\n", nome(g));
  printf("%d vertices\n", n_vertices(g));
  printf("%d arestas\n", n_arestas(g));
  /*
  printf("%d componentes\n", n_componentes(g));

  printf("%sbipartido\n", bipartido(g) ? "" : "não ");

  printf("diâmetros: %s\n", s=diametros(g));
  free(s);

  printf("vértices de corte: %s\n", s=vertices_corte(g));
  free(s);

  printf("arestas de corte: %s\n", s=arestas_corte(g));
  free(s);

  */
  return ! destroi_grafo(g);
}
