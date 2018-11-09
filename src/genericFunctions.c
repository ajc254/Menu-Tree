#include <genericFunctions.h>


/*
Function:
  Reads a string until a new line character/EOF is reached.
  String is dynamically sized.

Parameters:
  p_fp: File pointer
  size: Initial length to give the string capacity.

Returns:
  String read
  NULL if out of memory.
*/
extern char *getString (FILE *p_fp,
                 size_t size)
{
  char *str = NULL;
  char ch;
  size_t len = 0;

  /*Initial size manual memory allocation*/
  if (!(str = (char *) malloc(sizeof (char)*size)))
  {
    return NULL;
  }

  /*store characters until new line reached or EOF*/
  while (EOF != (ch = fgetc(p_fp)) && ch != '\n')
  {
    str[len++]=ch;
    /*If at capacity, scale by factor 2*/
    if(len==size)
    {
      if (!(str = realloc(str, sizeof(char)*(size *= 2))))
      {
        return NULL;
      }
    }
  }
  /*Append the null terminator*/
  str[len++]='\0';

  /*Reallocate to the exact size, reducing memory usage*/
  return realloc(str, sizeof(char)*len);
}


/*
Function:
  Reads a number from a file, from a specified number of characters.

Parameters:
  numLength: Number of characters to evaluate for the number
  number: An int pointer to send the output to
  p_fp: The file pointer to read the number from

Returns:
  1 if successful
  0 if not
*/
extern int readNum (unsigned short int numLength,
                      unsigned short int *number,
                      FILE *p_fp)
{
  char ch;
  int idValue = 0;
  int place = 10;
  int i;

  for (i=0; i<numLength-2; i++) {
    place *= 10;
  }

  for (i=0; i<numLength; i++, place/=10)
  {
    if (!(isdigit (ch = fgetc(p_fp))))
    {
      return 0;
    }
    /*Convert character to integer value and add to total value*/
    idValue += (ch-'0')*place;
  }
  *number = idValue;
  return 1;
}
