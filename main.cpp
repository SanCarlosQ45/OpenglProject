// $Id: main.cpp,v 1.13 2014-05-08 18:32:56-07 - - $
/*
* Opengl and polymorphism Assignment
* ---------------------------------------
* Josh Lohse
* 05/13/2014
* ---------------------------------------
* A program that draws various shapes on
* screen. These shapes come from an input
* file, and these shapes can be moved on
* screen via keyboard commands.
*/

#include <fstream>
#include <iostream>
#include <unistd.h>
#include <vector>
using namespace std;

#include "debug.h"
#include "graphics.h"
#include "interp.h"
#include "util.h"

//
// Parse a file.  Read lines from input file, parse each line,
// and interpret the command.
//

/*
Program first takes file and creates a map containing all of the
shapes that the program will display. So the file is scanned and 
each line is interprited to generate a list of shapes. 

*/


/**
* void parsefile (const string& infilename, istream& infile)
*
* Method that parses the input file gathering all of the 
* various shapes and their colors.
*
* @param infilename filename of the file to be parsed
* @param infile istream object to use on infilename file
*/
void parsefile (const string& infilename, istream& infile) {
   interpreter::shape_map shapemap;
   interpreter interp;
   for (int linenr = 1;; ++linenr) {
      try {
         string line;
         getline (infile, line);
         if (infile.eof()) break;
         if (line.size() == 0) continue;
         for (;;) {
            DEBUGF ('m', line);
            int last = line.size() - 1;
            if (line[last] != '\\') break;
            line[last] = ' ';
            string contin;
            getline (infile, contin);
            if (infile.eof()) break;
            line += contin;
         }
         interpreter::parameters words = split (line, " \t");
         if (words.size() == 0 or words.front()[0] == '#') continue;
         DEBUGF ('m', words);
         interp.interpret (words);
      }catch (runtime_error error) {
         complain() << infilename << ":" << linenr << ": "
                    << error.what() << endl;
      }
   }
   DEBUGF ('m', infilename << " EOF");
}


/**
* void scan_options (int argc, char** argv) {
*
* Method takes the input arguements and checks them
* to adjust window size
*
* @param argc number of arguements in argv array
* @param argv char* array containing input arguements at runtime
*/

void scan_options (int argc, char** argv) {
   opterr = 0;
   for (;;) {
      int option = getopt (argc, argv, "@:w:h:");
      if (option == EOF) break;
      switch (option) {
         case '@':
            debugflags::setflags (optarg);
            break;
         case 'w':
            window::setwidth (stoi (optarg));
            break;
         case 'h':
            window::setheight (stoi (optarg));
            break;
         default:
            complain() << "-" << (char) optopt << ": invalid option"
                       << endl;
            break;
      }
   }
}

//
// Main function.  Iterate over files if given, use cin if not.
//
int main (int argc, char** argv) {
   sys_info::execname (argv[0]);
   scan_options (argc, argv);
   vector<string> args (&argv[optind], &argv[argc]); //place split arguements into a vector
   if (args.size() == 0) {
      parsefile ("-", cin);
   }else if (args.size() > 1) {
      cerr << "Usage: " << sys_info::execname() << "-@flags"
           << "[filename]" << endl;
   }else {
      const string infilename = args[0];
      ifstream infile (infilename.c_str());
      if (infile.fail()) {
         syscall_error (infilename);
      }else {
         DEBUGF ('m', infilename << "(opened OK)");
         parsefile (infilename, infile);
         // fstream objects auto closed when destroyed
      }
   }
   int status = sys_info::exit_status();
   if (status != 0) return status;
   window::main(); //Enter opengl main
   return 0;
}

