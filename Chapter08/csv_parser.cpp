#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <list>

using namespace std;
using vec_str = vector<string>;
using list_str = list<string>;
using vec_list = vector<list_str>;

list_str parse_line(const string& line)
{
   list_str data;
   string::const_iterator it = line.begin();
   string item;
   bool bQuote = false;
   bool bDQuote = false;
   do
   {
      switch (*it)
      {
      case '\'':
         if (bDQuote) item.push_back(*it);
         else
         {
            if ((it + 1) != line.end() && *(it + 1) == '\'')
            {
               item.push_back(*it);
               ++it;
            }
            else
            {
               bQuote = !bQuote;
               if (bQuote) item.clear();
            }
         }
         break;
      case '"':
         if (bQuote) item.push_back(*it);
         else
         {
            if ((it + 1) != line.end() && *(it + 1) == '"')
            {
               item.push_back(*it);
               ++it;
            }
            else
            {
               bDQuote = !bDQuote;
               if (bDQuote) item.clear();
            }
         }
         break;
      case ',':
         if (bQuote || bDQuote)  item.push_back(*it);
         else                    data.push_back(move(item));
         break;
      default:
         item.push_back(*it);
      };
      ++it;
   } while (it != line.end());
   data.push_back(move(item));

   return data;
}

void usage()
{
   cout << "usage: csv_parser file" << endl;
   cout << "where file is the path to a csv file" << endl;
}

int main(int argc, const char* argv[])
{
   if (argc <= 1)
   {
      usage();
      return 1;
   }

   ifstream stm;
   stm.open(argv[1], ios_base::in);
   if (!stm.is_open())
   {
      usage();
      cout << "cannot open " << argv[1] << endl;
      return 1;
   }

   vec_str lines;
   for (string line; getline(stm, line); )
   {
      if (line.empty()) continue;
      lines.push_back(move(line));
   }
   stm.close();

   vec_list parsed;
   for (string& line : lines)
   {
      parsed.push_back(move(parse_line(line)));
   }

   int count = 0;
   for (list_str row : parsed)
   {
      cout << ++count << "> ";
      for (string field : row)
      {
         cout << field << " ";
      }
      cout << endl;
   }

   return 0;
}