#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;
const int block_length = 16;

void usage(const char* msg)
{
   cout << "filedump filename blocks" << endl;
   cout << "filename (mandatory) is the name of the file to dump" << endl;
   cout << "blocks (option) is the number of 16 byte blocks to dump" << endl;
   if (nullptr == msg) return;
   cout << endl << "Error! ";
   cout << msg << endl;
}

int read16(ifstream& stm)
{
   if (stm.eof()) return -1;

   int flags = cout.flags();
   cout << hex;

   string line;

   for (int i = 0; i < block_length; ++i)
   {
      // read a single character from the stream
      unsigned char c = stm.get();
      if (stm.eof()) break;

      // need to make sure that all hex are printed 
      // two character padded with zeros
      cout << setw(2) << setfill('0');
      cout << static_cast<short>(c) << " ";
      if (isprint(c) == 0) line += '.';
      else                 line += c;
   }

   // this is the padding between the hex and string
   // it is at least two characters wide
   string padding = " ";
   if (line.length() < block_length)
   {
      // we need to add padding
      padding += string(
         3 * (block_length - line.length()), ' ');
   }

   cout << padding;
   cout << line << endl;

   cout.setf(flags);
   return line.length();
}

// first parameter is the name of a file
// second parameter is the number iof bytes to print
int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      usage("not enough parameters");
      return 1;
   }

   if (argc > 3)
   {
      usage("too many parameters");
      return 1;
   }

   // the second parameter is file name
   string filename = argv[1];

   int blocks = 1;  // default value
   if (3 == argc)
   {
      // we have been passed the number of blocks
      istringstream ss(argv[2]);
      ss >> blocks;
      if (ss.fail() || 0 >= blocks)
      {
         // cannot convert to a number
         usage("second parameter: must be a number, "
               "and greater than zero");
         return 1;
      }
   }

   ifstream file(filename, ios::binary);
   if (!file.good())
   {
      usage("first parameter: file does not exist");
      return 1;
   }

   while (blocks-- && read16(file) != -1);

   file.close();
   return 0;
}
