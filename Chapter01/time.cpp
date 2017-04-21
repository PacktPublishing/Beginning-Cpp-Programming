#include "utils.h"
#include "time.h"

void PrintTime()
{
   std::time_t now = std::time(nullptr);
   std::cout << ", the time and date are " << std::asctime(gmtime(&now)) << std::endl;
}