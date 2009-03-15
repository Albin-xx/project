#include "databaseinterface.h"
#include "diskdatabase.h"
#include <utility>
#include <vector>
#include <string>
using namespace std;
using namespace client_server;

int main(int argc, char* argv[]) {
  DatabaseInterface* database; 
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

  try {
    printf("Trying to create a new newsgroup with existing name \"test1\"\n");
    database->createNewsgroup("test1");
    printf("Error, could create a duplicate \"test1\" newsgroup!\n");
  } catch (NewsgroupExistsException ex) {
    printf("Database threw NewsgroupExistsException, as expected!\n");
  }
  
  printf("Newsgrouplist:\n");
  vector<Newsgroup> result = database->listNewsgroups();
  vector<Newsgroup>::const_iterator it = result.begin();
  while(it != result.end()) {
    printf("#%d : %s\n", static_cast<int>((*it).getID()),
	   (*it).getName().c_str());
    ++it;
  }

  printf("Delete newsgroups:\n");
  result = database->listNewsgroups();
  it = result.begin();
  while(it != result.end()) {
    printf("Delete #%d \n", static_cast<int>((*it).getID()));
    database->deleteNewsgroup((*it).getID());
    ++it;
  }
  
  try {
    printf("Trying to delete a newsgroup with non-existing name id 1\n");
    database->deleteNewsgroup(1);
    printf("Error, could delete a non-existing newsgroup!\n");
  } catch (NoNewsgroupException ex) {
    printf("Database threw NoNewsgroupException, as expected!\n");
  }

  size = database->numberOfNewsgroups();
  printf("Number of newsgroups %d, expected %d\n", static_cast<int>(size), 0);
  
  return 0;
}
