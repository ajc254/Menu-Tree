#include <menuTrees.h>

int main(int argc, char **argv)
{
  FILE *fp;

  if (argc < 2)
  {
      printf ("%s\n", "No file Given");
      return EXIT_FAILURE;
   }
   if ((fp = fopen (argv[1],"r")) == NULL)
   {
      printf ("\n\nFile Error:\nUnable to read file: %s\n%s.\n",
              argv[1], strerror (errno));
      return EXIT_FAILURE;
   }

   int i;

   fprintf (stderr, "Parameter listing:\n");
   for (i=1; i<argc; i++)
   {
     fprintf (stderr, "%4d: input file name:  %s\n", i, argv[i]);
   }
   fprintf (stderr, "\nData values read from file:");
   TUPLE tree = constructTree(&fp, argv[1]);
   int pastParents[tree.numberOfNodes];
   printTree (tree.root, 0, 1, pastParents, 0);
   cleanUp  (tree.dataRoot, tree.root, fp);
   return 0;
}
