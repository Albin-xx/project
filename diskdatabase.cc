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

  vector<Newsgroup> DiskDatabase::listNewsgroups() const {
    vector<Newsgroup> v;
    rewinddir(root);
    struct dirent* entry;
    while((entry = readdir(root))) {
      if (entry->d_name[0] != '.' && entry->d_type == DT_DIR) {
	string str(entry->d_name);
	size_t sharp = str.find_last_of("#");
	size_t id = stringtosizet(str.substr(++sharp));
	str.erase(--sharp);
	v.push_back(Newsgroup(id, str));
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
    if (newsgroupExists(name)) {
      throw NewsgroupExistsException();
    }
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
	errno = 0;
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

  string DiskDatabase::getNewsgroupPath(size_t id) {
    rewinddir(root);
    string path;
    struct dirent* entry;
    while((entry = readdir(root))) {
      if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
	string str(entry->d_name);
	size_t sharp = str.find_last_of("#");
	size_t entryId = stringtosizet(str.substr(++sharp));
	if (entryId == id) {
	  str.insert(0, "/");
	  str.insert(0, rootPath);
	  path = str;
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

  bool DiskDatabase::newsgroupExists(string name) {
    rewinddir(root);
    struct dirent* entry;
    while((entry = readdir(root))) {
      if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
	string str(entry->d_name);
	size_t sharp = str.find_last_of("#");
	str.erase(sharp);
	if (str.compare(name) == 0) {
	  return true;
	}
      }
    }
    if (errno) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    }
    return false;
  }
	
  void DiskDatabase::deleteNewsgroup(size_t ID) throw(NoNewsgroupException) {
    string path(getNewsgroupPath(ID));
    if(path.length() == 0) {
      throw NoNewsgroupException();
    }
    rewinddir(root);
    struct dirent* entry;
    printf("innan opendir Path: %s\n", path.c_str());
    DIR* newsgroupRoot = opendir(path.c_str());
    printf("Innan if Path: %s\n", path.c_str());
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
      res = rmdir(path.c_str());
      if (errno || res) {
	int pError = errno;
	errno = 0;
	printf("Could not remove newsgroup directory, result: %d\nError id: %d\n",res, pError);
	res = 0;
      }
    }
  }

  vector<Article> DiskDatabase::listArticles(size_t newsgroupID) throw(NoNewsgroupException) {
    string path(getNewsgroupPath(newsgroupID));
    if(path.length() == 0) {
      throw NoNewsgroupException();
    }
    
    rewinddir(root);
    struct dirent* entry;
    vector<Article> v;
    DIR* newsgroupRoot = opendir(path.c_str());

    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    } else {
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_REG) {
	  size_t id = stringtosizet(string(entry->d_name));
	  /*

	    KEEP ON SWINGIN DUUUDE..
	  */
	  v.push_back(Article(id, ""));
	}
      }
      closedir(newsgroupRoot);
      if (errno) {
	int pError = errno;
	errno = 0;
	printf("Error occured while searching for directory: %d\n", pError);
      }
    }
    
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
    Article d(1, a,b,c);
    return d;
  }

}
