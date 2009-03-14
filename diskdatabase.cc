#include "diskdatabase.h"
#include "article.h"
#include "util.h"
#include "newsgroup.h"
#include <errno.h>
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
    setHighestId();
  }

  DiskDatabase::DiskDatabase(char* directory) {
    openRootDirectory(directory);
    setHighestId();
  }

  void DiskDatabase::openRootDirectory(const char* path) {
    int res = 0;
    root = opendir(path);
    rootPath = path;
    if (!root) {
      printf("Could not open default root-directory \"%s\",\nwill try to create it..\n",
	     path);
      errno = 0;
      res = mkdir(path, 0700);
      if (!res) {
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
    printf("Working directory \"%s\" opened, database operational!\n", path);
  }

  void DiskDatabase::setHighestId() {
    struct dirent* entry;
    highestId = 0;
    while((entry = readdir(root))) {
      if (entry->d_name[0] != '.' && entry->d_type == DT_DIR) {
	string str(entry->d_name);
	size_t sharp = str.find_last_of("#");
	str.erase(0, ++sharp);
	highestId = stringtosizet(str);
      }
    }
    if (errno) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    }
  }

  DiskDatabase::~DiskDatabase() {
    closedir(root);
  }

  size_t DiskDatabase::numberOfNewsgroups() const {
    rewinddir(root);
    struct dirent* entry;
    size_t groups=0;
    while((entry = readdir(root))) {
      if (entry->d_name[0] != '.' && entry->d_type == DT_DIR) {
	++groups;
      }
    }
    if (errno) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    }
    return groups;
  }
	
  vector<size_t> DiskDatabase::listNewsgroupIDs() const {
    vector<size_t> v;
    struct dirent* entry;
    while((entry = readdir(root))) {
      if (entry->d_name[0] != '.' && entry->d_type == DT_DIR) {
	string str(entry->d_name);
	size_t sharp = str.find_last_of("#");
	str.erase(0, ++sharp);
	v.push_back(stringtosizet(str));
      }
    }
    if (errno) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    }
    return v;
  }

  vector<string> DiskDatabase::listNewsgroupNames() const {
    vector<string> v;
    rewinddir(root);
    struct dirent* entry;
    while((entry = readdir(root))) {
      if (entry->d_name[0] != '.' && entry->d_type == DT_DIR) {
	string str(entry->d_name);
	size_t sharp = str.find_last_of("#");
	str.erase(--sharp);
	v.push_back(str);
      }
    }
    if (errno) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    }
    return v;
  }

  vector<pair<size_t, string> > DiskDatabase::listNewsgroups() const {
    vector<pair<size_t, string> > v;
    rewinddir(root);
    struct dirent* entry;
    while((entry = readdir(root))) {
      if (entry->d_name[0] != '.' && entry->d_type == DT_DIR) {
	string str(entry->d_name);
	size_t sharp = str.find_last_of("#");
	size_t id = stringtosizet(str.substr(++sharp));
	str.erase(--sharp);
	v.push_back(make_pair(id, str));
      }
    }
    if (errno) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    }
    return v;
  }

  void DiskDatabase::createNewsgroup(string name) throw(NewsgroupExistsException) {
     rewinddir(root);
     name.insert(0, 1, '/');
     name.insert(0, rootPath);
     name += "#";
     name += sizettostring(highestId);
     DIR* prev = opendir(name.c_str());
     if(!prev && errno == ENOENT) {
       int res = mkdir(name.c_str(), 0700);
       if (res) {
	 int pError = errno;
	 printf("An error occured, id: %d\n", pError);
       } else {
	 ++highestId;
       }
       errno=0;
     } else {
       errno=0;
       closedir(prev);
       throw NewsgroupExistsException();
     }
  }

  const char* DiskDatabase::getNewsgroupPath(size_t id) {
    rewinddir(root);
    const char* path = 0;
    struct dirent* entry;
    while((entry = readdir(root))) {
      if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
	string str(entry->d_name);
	size_t sharp = str.find_last_of("#");
	size_t entryId = stringtosizet(str.substr(++sharp));
	if (entryId == id) {
	  str.insert(0, "/");
	  str.insert(0, rootPath);
	  path = str.c_str();
	  break;
	}
      }
    }
    if (errno) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    }
    return path;
  }
	
  void DiskDatabase::deleteNewsgroup(size_t ID) throw(NoNewsgroupException) {
    const char* path = getNewsgroupPath(ID);
    rewinddir(root);
    struct dirent* entry;
    if(!path) {
      throw NoNewsgroupException();
    }
    
    DIR* newsgroupRoot = opendir(path);
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    } else {
      int res = 0;
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
	  printf("Error, newsgroup contains directories! %s\n", entry->d_name);
	} else if (entry->d_type == DT_REG) {
	  string filePath(path);
	  filePath += "/";
	  filePath += entry->d_name;
	  res = remove(filePath.c_str()); 
	  if (errno || res) {
	    int pError = errno;
	    errno = 0;
	    printf("Could not remove, result: %d\nError id: %d\n", res, pError);
	    res = 0;
	  }
	}
      }
      closedir(newsgroupRoot);
      res = rmdir(path);
      if (errno || res) {
	int pError = errno;
	errno = 0;
	printf("Could not remove newsgroup directory, result: %d\n Error id: %d\n",res, pError);
	res = 0;
      }
    }
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
