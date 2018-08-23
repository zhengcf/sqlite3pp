#include <string>

#include "sqlite3pp.h"


class DBClient {
  public:
    DBClient( ) = default;
    DBClient(const std::string& dbpath);
    ~DBClient() {}

  public:
    void SetDBPath(const std::string& dbpath);
    void AddCluster(const std::string& cluster, const int groupcnt, const std::string& nodes);
    void PrintCluster();
    void SetNodes(const std::string &cluster, const int groupid, const std::string &leader);
    void PrintNodes();
  private:
    void InitTable();
    bool TableExist(const std::string& table);


  private:
    std::string             dbpath_;
    sqlite3pp::database     db_;
};

