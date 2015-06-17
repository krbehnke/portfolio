
/********************************************************************/
/**  
/**        compile with:   gcc  -o  filerev.x  filerev.c
/**
/**  It takes the entire series of "tokens" in the input file, and
/**  outputs that series in the exact reverse order, with each token
/**  kept intact.  The file is re-written with the new order.
/**  Running this program on the original file twice will result in
/**  the resultant file being identical to the original.
/**
/**  For purposes of this exercise, I define a token as a sequence
/**  of alphanumeric characters,  or,  each single character
/**  punctuation or whitespace is defined as its own token.
/**
/********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


main (int argc, char *argv [])
{
   char *filearray, *textptr;
   FILE *iofile;
   int   filelen;

   if (argc < 2)
   {
      fprintf (stderr, "\nError -- please supply filename for input/output.\n\n");

      return 0;
   }

   if ((iofile = fopen (argv [1], "r+")) == 0)
   {
      fprintf (stderr, "\nError -- cannot open file %s for read/write.\n\n", argv [1]);

      return 0;
   }

   fseek (iofile, 0, SEEK_END);

   filelen = ftell (iofile);

   if ((filearray = malloc (filelen + 1)) == 0)
   {
      fprintf (stderr, "\nError -- cannot allocate sufficient program memory to hold file.\n\n");

      return 0;
   }

   *filearray = '\0';
   ++filearray;          /* will use to mark start of filearray */

   fseek (iofile, 0, SEEK_SET);

   fread (filearray, 1, filelen, iofile);

   fseek (iofile, 0, SEEK_SET);

   textptr = filearray + filelen - 1;

   while (textptr >= filearray)        /* process words working backwards */
   {                                   /* and write out to original file  */
      if ( ! isalnum (*textptr))
      {
         fwrite (textptr, 1, 1, iofile);

         --textptr;
      }
      else
      {
         char *wordbegin = textptr;
         int   wordlen;

         while (wordbegin > filearray
                && isalnum (* (wordbegin - 1) ) )
            --wordbegin;

         wordlen = textptr - wordbegin + 1;

         fwrite (wordbegin, 1, wordlen, iofile);

         textptr -= wordlen;
      }
   }

   fflush (iofile);
   fclose (iofile);
}

