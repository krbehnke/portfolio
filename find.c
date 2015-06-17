
/********************************************************************/
/**
/**  compile/link with:  gcc -ofind.x find.c -pthreads
/**
/**  This works, though not all parallelization optimizations
/**  have been realized.  I.e. there are some serializations (critical
/**  sections) which could be optimized away in a more refined version
/**  of the program.  E.g. the scandir() function is not declared to
/**  be thread-safe, so we only execute it in a critical section.
/**
/**  Additionally, we'd want to do more error checking and diagnostics
/**  reporting in a completely robust version of this utility.
/**
/********************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#define  MAXPATHLEN  1024
#define  MAXFILENAME  512

void search_dir_recursively (char *pathname);

pthread_mutex_t  scandir_mutex = PTHREAD_MUTEX_INITIALIZER,
                 printf_mutex  = PTHREAD_MUTEX_INITIALIZER,
                 active_mutex  = PTHREAD_MUTEX_INITIALIZER;

int  num_threads_active = 1;

pthread_cond_t  threads_active = PTHREAD_COND_INITIALIZER;

pthread_t  thread;

pthread_attr_t  detached_attr;

char filename [MAXFILENAME];


main (int argc, char *argv [])
{
   if (argc < 3)
   {
      fprintf (stderr, "\nUsage: myfind <dir> <filename>\n\n");

      return 0;
   }

   strcpy (filename, argv [2]);

   if (chdir (argv [1]) != 0)
   {
      fprintf (stderr, "\nUnable to search directory specified\n\n");

      return 0;
   }

   pthread_attr_init (&detached_attr);
   pthread_attr_setdetachstate (&detached_attr, PTHREAD_CREATE_DETACHED);

   search_dir_recursively (argv [1]);

   pthread_mutex_lock (&active_mutex);    /* Wait for all threads to exit */

   while (num_threads_active > 0)
   {
      pthread_cond_wait (&threads_active, &active_mutex);
   }

  /* We can exit main now, knowing that all threads have exited */

}         /* END  of  main ()  routine */


void search_dir_recursively (char *pathname)
{
   struct dirent **entries;
   int             num_entries,
                   i;

   pthread_mutex_lock (&scandir_mutex);   /* Not threadsafe -- serialize this call */

   num_entries = scandir (pathname, &entries, 0, 0);

   pthread_mutex_unlock (&scandir_mutex);

   for (i = 0; i < num_entries; i++)
   {
      struct stat statbuf;
      char        dirfile [MAXPATHLEN],
                 *new_dir;

      if (entries [i] -> d_ino == 0)                 /* Is it a non-empty entry ? */
         continue;

      if (strcmp (entries [i] -> d_name, ".") == 0        /* Don't re-search this */
          || strcmp (entries [i] -> d_name, "..") == 0)   /* dir or its parent    */
         continue;

      sprintf (dirfile, "%s/%s", pathname, entries [i] -> d_name);

      if (stat (dirfile, &statbuf) == -1                 /* Is it a directory ? */
          || statbuf.st_mode & S_IFDIR == 0)
         continue;

      new_dir = malloc (MAXPATHLEN);
      strcpy (new_dir, dirfile);

      pthread_mutex_lock (&active_mutex);    /* To update num threads active */

      ++num_threads_active;

      pthread_mutex_unlock (&active_mutex);

      pthread_create (&thread,                    /* We don't care about the */
                      0,                          /* thread ID -- it will signal */
             (void *) search_dir_recursively,     /* the main routine when it exits */
             (void *) new_dir);
   }

   for (i = 0; i < num_entries; i++)
      if (strcmp (entries [i] -> d_name, filename) == 0)
      {
         pthread_mutex_lock (&printf_mutex);    /* Don't want interleaved output */
                                                /* do output in critical section */
         printf ("%s/%s\n", pathname, filename);
         fflush (stdout);

         pthread_mutex_unlock (&printf_mutex);

         break;
      }

   pthread_mutex_lock (&active_mutex);    /* To update num threads active */

   if (--num_threads_active == 0)
      pthread_cond_signal (&threads_active);

   pthread_mutex_unlock (&active_mutex);
}

