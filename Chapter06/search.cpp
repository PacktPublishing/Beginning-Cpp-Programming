#include <iostream>
#include <string>
#include <io.h>
using namespace std;

// class that encapsulates searching for a file with a pattern
// using the C runtime functions
class file_search
{
   // class used to manage the lifetime of a search handle 
   // used by the C runtime _findfirst/_findnext functions
   class search_handle
   {
      // this is the data member that will be managed
      intptr_t handle;
      // public here means public to the containing class
   public:
      search_handle() : handle(-1) {}
      search_handle(intptr_t p) : handle(p) {}
      void operator=(intptr_t p) { handle = p; }
      // do not allow copying
      search_handle(search_handle& h) = delete;
      void operator=(search_handle& h) = delete;
      // move semantics allowed
      search_handle(search_handle&& h) { close(); handle = h.handle; }
      void operator=(search_handle&& h) { close(); handle = h.handle; }
      // allow implicit conversion to a bool ro check that the
      // data member is valid
      operator bool() const { return (handle != -1); }
      // allow implict conversion to an intptr_t
      operator intptr_t() const { return handle; }
      // allow callers to explictly release the handle
      void close() { if (handle != -1) _findclose(handle); handle = 0; }
      // allow automatic release of the handle
      ~search_handle() { close(); }
   };

   search_handle handle;
   string search;

   // these are the public interface of the class
public:
   // we can create objects with a C or C++ string
   file_search(const char* str) : search(str) {}
   file_search(const string& str) : search(str) {}
   // get access to the search string
   const char* path() const { return search.c_str(); }
   // explictly close the search
   void close() { handle.close(); }
   // get the next value in the search
   bool next(string& ret)
   {
      _finddata_t find{};
      if (!handle)
      {
         // the first call to get the search handle
         handle = _findfirst(search.c_str(), &find);
         if (!handle) return false;
      }
      else
      {
         // subsequent calls on the search handle
         if (-1 == _findnext(handle, &find)) return false;
      }

      // return the name of the file found
      ret = find.name;
      return true;
   }
};

void usage()
{
   cout << "usage: search pattern" << endl;
   cout << "pattern is the file or folder to search for "
      << "with or without wildcards * and ?" << endl;
}

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      usage();
      return 1;
   }

   file_search files(argv[1]);
   cout << "searching for " << files.path() << endl;
   string file;
   while (files.next(file))
   {
      cout << file << endl;
   }

   return 0;
}
