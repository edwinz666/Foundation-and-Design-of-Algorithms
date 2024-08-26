/*
graph.c

Set of vertices and edges implementation.

Implementations for helper functions for graph construction and manipulation.

Skeleton written by Grady Fitzpatrick for COMP20007 Assignment 1 2022
*/
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include "graph.h"
#include "utils.h"
#include "pq.h"

#define INITIALEDGES 32

struct edge;

/* Definition of a graph. */
struct graph {
  int numVertices;
  int numEdges;
  int allocedEdges;
  struct edge **edgeList;
};

/* Definition of an edge. */
struct edge {
  int start;
  int end;
  int cost;
};

/* algorithm to find shortest distance to each room */
int *dijkstra_alg(int numRooms, struct edge **edgeList, int numEdges, 
                int startingRoom, int numHeartRooms, int *heartRooms);

struct graph *newGraph(int numVertices){
  struct graph *g = (struct graph *) malloc(sizeof(struct graph));
  assert(g);
  /* Initialise edges. */
  g->numVertices = numVertices;
  g->numEdges = 0;
  g->allocedEdges = 0;
  g->edgeList = NULL;
  return g;
}

/* Adds an edge to the given graph. */
void addEdge(struct graph *g, int start, int end, int cost){
  assert(g);
  struct edge *newEdge = NULL;
  /* Check we have enough space for the new edge. */
  if((g->numEdges + 1) > g->allocedEdges){
    if(g->allocedEdges == 0){
      g->allocedEdges = INITIALEDGES;
    } else {
      (g->allocedEdges) *= 2;
    }
    g->edgeList = (struct edge **) realloc(g->edgeList,
      sizeof(struct edge *) * g->allocedEdges);
    assert(g->edgeList);
  }

  /* Create the edge */
  newEdge = (struct edge *) malloc(sizeof(struct edge));
  assert(newEdge);
  newEdge->start = start;
  newEdge->end = end;
  newEdge->cost = cost;

  /* Add the edge to the list of edges. */
  g->edgeList[g->numEdges] = newEdge;
  (g->numEdges)++;
}

/* Returns a new graph which is a deep copy of the given graph (which must be 
  freed with freeGraph when no longer used). */
struct graph *duplicateGraph(struct graph *g){
  struct graph *copyGraph = (struct graph *) malloc(sizeof(struct graph));
  assert(copyGraph);
  copyGraph->numVertices = g->numVertices;
  copyGraph->numEdges = g->numEdges;
  copyGraph->allocedEdges = g->allocedEdges;
  copyGraph->edgeList = (struct edge **) malloc(sizeof(struct edge *) * g->allocedEdges);
  assert(copyGraph->edgeList || copyGraph->numEdges == 0);
  int i;
  /* Copy edge list. */
  for(i = 0; i < g->numEdges; i++){
    struct edge *newEdge = (struct edge *) malloc(sizeof(struct edge));
    assert(newEdge);
    newEdge->start = (g->edgeList)[i]->start;
    newEdge->end = (g->edgeList)[i]->end;
    newEdge->cost = (g->edgeList)[i]->cost;
    (copyGraph->edgeList)[i] = newEdge;
  }
  return copyGraph;
}

/* Frees all memory used by graph. */
void freeGraph(struct graph *g){
  int i;
  for(i = 0; i < g->numEdges; i++){
    free((g->edgeList)[i]);
  }
  if(g->edgeList){
    free(g->edgeList);
  }
  free(g);
}

struct solution *graphSolve(struct graph *g, enum problemPart part,
  int numRooms, int startingRoom, int bossRoom, int numShortcuts, 
  int *shortcutStarts, int *shortcutEnds, int numHeartRooms, int *heartRooms){
  struct solution *solution = (struct solution *)
  malloc(sizeof(struct solution));
  assert(solution);

  int *dist;

  if(part == PART_A){
    /* IMPLEMENT 2A SOLUTION HERE */
    // run dijkstra's algorithm normally
    dist = dijkstra_alg(numRooms, g->edgeList, g->numEdges, startingRoom, numHeartRooms, heartRooms);
    solution->heartsLost = dist[bossRoom];
  } 
  
  else if(part == PART_B) {
    /* IMPLEMENT 2B SOLUTION HERE */
    struct graph *g_dupe;
    int min = INT_MAX;

    // construct a new graph for each shortcut, running dijkstra's algorithm to see if new graph is better
    for (int i=0; i<numShortcuts; i++) {
      g_dupe = duplicateGraph(g);
      addEdge(g_dupe, shortcutStarts[i], shortcutEnds[i], 1);
      dist = dijkstra_alg(numRooms, g_dupe->edgeList, g_dupe->numEdges, startingRoom, numHeartRooms, heartRooms);
      if (dist[bossRoom] < min) {
          min = dist[bossRoom];
      }

      freeGraph(g_dupe);
    }

    solution->heartsLost = min;
  } 
  
  else {
    /* IMPLEMENT 2C SOLUTION HERE */
    // run dijkstra's algorithm, adjusting costs for heart rooms
    dist = dijkstra_alg(numRooms, g->edgeList, g->numEdges, startingRoom, numHeartRooms, heartRooms);
    solution->heartsLost = dist[bossRoom];
  }

  return solution;
}

 
/* algorithm to find shortest distance to each room */
int *dijkstra_alg(int numRooms, struct edge **edgeList, int numEdges, 
                int startingRoom, int numHeartRooms, int *heartRooms) {
  int i, j, k;

  struct pq *p_Queue = newPQ();
  int *dist = (int *)malloc(sizeof(int) * numRooms);

  // enqueueing vertexes in the priority queue, and setting priorities/distances
  for (i=0; i<numRooms; i++) {
    int *num = malloc(sizeof(int));
    *num = i;

    enqueue(p_Queue, (void *)num, INT_MAX);
    dist[i] = INT_MAX;
  }

  (p_Queue->priorities)[startingRoom] = 0;
  dist[startingRoom] = 0;
  
  
  int curr; // current ejected value (room) from the queue
  int end; // for a given room and an edge, this labels the other room as the end room
  int found = 0;
  struct edge *currEdge;

  while(! empty(p_Queue)) {
    // eject the room with lowest priority from queue
    curr = *((int *)deletemin(p_Queue));
    // iterate over each edge
    for (i=0; i<numEdges; i++) {
      currEdge = edgeList[i];
      // one of the rooms is connected by this edge
      if (currEdge->start == curr || currEdge->end == curr) {
        // labelling the room that isn't the current room as the end
        if (currEdge->start == curr) {
          end = currEdge->end;
        } else {
          end = currEdge->start;
        }
        // the end room is part of the queue
        for (j=0; j<p_Queue->count; j++) {
          if (*((int *)(p_Queue->queue)[j]) == end) {
            found = 1;
            break;
          }
        }

        // for Part C, adjust costs for edges that lead to a heart room to 0
        if (found) {
          for (k=0; k<numHeartRooms; k++) {
            if (end == heartRooms[k]) {
              currEdge->cost = 0;
              break;
            }
          }
        }

        // updating the distances if the end room is part of the queue and a shorter distance found
        if (found && (dist[curr] + currEdge->cost < dist[end])) {
          (p_Queue->priorities)[j] = dist[curr] + currEdge->cost;
          dist[end] = dist[curr] + currEdge->cost;
        }
        
        // resetting found and cost back to original
        found = 0;
        currEdge->cost = 1;
          
      }
    }
  }

  return dist;
}