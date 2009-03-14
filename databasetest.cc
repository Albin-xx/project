#include "diskdatabase.h"
#include <utility>
#include <vector>
#include <string>
using namespace std;
using namespace client_server;

int main(int argc, char* argv[]) {
  DiskDatabase* database; 
  if (argc > 1) {
    database = new DiskDatabase(argv[1]);
  } else {
    database = new DiskDatabase();
  }

  size_t size = database->numberOfNewsgroups();
  printf("Number of newsgroups %d, expected %d\n", static_cast<int>(size), 0);
  
  database->createNewsgroup("test1");
  printf("Create newsgroup \"test1\"\n");
  
  size = database->numberOfNewsgroups();
  printf("Number of newsgroups %d, expected %d\n", static_cast<int>(size), 1);
  
  database->createNewsgroup("test2");
  printf("Create newsgroup \"test2\"\n");

  size = database->numberOfNewsgroups();
  printf("Number of newsgroups %d, expected %d\n", static_cast<int>(size), 2);
  
  printf("Newsgrouplist:\n");
  vector<pair<size_t, string> > result = database->listNewsgroups();
  vector<pair<size_t, string> >::const_iterator it = result.begin();
  while(it != result.end()) {
    printf("#%d : %s\n", static_cast<int>((*it).first),
	   (*it).second.c_str());
    ++it;
  }

  printf("Delete newsgroups:\n");
  result = database->listNewsgroups();
  it = result.begin();
  while(it != result.end()) {
    printf("Delete #%d \n", static_cast<int>((*it).first));
    database->deleteNewsgroup((*it).first);
    ++it;
  }

  size = database->numberOfNewsgroups();
  printf("Number of newsgroups %d, expected %d\n", static_cast<int>(size), 0);
  
  return 0;
}
