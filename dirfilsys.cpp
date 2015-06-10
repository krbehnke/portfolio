
// Compile with:  g++ -o dirfilsys.x dirfilsys.cpp

// Problem:
// 
// Implement In-Memory File System
// Following is the Problem Specification as Given by Aeiou:
// 
// ---------------------------------------------------------
// 
// The goal of this exercise is to create a small filesystem
// in memory.  You will need to implement the functions:
// 
// - mkdir - create directory
// - rmdir - remove directory
// - cd    - change directory
// - ls    - list directory
// - touch - create a file
// - rm    - remove a file
// 
// What I would like you to do is to create these functions
// above and you use them in the main of your code.
// 
// This filesystem must be very simple, don't take into
// account permission, privileges, ...  Just write a simple
// data structure and use it with the different functions.
// 
// ---------------------------------------------------------
// 
// Solution:
// 
// As in UNIX, the filesystem will be in the form of a tree.
// In order to keep it simple, each file will be either
// a regular text file or a directory.  All files in a
// directory (whether regular text file or directories) will
// be children of that directory's node, which will be their
// parent.  We will not consider (provide for) the linking
// of files, i.e. the creation of hard or soft (symbolic)
// links whereby a single file may reside in more than one
// directory.  All file nodes will be unique and will exist
// in exactly one directory.

// Completion Status:  The basic task specification has
// been completed.  All "filename" arguments are simple
// filenames, not pathnames.  The next enhancement would
// be to provide for slash-delineated pathnames, so that
// one could "cd" two levels down (e.g.), or "ls" the
// parent directory or a subdirectory of the current
// direction (e.g.).  As it is now, one can only
// create/remove, list, or cd from the current directory.
// A combination of C-style programming (e.g. to parse
// the input line and manipulate strings) is mixed with
// use of the C++ STL.
// 
// Rudimentary error checking is in place, though checks
// for duplicate (identical) filenames (e.g.) have not
// yet been added.  This could perhaps be implemented by
// changing the list of file in a directory from using a
// vector to using a set or map, with filename as key.
// (Or of course all current files in the directory could
// simply be inspected before adding a new one which could
// possibly have the same name.)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

#define  MAX_FILENM_LEN  100

void my_mkdir_and_touch (char *, bool),
     my_rm_and_rmdir (char *, bool),
     my_cd (char *),
     my_ls (char *);

class DIRENT
{
public:

   DIRENT *parent;

   const bool isdir;

   vector<DIRENT *> files;

   DIRENT (const char *filename, bool is_dir) : isdir (is_dir)
   {
      strncpy (filenm, filename, MAX_FILENM_LEN);
      filenm [MAX_FILENM_LEN] = '\0';
   }

   const char * get_filenm () { return filenm; }

private:

   char filenm [MAX_FILENM_LEN + 1];

// vector<char> data;  // We're not manipulating file data.
};

static DIRENT *root, *cwd;

int
main (int argc, char *argv [])
{
   string cmds ("Command Set:  mkdir, rmdir, cd, ls, touch, rm");
   string xit  ("To Quit:      exit");

   if (root == NULL)
   {
      cout << endl << "Entering without a filesystem." << endl;
      cout << "Creating root directory and setting as CWD." << endl;

      root = cwd = new DIRENT ("/", true);

      root->parent = root;
   }

   cout << endl << "Please enter directory/file commands." << endl;
   cout << endl << cmds << endl;
   cout << xit  << endl << endl;

   while (true)
   {
      string input_str;
      int  wordct;
      char verb [20], filename [MAX_FILENM_LEN + 1];

      cout << ">> ";
      getline (cin, input_str);

      if ((wordct = sscanf (input_str.c_str(), "%19s%99s", verb, filename)) == 0)
         continue;

      if (wordct == 1)
         filename [0] = '\0';
      
      if (strcmp (verb, "exit") == 0)
         break;
      
      if (strcmp (verb, "mkdir") == 0)
         my_mkdir_and_touch (filename, true);
      else if (strcmp (verb, "touch") == 0)
         my_mkdir_and_touch (filename, false);
      else if (strcmp (verb, "rm") == 0)
      {
         my_rm_and_rmdir (filename, false);
      }
      else if (strcmp (verb, "rmdir") == 0)
      {
         my_rm_and_rmdir (filename, true);
      }
      else if (strcmp (verb, "cd") == 0)
         my_cd (filename);
      else if (strcmp (verb, "ls") == 0)
         my_ls (filename);
      else
      {
         cout << endl << "Unrecognized command verb:  " << verb << endl;
         cout << endl << cmds << endl;
         cout << xit  << endl << endl;
      }
   }

   cout << endl;
}


void
my_mkdir_and_touch (char *filename, bool isdir)
{
   DIRENT *new_dir = new DIRENT (filename, isdir);

   new_dir->parent = cwd;

   cwd->files.push_back (new_dir);
}


void
my_rm_and_rmdir (char *filename, bool isdir)
{
   vector<DIRENT *>::iterator dirfile;

   for (dirfile = cwd->files.begin(); dirfile != cwd->files.end(); dirfile++)
      if (strcmp ((*dirfile)->get_filenm (), filename) == 0)
         if (isdir && ! (*dirfile)->isdir)
         {
            cout << endl << "\"" << filename << "\" not a directory -- cannot rmdir."
                 << endl << endl;
            return;
         }
         else if ( ! isdir && (*dirfile)->isdir)
         {
            cout << endl << "\"" << filename << "\" is a directory -- cannot rm."
                 << endl << endl;
            return;
         }
         else if ((*dirfile)->files.size() > 0)
         {
            cout << endl << "Directory \"" << filename << "\" not empty - cannot remove."
                 << endl << endl;

            return;
         }
         else
         {
            delete *dirfile;

            cwd->files.erase (dirfile);

            return;
         }

   cout << endl << "File/Directory \"" << filename << "\" not found." << endl << endl;
}


void
my_cd (char *filename)
{
   vector<DIRENT *>::iterator dirfile;

   if (strcmp (filename, ".") == 0)
      return;
   else if (strcmp (filename, "..") == 0)
   {
      cwd = cwd->parent;
      return;
   }
   else if (*filename == '\0')  /* Change to root */
   {
      cwd = root;
      return;
   }

   for (dirfile = cwd->files.begin(); dirfile != cwd->files.end(); dirfile++)
      if (strcmp ((*dirfile)->get_filenm (), filename) == 0)
         if ((*dirfile)->isdir)
         {
            cwd = *dirfile;
            return;
         }
      else
      {
         cout << "\"" << filename << "\" not directory -- can't cd."
                      << endl << endl;
         return;
      }

   cout << "Subdirectory \"" << filename << "\" not found." << endl << endl;
}


void
my_ls (char *filename)
{
   vector<DIRENT *>::iterator dirfile;

   cout << endl << "Directory \"" << cwd->get_filenm () << "\" contents:" << endl;

   for (dirfile = cwd->files.begin(); dirfile != cwd->files.end(); dirfile++)
      cout << ((*dirfile)->isdir ? "(DIR) " : "      ")
           << (*dirfile)->get_filenm () << endl;

   cout << endl;
}

