#include <iostream>
#include "sqlite.hh"


int main(int argc, char *argv[])
{
  DBClient db("testdb.db");
  db.AddCluster("cluster_one", 20, "127,0,0,1:2379,127.0..0.1:2380");
  db.PrintCluster();
  db.SetNodes("cluster_one", 0, "127,0,0,1:2379");
  db.PrintNodes();
  db.SetNodes("cluster_one", 1, "127,0,0,1:2379");
  db.PrintNodes();
  db.SetNodes("cluster_two", 0, "127,0,0,1:2379");
  db.PrintNodes();
  db.SetNodes("cluster_two", 2, "127,0,0,1:2379");
  db.PrintNodes();

  return 0;
}
