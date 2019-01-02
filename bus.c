#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "bus.h"

#include "jrb.h"
#include "jval.h"
#include "dllist.h"

#define MAX 100000

Graph createGraph() {
  Graph graph;
  graph.edges = make_jrb();
  graph.vertices = make_jrb();
  return graph;
}

void addVertex(Graph g, int id, char* name) {
  JRB v = jrb_find_int(g.vertices, id);
  if (v == NULL) 
    jrb_insert_int(g.vertices, id, new_jval_s(name));
  return;
}

char* getVertex(Graph g, int id) {
  JRB v = jrb_find_int(g.vertices, id);
  if (v == NULL)
    return NULL;
  else
    return jval_s(v->val);
}

void addEdge(Graph g, int v1, int v2, char *weight) {
  JRB tree;
  JRB node = jrb_find_int(g.edges, v1);
  if (node == NULL) {
    tree = make_jrb();
    jrb_insert_int(g.edges, v1, new_jval_v(tree));
  }
  else {
    tree = (JRB)jval_v(node->val);
  }
  jrb_insert_int(tree, v2, new_jval_s(weight));
}

int hasEdge(Graph g, int v1, int v2) {
  JRB node = jrb_find_int(g.edges, v1);
  if (node == NULL)
    return -1;
  else {
    JRB tree = (JRB)jval_v(node->val);
    JRB temp = jrb_find_int(tree, v2);
    if (temp == NULL)
      return 0;
    else
      return 1;
  }
}

char* getEdgeValue(Graph g, int v1, int v2) {
  if (hasEdge(g, v1, v2) == -1)
    return NULL;
  else if (hasEdge(g, v1, v2) == 0)
    return NULL;
  else {
    JRB node = jrb_find_int(g.edges, v1);
    JRB tree = (JRB)jval_v(node->val);
    JRB temp = jrb_find_int(tree, v2);
    return jval_s(temp->val);
  }
}

void setEdgeValue(Graph g, int v1, int v2, char* new_weight) {
  JRB node = jrb_find_int(g.edges, v1);
  JRB tree = (JRB)jval_v(node->val);
  JRB temp = jrb_find_int(tree, v2);
  temp->val = new_jval_s(new_weight);
}

int indegree(Graph g, int v, int* output) {
  JRB tree, node;
  int total = 0;
  jrb_traverse(node, g.edges) {
    tree = (JRB) jval_v(node->val);
    if (jrb_find_int(tree, v)) {
      output[total] = jval_i(node->key);
      total++;
    }
  }
  return total;
}

int outdegree(Graph g, int v, int *output) {
  JRB tree, node;
  int total = 0;
  node = jrb_find_int(g.edges, v);
  if (node == NULL)
    return 0;
  else {
    tree = jval_v(node->val);
    jrb_traverse(node, tree) {
      output[total] = jval_i(node->key);
      total++;
    }
  }
  return total;
}

int BFS(Graph graph, int start, int stop, JRB sp_distance, JRB sp_parent) {
  JRB visited = make_jrb();
  Dllist node;
  Dllist queue = new_dllist();
  int n, output[100], i, u, v, min_length = 0;
  jrb_insert_int(sp_distance, start, new_jval_i(0));
  jrb_insert_int(sp_parent, start, new_jval_i(start));
  dll_prepend(queue, new_jval_i(start));
  while ( !dll_empty(queue) ) {
    node = dll_first(queue);
    u = jval_i(node->val);
    dll_delete_node(node);
    min_length = jval_i(jrb_find_int(sp_distance, u)->val);

    if (jrb_find_int(visited, u) == NULL) { 
      jrb_insert_int(visited, u, new_jval_i(1));
      n = outdegree(graph, u, output);
      for (i = 0; i < n; i++) {
	v = output[i];
	if (jrb_find_int(visited, v) == NULL) {
	  dll_append(queue, new_jval_i(v));
	  JRB temp = jrb_find_int(sp_distance, output[i]);
	  if (temp == NULL) {
	    jrb_insert_int(sp_distance, v, new_jval_i(min_length + 1));
	    jrb_insert_int(sp_parent, v, new_jval_i(u));
	  }
	  if (v == stop) {
	    jrb_free_tree(visited);
	    return 1;
	  }
	}
      }
    }
			
  }
  jrb_free_tree(visited);
  return 0;
}

char *buses[MAX]; 

char *readHolder1[MAX]; 

char *readHolder2[MAX]; 

int busLine[MAX][MAX]; 
int sizeHolder = 0; 
int sizeBuses = 0; 
int row = 0, col = 0;

void printVertex(int v)
{
  printf("%4d ", v);
}

void addStations(Graph g, char *station, int *vertexID) {
  int flag = 0;
  JRB node;
  jrb_traverse(node, g.vertices) {
    if (strcmp(getVertex(g, jval_i(node->key)), station) == 0) {
      flag = 1;
      break;
    }
  }
  if (flag == 0) {
    addVertex(g, *vertexID, station);
    (*vertexID)++;
  }
}

void getVertexID(Graph g, int *id1, int *id2, char* station1, char* station2) {
  JRB node;
  jrb_traverse(node, g.vertices) {
    if (strcmp(getVertex(g, jval_i(node->key)), station1) == 0) {
      *id1 = jval_i(node->key);
    }
    if (strcmp(getVertex(g, jval_i(node->key)), station2) == 0) {
      *id2 = jval_i(node->key);
    }
  }
}

JRB createBusTable() {
  JRB busTable = make_jrb();
  return busTable;
}

void makeBusTable(JRB bus, int size, char* buses[]) {
  char *busName[MAX];
  for (int i = 0; i < size; i++) {
    busName[i] = strdup(buses[i]);
  }
  JRB node;
  for (int i = 1; i <= size; i++) {
    char *temp = (char*)malloc(sizeof(char));
    temp[0] = i + '0';
    temp[1] = '\0';
    int len = strlen(buses[i - 1]);
    busName[i - 1][len] = '/';
    busName[i - 1][len + 1] = '\0';
    if (jrb_find_str(bus, strdup(temp)) == NULL) {
      jrb_insert_str(bus, strdup(temp), new_jval_s(busName[i - 1]));
    }
  }
}

void setBus(JRB busTable, char *station_id) {
  JRB node;
  char *station_name = (char*)malloc(sizeof(char));
  if (jrb_find_str(busTable, strdup(station_id)) == NULL) {
    int i = 0;
    while (station_id[i] != '\0') {
      char* station = (char*)malloc(sizeof(char));
      station[0] = station_id[i];
      station[1] = '\0';
      node = jrb_find_str(busTable, station);
      if (node == NULL) {
	printf("error\n");
	break;
      }
      else {
	strcat(station_name, jval_s(node->val));
	if (station_id[i + 1] == '\0') break;
	else i = i + 2;
      }
    }
    jrb_insert_str(busTable, strdup(station_id), new_jval_s(station_name));
  }
}

void adjustBusVal(JRB bus) {
  JRB node;
  jrb_traverse(node, bus) {
    char *temp = jval_s(node->val);
    int len = strlen(temp);
    temp[len - 1] = '\0';
    node->val = new_jval_s(strdup(temp));
  }
}

void addEdges(Graph g, JRB bus, char* station1, char* station2, int bus_num) {
  int id1 = 0;
  int id2 = 0;
  getVertexID(g, &id1, &id2, station1, station2);
  if (id1 != id2) {
    char *edgeValue = (char*)malloc(sizeof(char));
    if (getEdgeValue(g, id1, id2) != NULL) {
      strcpy(edgeValue, getEdgeValue(g, id1, id2));
      if (edgeValue[0] == (bus_num + 1) + '0') 
	return;
      int len = strlen(edgeValue);
      edgeValue[len] = '-';
      edgeValue[len + 1] = (bus_num + 1) + '0';
      edgeValue[len + 2] = '\0';
      setEdgeValue(g, id1, id2, edgeValue);
			
    }
    else {
      edgeValue[0] = (bus_num + 1) + '0';
      edgeValue[1] = '\0';
      addEdge(g, id1, id2, edgeValue);
    }
    setBus(bus, edgeValue);
  }
}

int checkStationName(Graph g, char *station) {
  JRB node;
  jrb_traverse(node, g.vertices) {
    if (strcmp(jval_s(node->val), station) == 0)
      return jval_i(node->key);
  }
  return -1;
}

void trim(char *line) {
  int i = strlen(line);
  while (i > 0 && isspace(line[i - 1])) {
    --i;
  }
  line[i] = '\0';
  i = strspn ( line, " \t\n\v" );
  memmove ( line, line + i, strlen ( line + i ) + 1 );
}

void readFile(FILE *f1, FILE *f2) {
  int i = 0, k = 0, flag = 0;
  char *reader = (char*)malloc(sizeof(char));
  int fscanfChecker = fscanf(f1, "%[^\n]%*c", reader);
  buses[k] = strdup(reader);
  k++;
  busLine[row][col] = k;
  while (fscanfChecker == 1) {
    flag = 0;
    fscanfChecker = fscanf(f1, "%[^\n]%*c", reader);
    if (fscanfChecker != 1) break;
    if (strchr(strdup(reader), '-') != NULL) {
      readHolder1[i] = strdup(reader);
      flag = 1;
      i++;
      busLine[row][col + 1] = busLine[row][col + 1] + 1;
    }
    if (flag == 0) {
      buses[k] = strdup(reader);
      row++;
      k++;
      busLine[row][col] = k;
    }
  }
  sizeHolder = i; sizeBuses = k;
  i = 0;
  fscanfChecker = fscanf(f2, "%[^\n]%*c", reader);
  while (fscanfChecker == 1) {
    fscanfChecker = fscanf(f2, "%[^\n]%*c", reader);
    if (fscanfChecker != 1) break;
    if (strchr(strdup(reader), '-') != NULL) {
      readHolder2[i] = strdup(reader);
      i++;
    }
  }
}
void copyData(char *bus[], char* file1[], char* file2[]) {
  for (int i = 0; i < sizeHolder; i++) {
    file1[i] = strdup(readHolder1[i]);
    file2[i] = strdup(readHolder2[i]);
  }
  for (int i = 0; i < sizeBuses; i++) {
    bus[i] = strdup(buses[i]);
  }
}
void dataProcess(Graph g, JRB bus) {
  int i = 0, count = 0;
  int vertexID = 0;
  char *linesplitter[MAX];
  char *temp = "-";
  for (i = 0; i <= row; i++) {
    for (int m = 0; m < busLine[i][col + 1]; m++) {
      int j = 0, k = 0;
      linesplitter[j] = strtok(readHolder1[count], temp);
      while (linesplitter[j] != NULL) {
	j++;
	linesplitter[j] = strtok(NULL, temp);
      }
      linesplitter[j] = strtok(readHolder2[count], temp);
      while (linesplitter[j] != NULL) {
	j++;
	linesplitter[j] = strtok(NULL, temp);
      }
			
      char weight[10];
      weight[0] = (i + 1) + '0';
      weight[1] = '\0';
      for (int k = 0; k < j; k++) {
	trim(linesplitter[k]);
	addStations(g, linesplitter[k], &vertexID);
      }
			
      for (int k = 0; k < j - 1; k++) {
	addEdges(g, bus, linesplitter[k], linesplitter[k + 1], i);
      }
      count++;
    }
		
  }
}

void printFileData(char *bus[], char* file1[], char* file2[]) {
  int count = 0;
  for (int i = 0; i < sizeBuses; i++) {
    printf("BUS: %s\n", bus[i]);
    for (int j = 0; j < busLine[i][col + 1]; j++) {
      printf("CHIEU DI: %s\n\n", file1[count]);
      printf("CHIEU VE: %s\n\n", file2[count]);
      count++;
    }
  }
}

void printBuses(JRB bus) {
  JRB node;
  jrb_traverse(node, bus) {
    printf("%s - %s\n", jval_s(node->key), jval_s(node->val));
  }
}

void printPath(Graph g, JRB busTable, int *sizeOutput, int *output) {
  if (*sizeOutput <= 0)
    return;
  int i = *sizeOutput - 1;;
  for (i = *sizeOutput - 1; i > 0; i--) {
    char*station1 = getVertex(g, output[i]);
    if (station1 == NULL) {
      printf("station1 at printPath is null\n");
      exit(0);
    }
    char*edgeValue = getEdgeValue(g, output[i], output[i - 1]);
    if (edgeValue == NULL) {
      printf("Edge value is null\n");
      exit(0);
    }
    printf("%s (%s) --> ", station1, jval_s(jrb_find_str(busTable, strdup(edgeValue))->val));
  }
  printf("%s\n", getVertex(g, output[i]));
}

void printVertices(Graph g) {
  JRB node;
  jrb_traverse(node, g.vertices) {
    printf("%d - %s\n", jval_i(node->key), jval_s(node->val));
  }
}

void printEdges(Graph g) {
  JRB node;
  JRB node2;
  jrb_traverse(node, g.vertices) {
    jrb_traverse(node2, g.vertices) {
      char *temp = getEdgeValue(g, jval_i(node->key), jval_i(node2->key));
      if (temp != NULL)
	printf("%s\n", temp);
    }
  }
}

void printShortestPath(Graph g, int start, int stop, int *size, int *path) {
  JRB sp_distance = make_jrb();
  JRB sp_parent = make_jrb();
  int shortest_path = BFS(g, start, stop, sp_distance, sp_parent);
  if (shortest_path == 0) 
    printf("Invalid route\n");
  else {
    int current = 0, parent = 0;
    current = stop;
    parent = jval_i(jrb_find_int(sp_parent, current)->val);
    path[*size] = current;
    while (current != parent) {
      path[*size] = current;
      current = parent;
      (*size)++;
      parent = jval_i(jrb_find_int(sp_parent, current)->val);
      path[*size] = current;
    }
    (*size)++;
    path[*size] = start;
    for (int j = (*size) - 1; j >= 0; j--) {
      printf("%d ", path[j]);
    }
    printf("\n");
  }
  jrb_free_tree(sp_distance);
  jrb_free_tree(sp_parent);
}
void freeGraph(Graph graph) {
  JRB node, tree;
  jrb_traverse(node, graph.edges) {
    tree = (JRB) jval_v(node->val);
    jrb_free_tree(tree);
  }
  jrb_free_tree(graph.edges);
  jrb_free_tree(graph.vertices);
}
