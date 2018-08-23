#include "sqlite.hh"
#include <iostream>

DBClient::DBClient(const std::string &dbpath)
  : dbpath_(dbpath), db_(dbpath.c_str())
{
  InitTable();
}


void DBClient::SetDBPath(const std::string &dbpath)
{
  if (dbpath_ == dbpath)
    return;
  dbpath_ = dbpath;
  auto rc = db_.connect(dbpath_.c_str(), SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
  if (rc != SQLITE_OK)
    throw sqlite3pp::database_error(db_.error_msg());
  InitTable();
}

#if 0
const std::array<std::pair<std::string, string>, 3> TABLE = {
    std::pair<string, string>{"t_cluster", "cluster "},
    std::pair<string, string>{"t_cluster1", "cluster "},
    std::pair<string, string>{"t_cluster2", "cluster "}
};

for(auto &a : TABLE){
    cout<<a.first<<a.second<<endl;
}
#endif

const std::string T_CLUSTER = "t_cluster";
const std::string T_NODES   = "t_nodes";

const std::string TABLE[][2] = {
    {T_CLUSTER,   "cluster VARCHAR(255) PRIMARY KEY NOT NULL, groupcnt INTEGER, nodes  VARCHAR(255)"},
    {T_NODES,     "cluster VARCHAR(255) NOT NULL, groupid  INTEGER, leader VARCHAR(255), PRIMARY KEY(cluster, groupid)"}
};

//for(auto &a : TABLE2){
//    cout<<a[0]<<a[1]<<endl;
//}


void DBClient::InitTable()
{
  for(auto& t : TABLE) {
    if (TableExist(t[0]))
      continue;
    auto rc = db_.executef("CREATE TABLE %s ( %s ) ", t[0].c_str(), t[1].c_str());
    if (rc != SQLITE_OK)
      throw sqlite3pp::database_error(db_.error_msg());
  }
}

bool DBClient::TableExist(const std::string &table)
{
  sqlite3pp::query query(db_, "SELECT count(*) FROM sqlite_master WHERE type='table' AND name=?");
  query.bind(1, table, sqlite3pp::nocopy);
  auto i = query.begin();
  return (1 == (*i).get<int>(0));
}

void DBClient::AddCluster(const std::string &cluster, const int groupcnt, const std::string &nodes)
{
  static std::string cmd_str = std::string("REPLACE INTO ") + T_CLUSTER +  " VALUES ( ?, ?, ? )";
  sqlite3pp::command cmd(db_, cmd_str.c_str());
  //cmd.bind(1, cluster, sqlite3pp::nocopy);
  //cmd.bind(2, groupcnt);
  //cmd.bind(3, nodes, sqlite3pp::nocopy);
  cmd.binder()<<cluster<<groupcnt<<nodes; 
  auto rc = cmd.execute();
  if (rc != SQLITE_OK)
    throw sqlite3pp::database_error(db_.error_msg());
}

void DBClient::PrintCluster()
{
  static std::string cmd_str = std::string("SELECT cluster, groupcnt, nodes FROM ")
                               + T_CLUSTER;
  sqlite3pp::query   qry(db_, cmd_str.c_str());
  std::cout<<"\n\n-------------"<<T_CLUSTER<<"-------------"<<std::endl;
  for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
    char const* cluster; int slotcnt; char const *nodes;
    std::tie(cluster, slotcnt, nodes) = (*i).get_columns<char const*, int, char const*>(0, 1, 2);
    std::cout << cluster << "\t" << slotcnt << "\t" << nodes << std::endl;
  }
}


void DBClient::SetNodes(const std::string &cluster, const int groupid, const std::string &leader)
{
  static std::string cmd_str = std::string("REPLACE INTO ") + T_NODES
                               +  "(cluster, groupid, leader) VALUES ( ?, ?, ? )";
  sqlite3pp::command cmd(db_, cmd_str.c_str());
  cmd.binder()<<cluster<<groupid<<leader;
  auto rc = cmd.execute();
  if (rc != SQLITE_OK)
    throw sqlite3pp::database_error(db_.error_msg());
}

void DBClient::PrintNodes()
{
  static std::string cmd_str = std::string("SELECT cluster, groupid, leader FROM ")
                               + T_NODES + " ORDER BY cluster, groupid";
  sqlite3pp::query   qry(db_, cmd_str.c_str());
  std::cout<<"\n\n-------------"<<T_NODES<<"-------------"<<std::endl;
  for (sqlite3pp::query::iterator i = qry.begin(); i != qry.end(); ++i) {
    char const* cluster; int groupid; char const *leader;
    std::tie(cluster, groupid, leader) = (*i).get_columns<char const*, int, char const*>(0, 1, 2);
    std::cout << cluster << "\t" << groupid << "\t" << leader << std::endl;
  }
}

