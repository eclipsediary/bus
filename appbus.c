#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 100

#include "jrb.h"
#include "jval.h"
#include "dllist.h"
#include "bus.h"

int main() {
  
  Graph g = createGraph();
  JRB busTable = createBusTable();
  JRB lineTable = make_jrb();
  char *buses[MAX]; 
  char *readHolder1[MAX]; 
  char *readHolder2[MAX]; 
  int busLine[MAX][MAX]; 
  int sizeHolder = 0; 
  int sizeBuses = 0; 
  int row = 0, col = 0;
  int menu, id1 = 0, id2 = 0, flag = 0;
  char *name1 = (char*)malloc(sizeof(char));
  char *name2 = (char*)malloc(sizeof(char));
  char *bus[MAX];
  char *file1[MAX];
  char *file2[MAX];
  FILE *f1, *f2;
  f1 = fopen("chieudi.txt", "r"); f2 = fopen("chieuve.txt", "r");
  if (f1 == NULL || f2 == NULL) {
    printf("File not found\n");
    exit(0);
  }
  readFile(f1, f2);				
  copyData(bus, file1, file2);	
  makeBusTable(busTable, sizeBuses, buses);				
  dataProcess(g, busTable);	
  adjustBusVal(busTable);
  do {
    printf("Bus Rout\n");
    printf("1 Hien thi danh sach bus va tuyen\n");
    printf("2 Tim bus\n");
    printf("3 Thoat\n");
    printf("Your choice: "); scanf("%d", &menu);
    switch(menu) {
    case 1:
      printFileData(bus, file1, file2);
      printBuses(busTable);
      break;
    case 2:
      printVertices(g);
      printf("Vi tri hien tai ");scanf("%d", &id1); getchar();
      if (!isdigit(id1)) {
	printf("Invalid\n");
	break;
      }
      if (getVertex(g, id1) == NULL) {
	printf("Khong tim thay\n");
      }
      else {
	printf("Diem den ");scanf("%d", &id2); getchar();
	if (!isdigit(id2)) {
	  printf("Invalid\n");
	  break;
	}
	if (getVertex(g, id2) == NULL) {
	  printf("Khong tim thay\n");
	}     
	else {
	  int output[MAX];
	  int sizeOutput = 0;
	  printShortestPath(g, id1, id2, &sizeOutput, output);
	  printPath(g, busTable, &sizeOutput, output);
	}
      }
      break;
    case 3:
      break;
    default:
      printf("Invalid. Choose again.");
      break;
    }
  }while (menu != 3);
  jrb_free_tree(busTable);
  freeGraph(g);
  return 0;
}
