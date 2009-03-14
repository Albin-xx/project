#include "diskdatabase.h"
#include "article.h"
#include "newsgroup.h"
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
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

  void DiskDatabase::openRootDirectory(const char* path) {
    int res = 0;
    root = opendir(path);
    if (!root) {
      printf("Could not open default root-directory \"%s\",\nwill try to create it..\n",
	     path);
      res = mkdir(path, 700);
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
    return 0;
  }
	
  vector<size_t> DiskDatabase::listNewsgroupIDs() const {
    vector<size_t> v;
    return v;
  }

  vector<string> DiskDatabase::listNewsgroupNames() const {
    vector<string> v;
    return v;
  }

  vector<pair<size_t, string> > DiskDatabase::listNewsgroups() const {
    vector<pair<size_t, string> > v;
    return v;
  }

  void DiskDatabase::createNewsgroup(string name) throw(NewsgroupExistsException) {

  }
	
  void DiskDatabase::deleteNewsgroup(size_t ID) throw(NoNewsgroupException) {

  }

  vector<pair<size_t, string> > DiskDatabase::listArticles(size_t newsgroupID)  const throw(NoNewsgroupException) {
    vector<pair<size_t, string> > v;
    return v;
  }

  void DiskDatabase::createArticle(size_t newsgroupID, Article& article) throw(NoNewsgroupException) {

  }

  void DiskDatabase::deleteArticle(size_t newsgroupID, size_t articleID)  throw(NoNewsgroupException, NoArticleException) {

  }

  Article DiskDatabase::getArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException) {
    string a("a");
    string b("b");
    string c("c");
    Article d(a,b,c);
    return d;
  }

}
