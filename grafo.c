#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "grafo.h"


void inicializar_grafo(grafo *g){
   
}

void adicionar_aresta(grafo *g, char *v1, char *v2, int peso){
  if (g->l == NULL){
    struct lista_adjacencia * l = malloc(sizeof(struct lista_adjacencia));
    g->l = lista_adjacencia;
    struct vizinho lista_vizinhos *l_v = malloc(sizeof(struct vizinho)); 
    g->l->lista_vizinhos = l_v;
     
    // Adiciona o primeiro nodo_ref 
    g->l->nome_nodo_ref = v1;
    g->l->lista_vizinhos->nome_nodo = v2;
    g->l->lista_vizinhos->peso = peso;
    g->l->lista_vizinhos->prox_vizinho = NULL; 
    // Adiciona o segundo  nodo_ref
    l = malloc(sizeof(struct lista_adjacencia));
    g->l->proxima = l;
    g->l->proxima->nome_nodo_ref = v2;
    l_v = malloc(sizeof(struct vizinho)); 
    g->l->proxima->lista_vizinhos = l_v;
    g->l->proxima->lista_vizinhos->nome_nodo = v1;
    g->l->proxima->lista_vizinhos->peso = peso;
    g->l->proxima->lista_vizinhos->prox_vizinhos = NULL;
    // Ainda nao pronto
  } 
}


grafo *le_grafo(FILE *f){
  grafo *g = malloc(sizeof(struct grafo));
  g->l = NULL;
  inicializar_grafo(g);
   
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
          strcpy(nome_grafo, linha);
          printf("Nome do grafo: %s\n", nome_grafo);
          primeira_linha = 0;
          continue;
      }

      // Analisa aresta

      char vertice1[TAMANHO_MAX_LINHA], vertice2[TAMANHO_MAX_LINHA]; 
      int peso = 0;
      // Tenta ler dois vértices e um peso
      if (sscanf(linha, "%s -- %s %d", vertice1, vertice2, &peso) == 3) {
          printf("Aresta: %s -- %s, Peso: %d\n", vertice1, vertice2, peso);
          adicionar_aresta(g, vertice1, vertice2, peso);
      }
      // Tenta ler apenas dois vértices (peso opcional)
      else if (sscanf(linha, "%s -- %s", vertice1, vertice2) == 2) {
          printf("Aresta: %s -- %s, Peso: %d\n", vertice1, vertice2, peso);
          adicionar_aresta(g, vertice1, vertice2, 0);
      } 
      // Vértice isolado
      else {
          printf("Vértice isolado: %s\n", linha);
      }
  } 

}

//------------------------------------------------------------------------------
// desaloca toda a estrutura de dados alocada em g
//
// devolve 1 em caso de sucesso e 0 em caso de erro

unsigned int destroi_grafo(grafo *g){
  return 1;
}

//------------------------------------------------------------------------------
// devolve o nome de g

char *nome(grafo *g){
  return 1;
}

//------------------------------------------------------------------------------
// devolve 1 se g é bipartido e 0 caso contrário

unsigned int bipartido(grafo *g){
  return 1;
}

//------------------------------------------------------------------------------
// devolve o número de vértices em g

unsigned int n_vertices(grafo *g){
  return 1;
}

//------------------------------------------------------------------------------
// devolve o número de arestas em g

unsigned int n_arestas(grafo *g){
  return 1;
}

//------------------------------------------------------------------------------
// devolve o número de componentes em g

unsigned int n_componentes(grafo *g){
  return 1;
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


