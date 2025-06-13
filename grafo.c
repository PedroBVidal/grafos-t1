#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
  int primeira_linha = 1;
  while (fgets(linha, sizeof(linha), f)) {
      // Remove quebra de linha
      linha[strcspn(linha, "\n")] = 0;
      
      // Ignora comentários e linhas vazias
      if ((linha[0] == '/' && linha[1] == '/') || (linha[0] == '\0')) {
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
          adicionar_aresta(g, vertice1, vertice2, 1);
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



// Funções completas de análise de grafo

#include <limits.h>
#include <stdbool.h>
#include <ctype.h>

// Função auxiliar: BFS para encontrar distâncias a partir de um vértice
int bfs_distancias(grafo *g, lista_adjacencia *inicio, int *dist, int *visitado, lista_adjacencia **v_array, int n) {
    for (int i = 0; i < n; i++) {
        dist[i] = -1;
        visitado[i] = 0;
    }

    int ini = 0, fim = 0;
    int *fila = malloc(n * sizeof(int));

    int start = indice_vertice(g, inicio->nome_nodo_ref);
    fila[fim++] = start;
    dist[start] = 0;
    visitado[start] = 1;

    while (ini < fim) {
        int u = fila[ini++];
        for (vizinho *vz = v_array[u]->lista_vizinhos; vz != NULL; vz = vz->prox_vizinho) {
            int v = indice_vertice(g, vz->nome_nodo);
            if (!visitado[v]) {
                visitado[v] = 1;
                dist[v] = dist[u] + 1;
                fila[fim++] = v;
            }
        }
    }

    free(fila);
    return start;
}

char *diametros(grafo *g) {
    int n = n_vertices(g);
    lista_adjacencia **v_array = malloc(n * sizeof(lista_adjacencia *));
    int idx = 0;
    for (lista_adjacencia *v = g->l; v != NULL; v = v->proxima)
        v_array[idx++] = v;

    int *visitado_global = calloc(n, sizeof(int));
    int *dist = malloc(n * sizeof(int));
    int *visitado = malloc(n * sizeof(int));
    int *diametros = malloc(n * sizeof(int));
    int dcount = 0;

    // Função Dijkstra
    void dijkstra(int start, int *dist) {
        for (int i = 0; i < n; i++) {
            dist[i] = INT_MAX;
            visitado[i] = 0;
        }
        dist[start] = 0;

        for (int iter = 0; iter < n; iter++) {
            int u = -1, min_dist = INT_MAX;
            for (int j = 0; j < n; j++) {
                if (!visitado[j] && dist[j] < min_dist) {
                    min_dist = dist[j];
                    u = j;
                }
            }
            if (u == -1) break;
            visitado[u] = 1;

            for (vizinho *vz = v_array[u]->lista_vizinhos; vz != NULL; vz = vz->prox_vizinho) {
                int v = indice_vertice(g, vz->nome_nodo);
                if (!visitado[v] && dist[u] + vz->peso < dist[v]) {
                    dist[v] = dist[u] + vz->peso;
                }
            }
        }
    }

    for (int i = 0; i < n; i++) {
        if (visitado_global[i]) continue;

        // marca todos os vértices do componente
        dijkstra(i, dist);
        for (int j = 0; j < n; j++) if (dist[j] != INT_MAX) visitado_global[j] = 1;

        // encontra o mais distante de i
        int mais_dist = i, max_d = 0;
        for (int j = 0; j < n; j++) {
            if (dist[j] != INT_MAX && dist[j] > max_d) {
                max_d = dist[j];
                mais_dist = j;
            }
        }

        dijkstra(mais_dist, dist);
        max_d = 0;
        for (int j = 0; j < n; j++) {
            if (dist[j] != INT_MAX && dist[j] > max_d)
                max_d = dist[j];
        }

        diametros[dcount++] = max_d;
    }

    // ordena os diâmetros
    for (int i = 0; i < dcount; i++) {
        for (int j = i + 1; j < dcount; j++) {
            if (diametros[i] > diametros[j]) {
                int tmp = diametros[i];
                diametros[i] = diametros[j];
                diametros[j] = tmp;
            }
        }
    }

    // monta string
    char *saida = malloc(dcount * 32);
    saida[0] = '\0';
    char buffer[32];
    for (int i = 0; i < dcount; i++) {
        sprintf(buffer, "%d", diametros[i]);
        strcat(saida, buffer);
        if (i < dcount - 1) strcat(saida, " ");
    }

    free(v_array); free(visitado_global); free(dist); free(visitado); free(diametros);
    return saida;
}


int time_tarjan;

void dfs_tarjan(grafo *g, lista_adjacencia **v_array, int *disc, int *low, int *pai, bool *ap, int u, int n) {
    disc[u] = low[u] = ++time_tarjan;
    int filhos = 0;

    for (vizinho *vz = v_array[u]->lista_vizinhos; vz != NULL; vz = vz->prox_vizinho) {
        int v = indice_vertice(g, vz->nome_nodo);
        if (disc[v] == -1) {
            filhos++;
            pai[v] = u;
            dfs_tarjan(g, v_array, disc, low, pai, ap, v, n);
            if (low[v] >= disc[u] && pai[u] != -1)
                ap[u] = true;
            if (pai[u] == -1 && filhos > 1)
                ap[u] = true;
            if (low[u] > low[v]) low[u] = low[v];
        } else if (v != pai[u]) {
            if (low[u] > disc[v]) low[u] = disc[v];
        }
    }
}

char *vertices_corte(grafo *g) {
    int n = n_vertices(g);
    lista_adjacencia **v_array = malloc(n * sizeof(lista_adjacencia *));
    int i = 0;
    for (lista_adjacencia *v = g->l; v != NULL; v = v->proxima)
        v_array[i++] = v;

    int *disc = malloc(n * sizeof(int));
    int *low = malloc(n * sizeof(int));
    int *pai = malloc(n * sizeof(int));
    bool *ap = calloc(n, sizeof(bool));

    for (i = 0; i < n; i++) {
        disc[i] = -1;
        pai[i] = -1;
    }

    time_tarjan = 0;
    for (i = 0; i < n; i++) {
        if (disc[i] == -1)
            dfs_tarjan(g, v_array, disc, low, pai, ap, i, n);
    }

    char **nomes = malloc(n * sizeof(char *));
    int count = 0;
    for (i = 0; i < n; i++) {
        if (ap[i]) nomes[count++] = v_array[i]->nome_nodo_ref;
    }

    for (i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(nomes[i], nomes[j]) > 0) {
                char *tmp = nomes[i];
                nomes[i] = nomes[j];
                nomes[j] = tmp;
            }
        }
    }

    char *saida = malloc(count * TAMANHO_MAX_LINHA);
    saida[0] = '\0';
    for (i = 0; i < count; i++) {
        strcat(saida, nomes[i]);
        if (i < count - 1) strcat(saida, " ");
    }

    free(v_array); free(disc); free(low); free(pai); free(ap); free(nomes);
    return saida;
}

char *arestas_corte(grafo *g) {
    int n = n_vertices(g);
    lista_adjacencia **v_array = malloc(n * sizeof(lista_adjacencia *));
    for (int i = 0; i < n; i++) v_array[i] = NULL;
    int i = 0;
    for (lista_adjacencia *v = g->l; v != NULL; v = v->proxima)
        v_array[i++] = v;

    int *disc = malloc(n * sizeof(int));
    int *low = malloc(n * sizeof(int));
    int *pai = malloc(n * sizeof(int));

    typedef struct { char *a, *b; } corte;
    corte *arestas = malloc(n * n * sizeof(corte));
    int count = 0;

    for (i = 0; i < n; i++) {
        disc[i] = -1;
        pai[i] = -1;
    }

    void dfs_arestas(int u) {
        disc[u] = low[u] = ++time_tarjan;
        for (vizinho *vz = v_array[u]->lista_vizinhos; vz != NULL; vz = vz->prox_vizinho) {
            int v = indice_vertice(g, vz->nome_nodo);
            if (disc[v] == -1) {
                pai[v] = u;
                dfs_arestas(v);
                if (low[v] > disc[u]) {
                    char *a = v_array[u]->nome_nodo_ref;
                    char *b = v_array[v]->nome_nodo_ref;
                    if (strcmp(a, b) > 0) { char *tmp = a; a = b; b = tmp; }
                    arestas[count++] = (corte){a, b};
                }
                if (low[u] > low[v]) low[u] = low[v];
            } else if (v != pai[u] && low[u] > disc[v]) {
                low[u] = disc[v];
            }
        }
    }

    time_tarjan = 0;
    for (i = 0; i < n; i++) {
        if (disc[i] == -1)
            dfs_arestas(i);
    }

    for (i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            int c = strcmp(arestas[i].a, arestas[j].a);
            if (c > 0 || (c == 0 && strcmp(arestas[i].b, arestas[j].b) > 0)) {
                corte tmp = arestas[i];
                arestas[i] = arestas[j];
                arestas[j] = tmp;
            }
        }
    }

    char *saida = malloc(count * TAMANHO_MAX_LINHA);
    saida[0] = '\0';
    for (i = 0; i < count; i++) {
        strcat(saida, arestas[i].a);
        strcat(saida, " ");
        strcat(saida, arestas[i].b);
        if (i < count - 1) strcat(saida, " ");
    }

    free(v_array); free(disc); free(low); free(pai); free(arestas);
    return saida;
}
