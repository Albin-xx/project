#include "diskdatabase.h"
#include "dirent.h"
#include "dir.h"
#include <vector>
#include <utility>
using namespace std;

namespace client_server {
  DiskDatabase::DiskDatabase() {
    openRootDirectory("newsgroups");
  }

  DiskDatabase::DiskDatabase(string directory) {
    openRootDirectory(directory.c_str());
  }

  void DiskDatabase::openRootDirectory(char* path) {
    int res = 0;
    root = opendir(path);
    if (!root) {
      printf("Could not open default root-directory \"%s\",\nwill try to create it..\n",
	     path);
      res = mkdir(path);
      if (res) {
	printf("Successfully created root-directory, trying to open it..\n");
	root = opendir(path);
	if (!root) {
	  printf("Fatal error, could still not open, buhu..\n");
	  exit(1);
	}
      } else {
	printf("Fatal error, could not create directory, terminating..\n");
	exit(1);
      }
    }
    printf("Working directory \"%s\" opened, server running!\n", path);
  }

  size_t DiskDatabase::numberOfNewsgroups() const {

  }
	
  vector<size_t> DiskDatabase::listNewsgroupIDs() const {

  }

  vector<string> DiskDatabase::listNewsgroupNames() const {

  }

  vector<pair<size_t, string> > DiskDatabase::listNewsgroups() const {

  }

  void DiskDatabase::createNewsgroup(string name) throw(NewsgroupExistsException) {

  }
	
  void DiskDatabase::deleteNewsgroup(size_t ID) throw(NoNewsgroupException) {

  }

  vector<pair<size_t, string> > DiskDatabase::listArticles(size_t newsgroupID)  const throw(NoNewsgroupException) {

  }

  void DiskDatabase::createArticle(size_t newsgroupID, Article& article) throw(NoNewsgroupException) {

  }

  void DiskDatabase::deleteArticle(size_t newsgroupID, size_t articleID)  throw(NoNewsgroupException, NoArticleException) {

  }

  const Article& DiskDatabase::getArticle(size_t newsgroupID, size_t articleID) const throw(NoNewsgroupException, NoArticleException) {

  }

}
