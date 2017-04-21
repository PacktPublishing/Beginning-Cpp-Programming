#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
using namespace std;

#define interface struct

interface IWork; // forward decl

interface ISerializer
{
   virtual void write_string(const string& line) = 0;
   virtual void write_worker(IWork *worker) = 0;
   virtual void write_workers(const vector<unique_ptr<IWork>>& workers) = 0;
};

interface ISerializable
{
   virtual void serialize(ISerializer *stm) = 0;
};

interface IDeserializer
{
   virtual string read_string() = 0;
   virtual unique_ptr<IWork> read_worker() = 0;
   virtual void read_workers(vector<unique_ptr<IWork>>& team) = 0;
};

interface IWork
{
   virtual const char* get_name() = 0;
   virtual const char* get_position() = 0;
   virtual void do_work() = 0;
};

interface IManage
{
   virtual const vector<unique_ptr<IWork>>& get_team() = 0;
   virtual void manage_team() = 0;
};

interface IDevelop
{
   virtual void write_code() = 0;
};

class file_writer : public ISerializer
{
   ofstream stm;
public:
   file_writer() = delete;
   file_writer(const char *file) { stm.open(file, ios::out); }
   ~file_writer() { close(); }
   void close() { stm.close(); }
   virtual void write_worker(IWork *worker) override
   {
      ISerializable *object = dynamic_cast<ISerializable*>(worker);
      if (object != nullptr)
      {
         ISerializer* serializer = dynamic_cast<ISerializer*>(this);
         serializer->write_string(typeid(*worker).raw_name());
         object->serialize(serializer);
      }
   }
   virtual void write_workers(
      const vector<unique_ptr<IWork>>& workers) override
   {
      write_string("[[");
      for (unsigned idx = 0; idx < workers.size(); ++idx)
      {
         write_worker(workers[idx].get());
      }
      write_string("]]"); // end marker of team
   }
   virtual void write_string(const string& line) override
   {
      stm << line << endl;
   }
};

class file_reader : public IDeserializer
{
   ifstream stm;
public:
   file_reader() = delete;
   file_reader(const char *file) { stm.open(file, ios::in); }
   ~file_reader() { close(); }
   void close() { stm.close(); }
   virtual unique_ptr<IWork> read_worker() override;
   virtual void read_workers(vector<unique_ptr<IWork>>& team) override;
   virtual string read_string() override
   {
      string line;
      getline(stm, line);
      return line;
   }
};

class worker : public IWork, public ISerializable
{
   string name;
   string position;
public:
   worker() = delete;
   worker(IDeserializer *stm)
   {
      name = stm->read_string();
      position = stm->read_string();
   }
   worker(const char *n, const char *p) : name(n), position(p) {}
   virtual ~worker() {}
   virtual const char* get_name() override { return this->name.c_str(); }
   virtual const char* get_position() override { return this->position.c_str(); }
   virtual void do_work() override { cout << "works" << endl; }
   virtual void serialize(ISerializer *stm) override
   {
      stm->write_string(name);
      stm->write_string(position);
   }
};

class manager : public worker, public IManage
{
   vector<unique_ptr<IWork>> team;
public:
   manager() = delete;
   manager(const char *n, const char* p) : worker(n, p) {}
   manager(IDeserializer* stm) : worker(stm)
   {
      stm->read_workers(this->team);
   }
   const vector<unique_ptr<IWork>>& get_team() { return team; }
   virtual void manage_team() override { cout << "manages a team" << endl; }
   void add_team_member(IWork* worker)
   {
      team.push_back(unique_ptr<IWork>(worker));
   }
   virtual void do_work() override { this->manage_team(); }
   virtual void serialize(ISerializer* stm) override
   {
      worker::serialize(stm);
      stm->write_workers(team);
   }
};

class project_manager : public manager
{
public:
   project_manager() = delete;
   project_manager(const char *n) : manager(n, "Project Manager") {}
   project_manager(IDeserializer* stm) : manager(stm) {}
   virtual void manage_team() override { cout << "manages team of developers" << endl; }
   virtual void serialize(ISerializer* stm) override
   {
      manager::serialize(stm);
   }
};

class cpp_developer
   : public worker, public IDevelop
{
public:
   cpp_developer() = delete;
   cpp_developer(const char *n) : worker(n, "C++ Dev") {}
   cpp_developer(IDeserializer* stm) : worker(stm) {}
   void write_code() { cout << "Writing C++ ..." << endl; }
   virtual void do_work() override { this->write_code(); }
   virtual void serialize(ISerializer* stm) override
   {
      worker::serialize(stm);
   }
};

class database_admin
   : public worker, public IDevelop
{
public:
   database_admin() = delete;
   database_admin(const char *n) : worker(n, "DBA") {}
   database_admin(IDeserializer* stm) : worker(stm) {}
   void write_code() { cout << "Writing SQL ..." << endl; }
   virtual void do_work() override { this->write_code(); }
   virtual void serialize(ISerializer* stm) override
   {
      worker::serialize(stm);
   }
};

void print_team(IWork *mgr)
{
   cout << mgr->get_name() << " is " << mgr->get_position() << " and ";
   IManage *manager = dynamic_cast<IManage*>(mgr);
   if (manager != nullptr)
   {
      cout << "manages a team of: " << endl;
      const vector<unique_ptr<IWork>>& team = manager->get_team();
      for (unsigned idx = 0; idx < team.size(); ++idx)
      {
         cout << team[idx]->get_name() << " "
            << team[idx]->get_position() << endl;
      }
   }
   else
   {
      cout << "is not a manager" << endl;
   }
}

unique_ptr<IWork> file_reader::read_worker()
{
   string type = read_string();
   if (type == "[[") type = read_string();
   if (type == "]]") return nullptr;

   if (type == typeid(worker).raw_name())
   {
      return unique_ptr<IWork>(dynamic_cast<IWork*>(new worker(this)));
   }
   if (type == typeid(project_manager).raw_name())
   {
      return unique_ptr<IWork>(dynamic_cast<IWork*>(new project_manager(this)));
   }
   if (type == typeid(cpp_developer).raw_name())
   {
      return unique_ptr<IWork>(dynamic_cast<IWork*>(new cpp_developer(this)));
   }
   if (type == typeid(database_admin).raw_name())
   {
      return unique_ptr<IWork>(dynamic_cast<IWork*>(new database_admin(this)));
   }
   return nullptr;
}

void file_reader::read_workers(vector<unique_ptr<IWork>>& team)
{
   while (true)
   {
      unique_ptr<IWork> worker = read_worker();
      if (!worker) break;
      team.push_back(move(worker));
   }
}

void serialize(const char* file)
{
   project_manager pm("Agnes");
   pm.add_team_member(new cpp_developer("Bill"));
   pm.add_team_member(new cpp_developer("Chris"));
   pm.add_team_member(new cpp_developer("Dave"));
   pm.add_team_member(new database_admin("Edith"));
   print_team(&pm);

   cout << endl << "writing to " << file << endl;

   file_writer writer(file);
   ISerializer* ser = dynamic_cast<ISerializer*>(&writer);
   ser->write_worker(&pm);
   writer.close();
}

void deserialize(const char* file)
{
   file_reader reader(file);
   while (true)
   {
      unique_ptr<IWork> worker = reader.read_worker();
      if (worker) print_team(worker.get());
      else break;
   }
   reader.close();
}

void usage()
{
   cout << "usage: team_builder file [r|w]" << endl;
   cout << "file is the name of the file to read or write" << endl;
   cout << "provide w to file the file (the default)" << endl;
   cout << "        r to read the file" << endl;
}

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      usage();
      return 1;
   }

   bool write = true;
   const char *file = argv[1];
   if (argc > 2) write = (argv[2][0] == 'w');

   cout << (write ? "Write " : "Read ") << file << endl << endl;

   if (write) serialize(file);
   else deserialize(file);
   return 0;
}