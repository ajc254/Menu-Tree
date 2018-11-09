#include <menuTrees.h>

/*
Function:
  Processes a file and constructs a tree, rigorous error trapping.

Parameters:
  p_fp: File pointer
  p_fileName: string filename

Returns:
  TUPLE structure: (data items root,
                    tree root,
                    number of nodes)
*/
TUPLE constructTree (FILE **p_fp,
                     char *p_fileName)
{
  char dataType;
  char success;
  char *textLabel;
  char bRead = 0;
  unsigned short int nodeID;
  unsigned short int childNodeID;
  unsigned short int numberOfNodes = 0;
  size_t lineNumber = 1;
  DATA *data;
  LINK *link;
  LINK *dataItemsRoot = NULL;
  LINK *treeRoot = createLink (createData (0, NULL));

  /*Read characters until end of file*/
  while (EOF != (dataType = fgetc(*p_fp)))
  {

    if (dataType == 'A')
    {
      if (bRead)
      {
        fprintf(stderr, "\n\nError #%d:\nIn file: %s   On line: %zu\n"
                        "'A' structure read after 'B' instruction.\n"
                        "All A's must precede B's.\n",
                        ++errno, p_fileName, lineNumber);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }
      if (!(readNum (4, &nodeID, *p_fp)))
      {
        fprintf(stderr, "\n\nError #%d:\nIn file: %s   On line: %zu\n"
               "Invalid ID format given. An ID must be 4 digits.\n",
               ++errno, p_fileName, lineNumber);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }
      if (nodeID == 0)
      {
        fprintf (stderr, "\n\nError #%d\nIn file: %s   On line: %zu\n"
                         "Node ID cannot be 0 (between 0 and 9999 inclusive)\n",
                         ++errno, p_fileName, lineNumber);
      cleanUp (dataItemsRoot, treeRoot, *p_fp);
      exit(-1);
      }
      /*Read the menu description and store as a string (dynamically sized)*/
      if (!(textLabel = getString (*p_fp, 20)))
      {
        fprintf(stderr, "\n\nError #%d\n:Out of memory!\n"
               "Text label for node with ID: %04hu could not be stored.\n",
               ++errno, nodeID);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }
      fprintf (stderr, "\n%5hu: %s", nodeID, textLabel);
      if (!(data = createData (nodeID, textLabel)))
      {
        fprintf(stderr, "\n\nError #%d\n:Out of memory!\n", ++errno);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }
      if (!(link = createLink (data)))
      {
        fprintf(stderr, "\n\nError #%d\n:Out of memory!\n", ++errno);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }
      /*Create a link in the list referencing the data read*/
      append (link, &dataItemsRoot);
      lineNumber++;
    }

    else if (dataType == 'B')
    {
      if (!bRead)
      {
        bRead = 1;
      }
      if (!(readNum (4, &childNodeID, *p_fp)))
      {
        fprintf(stderr, "\n\nError #%d:\nIn file: %s   On line: %zu\n"
               "Invalid child ID format given. An ID must be 4 digits.\n",
               ++errno, p_fileName, lineNumber);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }
      if (!(readNum (4, &nodeID, *p_fp)))
      {
        fprintf(stderr, "\n\nError #%d:\nIn file: %s   On line: %zu\n"
               "Invalid ID format given. An ID must be 4 digits.\n",
               ++errno, p_fileName, lineNumber);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }

      if (nodeID == childNodeID) {
        fprintf (stderr, "\n\nError #%d\nIn file: %s   On line: %zu\n"
               "Cannot insert child with ID: %04hu as a child of itself.\n"
               "Parent and child IDs must be different.\n",
               ++errno, p_fileName, lineNumber, childNodeID);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }
      if (nodeID != 0)
      {
        fprintf (stderr, "\n%8s %d, parent %d", "child", childNodeID, nodeID);
      }
      /*Find the data for the child node ID*/
      data = getData (childNodeID, dataItemsRoot);
      if (!data)
      {
        fprintf (stderr, "\n\nError #%d\nIn file: %s   On line: %zu\n"
               "Cannot insert child into tree. No such node with ID %04hu"
               " declared.\n", ++errno, p_fileName, lineNumber, childNodeID);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }
      /*Insert a link into the tree pointing to the correct data,
        as a child of the correct parent*/
      success = 0;
      insertChild ( data, nodeID, &treeRoot, &success);
      if (!success)
      {
        fprintf (stderr, "\n\nError #%d\nIn file: %s   On line: %zu\n"
               "parent with ID: %04hu not currently existent in tree.\n"
               "Therefore cannot insert node with ID: %04hu as a child.\n",
               ++errno, p_fileName, lineNumber, nodeID, childNodeID);
        cleanUp (dataItemsRoot, treeRoot, *p_fp);
        exit(-1);
      }
      numberOfNodes++;
    }
    /*In case there is a blank line*/
    else if (dataType == '\n')
    {
      lineNumber++;
    }
    /*Report any other unexpected read that is not whitespace*/
    else if (!isspace (dataType))
    {
      fprintf (stderr, "\nWarning. In file: %s   On line: %zu\n"
             "Unexpected character: %c Found.\n",
             p_fileName, lineNumber, dataType);
    }
  }
  TUPLE t = {dataItemsRoot, treeRoot, numberOfNodes};
  return t;
}


/*
Function:
  Create a structure to hold the physical data of a particular menu item

Parameters
  p_nodeID: The ID of the menu item
  p_textLabel: The string description of the menu item

Returns:
  DATA structure pointer holding the physical data
  NULL if out of memory
*/
DATA *createData (unsigned short int p_nodeID,
                  char *p_textLabel)
{
  DATA *node = NULL;
  if (!(node = (DATA *)malloc (sizeof(DATA))))
  {
      return NULL;
  }

   node -> nodeID = p_nodeID;
   node -> textLabel = p_textLabel;

   return node;
}


/*
Function:
  Create a LINK to represent a data record, with the ability to link to other
  links. Allowing construction of linked lists or trees.

Parameters
  p_data: Pointer to the data to be represented

Returns:
  LINK pointer holding a pointer reference to corresponding data
  NULL if out of memory
*/
LINK *createLink (DATA *p_data)
{
  LINK *node = NULL;
  if (!(node = (LINK *)malloc(sizeof(LINK))))
  {
      return NULL;
  }
   node -> data = p_data;
   node -> next = NULL;
   node -> subTreeRoot = NULL;

   return node;
}


/*
Function:
  Retrieve a stored physical data record structure.

Parameters
  nodeID: The ID of the data record wished to be retrieved
  dataItemsRoot: Root of the linked list holding references to the data.

Returns:
  DATA structure pointer holding the physical data
  NULL if data not found
*/
DATA *getData (unsigned short int nodeID,
               LINK *dataItemsRoot)
{
  /*Traverse list until correct ID found*/
  while (dataItemsRoot && dataItemsRoot -> data -> nodeID != nodeID)
  {
    dataItemsRoot =  dataItemsRoot -> next;
  }
  if (!dataItemsRoot)
  {
    return NULL;
  }
  return dataItemsRoot -> data;
}


/*
Function:
  Append a LINK to the end of a linked list

Parameters
  item: LINK to be added
  root: Root of the linked list

Returns:
  (nothing)
*/
void append (LINK *item,
             LINK **root)
{
  if (!*root)   /*if empty*/
  {
    *root = item;
    return;
  }
  while ((*root) -> next )   /*find the end*/
  {
    root = &(*root) -> next;
  }
  (*root) -> next = item;
}


/*
Function:
  Insert a LINK into a tree as a child of all instances of a specified parent.

Parameters
  childData: Pointer to the physical data for the LINK to represent
  parentID:  ID of the parent
  treeRoot:  Pointer to the tree root LINK
  success:   Pointer to a character, set to 1 if at least one child was added.

Returns:
  (nothing)
*/
void insertChild (DATA *childData,
                  unsigned short int parentID,
                  LINK **treeRoot,
                  char *success)
{
  if ((*treeRoot) -> data -> nodeID == parentID)
  {
    /*Add to the end of the parent's subTree*/
    append (createLink (childData), &((*treeRoot) -> subTreeRoot));
    *success = 1;
  }
  /*If subroot exists*/
  if (treeRoot = &(*treeRoot) -> subTreeRoot)
  {
    /*Depth first recursive search on all subRoot members*/
    while (*treeRoot)
    {
      insertChild (childData, parentID, treeRoot, success);
      treeRoot = &((*treeRoot) -> next);
    }
  }
}


/*
Function:
  Print a menu tree in the correct format

Parameters
  treeRoot:      Pointer to the root of the tree
  depth:         Current recursive depth (initialise at 1)
  childNo:       Current child number (initialise as 1)
  pastParents:   Array of integers to represent past parent ordering values
                 (initialse as empty)
  parentsLength: Size of the pastParents array (initialise as 0)


Returns:
  (nothing)
*/
void printTree (LINK *treeRoot,
                int depth,
                int childNo,
                int pastParents[],
                int parentsLength)
{
  int i;

  if (treeRoot = treeRoot -> subTreeRoot)
  {
    while (treeRoot)
    {
      /*Indentation*/
      for (i=0; i<depth; i++)
      {
        printf("   ");
      }
      /*Menu ordering*/
      for (i=0; i<parentsLength; i++)
      {
        printf("%d.", pastParents[i]);
      }
      printf("%d %s\n", childNo, treeRoot -> data -> textLabel);
      pastParents[parentsLength] = childNo;

      printTree (treeRoot, depth+1, 1, pastParents, parentsLength+1);
      treeRoot = treeRoot -> next;
      childNo++;
    }
  }
}


/*
Function:
  Free all physical data stored, and the linked list referencing them.

Parameters
  root: Pointer to the root of the linked list referencing the data structures.

Returns:
  (nothing)
*/
void freeData (LINK *root)
{
   while (root)
   {
     LINK *current = root;
     root = root -> next;
     free (current -> data -> textLabel);
     free (current -> data);
     free (current);
   }
}


/*
Function:
  Free the links of the tree

Parameters
  root: Pointer to the root of the tree

Returns:
  (nothing)
*/
void freeTreeLinks (LINK *treeRoot)
{
  LINK *subRoot;
  LINK *currentChild;

  subRoot = treeRoot -> subTreeRoot;
  free (treeRoot);

  while (subRoot)
  {
    currentChild = subRoot;
    subRoot = subRoot -> next;
    freeTreeLinks (currentChild);
  }
}


/*
Function:
  Compacts together all the freeing functions, frees the virtual root
  and closes the open file. Ensuring no memory leaks.

Parameters
  dataItemsRoot: Pointer to the root of the linked list referencing
                 the data structures.
  treeRoot: Pointer to the root of the tree
  fp: Pointer to the open file

Returns:
  (nothing)
*/
void cleanUp (LINK *dataItemsRoot,
              LINK *treeRoot,
              FILE *fp)
{
  freeData (dataItemsRoot);
  DATA *virtualRootData = treeRoot -> data;
  freeTreeLinks (treeRoot);
  free (virtualRootData);
  fclose (fp);
}
