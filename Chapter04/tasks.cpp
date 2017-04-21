#include <iostream>
#include <string> // this is needed to use string with cout
using namespace std;

struct task
{
   task* pNext;
   string description;
};

task* pHead = nullptr;     // this is the start of the list
task* pCurrent = nullptr;  // when constructing the list, this is the last item

// Add a new task to the list with the specified description
void queue_task(const string& name)
{
   task* pTask = new task;
   pTask->description = name;
   pTask->pNext = nullptr;

   if (nullptr == pHead)
   {
      // The list is empty, this this task *is* the list
      pHead = pTask;
      pCurrent = pTask;
   }
   else
   {
      // Add the new task to the end of the list
      pCurrent->pNext = pTask;
      pCurrent = pTask;
   }
}

// Remove the task at the beginning of the list and update pHead
// to the new start of the list
bool remove_head()
{
   if (nullptr == pHead) return false;
   task* pTask = pHead;
   pHead = pHead->pNext;
   delete pTask;
   return (pHead != nullptr);
}

// Delete all items in the list, starting at the item at the begining
void destroy_list()
{
   while (remove_head());
}

// Execute the specified task and return a pointer to the next task
task *execute_task(const task* pTask)
{
   if (nullptr == pTask) return nullptr;
   cout << "executing " << pTask->description << endl;
   return pTask->pNext;
}

// Iterate through the entire lisrt, executing all tasks
void execute_all()
{
   task* pTask = pHead;
   while (pTask != nullptr)
   {
      pTask = execute_task(pTask);
   }
}

// Find the task item with the specified description, or nullptr
// if the task does not exist
task *find_task(const string& name)
{
   task* pTask = pHead;

   while (nullptr != pTask)
   {
      if (name == pTask->description) return pTask;
      pTask = pTask->pNext;
   }

   return nullptr;
}

// Insert a new task with the specified description, after the 
// specified task. If the task pointer is nullptr then place the new
// task at the head of the list
void insert_after(task* pTask, const string& name)
{
   task* pNewTask = new task;
   pNewTask->description = name;

   if (nullptr != pTask)
   {
      // Insert the new task after the specified task
      pNewTask->pNext = pTask->pNext;
      pTask->pNext = pNewTask;
   }
   else
   {
      // Insert the new task at the beginning of the list
      pNewTask->pNext = pHead;
      pHead = pNewTask;
   }
}

int main()
{
   // Queue the tasks to do to wall paper a room
   queue_task("remove old wallpaper");
   queue_task("fill holes");
   queue_task("size walls");
   queue_task("hang new wallpaper");

   // Oops, forgot to paint woodwork
   task* pTask = find_task("fill holes");
   if (nullptr != pTask)
   {
      insert_after(pTask, "paint woodwork");
   }
   // Oops, forgot to cover the furniture befopre doing any work
   insert_after(nullptr, "cover furniture");

   execute_all();
   destroy_list();
   return 0;
}