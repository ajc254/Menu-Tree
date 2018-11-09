#ifndef __MENUTREES__
#define __MENUTREES__

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <genericFunctions.h>

#endif

struct menuTreeData
{
  unsigned short int nodeID;
  char *textLabel;
};
typedef struct menuTreeData DATA;


struct dataLink
{
  DATA  *data;
  struct dataLink   *next;
  struct dataLink   *subTreeRoot;
};
typedef struct dataLink LINK;


struct treeTuple
{
  LINK *dataRoot;
  LINK *root;
  int numberOfNodes;
};
typedef struct treeTuple TUPLE;


TUPLE constructTree (FILE **, char *);
DATA *createData (unsigned short int, char *);
LINK *createLink (DATA *);
DATA *getData (unsigned short int, LINK *);
void append (LINK *, LINK **);
void insertChild (DATA *, unsigned short int, LINK **, char *);
void printTree (LINK *, int, int, int[], int);
void freeLinkList (LINK *);
void freeData (LINK *);
void freeTreeLinks (LINK *);
void cleanUp (LINK *dataItemsRoot, LINK *treeRoot, FILE *fp);
