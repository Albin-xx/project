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
#include <iostream>
#include <fstream>
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
    rootPath = string(path);
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
	size_t id = stringtosizet(str);
	if (id > highestId) {
	  highestId = id;
	}
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
	size_t id = stringtosizet(string(entry->d_name));
	string title;
	ifstream ifs((rootPath + "/" + entry->d_name + "/newsgroup").c_str());
	if (ifs.good()) {
	  string line;
	  while(getline(ifs,line)) {
	    title += line;
	  }
	  v.push_back(Newsgroup(id, title));
	} else {
	  printf("Error, could not read newsgroupdata: %s\n",(rootPath + "/" + entry->d_name + "/newsgroup").c_str());
	}
	ifs.close();
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
    string dirName = rootPath;
    dirName += "/";
    dirName += sizettostring(highestId);
    DIR* prev = opendir(dirName.c_str());
    if(!prev && errno == ENOENT) {
      int res = mkdir(dirName.c_str(), 0700);
      if (res) {
	int pError = errno;
	errno = 0;
	printf("An error occured while making newsgroup directory, id: %d\n", pError);
      } else {
	ofstream ofs((dirName + "/newsgroup").c_str());
	if (ofs.good()) {
	  ofs << name;
	  ++highestId;
	} else {
	  printf("Could not open/create newsgroupdata: %s\n", (dirName + "/newsgroup").c_str());
	}
	ofs.close();	
      }
      closedir(prev);
      errno=0;
    } else {
      closedir(prev);
      errno=0;
      throw NewsgroupExistsException();
    }
  }

  bool DiskDatabase::newsgroupExists(string name) {
    rewinddir(root);
    struct dirent* entry;
    bool rtr = false;
    while((entry = readdir(root))) {
      if (entry->d_type == DT_DIR && entry->d_name[0] != '.') {
	string title;
	ifstream ifs((rootPath + "/" + entry->d_name + "/newsgroup").c_str());
	if (ifs.good()) {
	  string line;
	  while(getline(ifs,line)) {
	    title += line;
	  }
	  if (title.compare(name) == 0) {
	    rtr = true;
	  }
	} else {
	  printf("Error, could not read newsgroupdata: %s\n",(rootPath + "/" + entry->d_name + "/newsgroup").c_str());
	}
	ifs.close();
      }
    }
    if (errno) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    }
    return rtr;
  }
	
  void DiskDatabase::deleteNewsgroup(size_t ID) throw(NoNewsgroupException) {
    string path = rootPath; 
    path += "/";
    path += sizettostring(ID);
    rewinddir(root);
    struct dirent* entry;
    DIR* newsgroupRoot = opendir(path.c_str());
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      if (pError == ENOENT) {
	 throw NoNewsgroupException();
      } else {
	printf("An error occured, id: %d\n", pError);
      }
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
    string path = rootPath; 
    path += "/";
    path += sizettostring(newsgroupID);
    
    rewinddir(root);
    struct dirent* entry;
    vector<Article> v;
    DIR* newsgroupRoot = opendir(path.c_str());
    path += "/";
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
     if (pError == ENOENT) {
	 throw NoNewsgroupException();
      } else {
	printf("An error occured, id: %d\n", pError);
      }
    } else {
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_REG) {
	  string str(entry->d_name);
	  size_t sharp = str.find_last_of("#");
	  size_t id = stringtosizet(str.substr(++sharp));
	  ifstream ifs((path + entry->d_name).c_str());
	  if(ifs.good()) {
	    string title;
	    getline(ifs, title);
	    v.push_back(Article(id, title));
	  } else {
	    printf("Could not read %s, ignoring\n", (path +entry->d_name).c_str());
	  }
	  ifs.close();
	}
      }
      closedir(newsgroupRoot);
      if (errno) {
	int pError = errno;
	errno = 0;
	printf("Error occured while listing articles: %d\n", pError);
      }
    }
    
    return v;
  }

  size_t DiskDatabase::getNextArticleId(string newsgroupPath) {
    size_t highestId = 0;
    struct dirent* entry;
    DIR* newsgroupRoot = opendir(newsgroupPath.c_str());
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      printf("An error occured, id: %d\n", pError);
    } else {
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_REG) {
	  string str(entry->d_name);
	  size_t sharp = str.find_last_of("#");
	  size_t id = stringtosizet(str.substr(++sharp));
	  if (id > highestId) {
	    highestId = id;
	  }
	}
      }
      closedir(newsgroupRoot);
      if (errno) {
	int pError = errno;
	errno = 0;
	printf("Error occured while fetching highest article id: %d\n", pError);
      }
    }
    return ++highestId;
  }

  void DiskDatabase::createArticle(size_t newsgroupID, Article& article) throw(NoNewsgroupException) {
    string path = rootPath; 
    path += "/";
    path += sizettostring(newsgroupID);
    DIR* newsgroupRoot = opendir(path.c_str());
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      if (pError == ENOENT) {
	 throw NoNewsgroupException();
      } else {
	printf("An error occured, id: %d\n", pError);
      }
    }
    closedir(newsgroupRoot);
    errno = 0;
    size_t id = getNextArticleId(path);
    path += "/";
    path += sizettostring(id);
    ofstream ofs(path.c_str());
    if (ofs.bad()) {
      ofs << article.getTitle() << "\n";
      ofs << article.getAuthor() << "\n";
      ofs << article.getText() << "\n";
    } else {
      printf("Could not open %s for writing, error!\n", path.c_str());
    }
    ofs.close();
  }

  void DiskDatabase::deleteArticle(size_t newsgroupID, size_t articleID)  throw(NoNewsgroupException, NoArticleException) {
    string path = rootPath; 
    path += "/";
    path += sizettostring(newsgroupID);
    rewinddir(root);
    struct dirent* entry;
    DIR* newsgroupRoot = opendir(path.c_str());
    path += "/";
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      if (pError == ENOENT) {
	 throw NoNewsgroupException();
      } else {
	printf("An error occured, id: %d\n", pError);
      }
    } else {
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_REG) {
	  string str(entry->d_name);
	  size_t sharp = str.find_last_of("#");
	  size_t id = stringtosizet(str.substr(++sharp));
	  if (id == articleID) {
	    remove((path + entry->d_name).c_str());
	    if (errno) {
	      int pError = errno;
	      errno = 0;
	      printf("fatal error occured while deleting article: %d\n", pError);
	    }
	    closedir(newsgroupRoot);
	    break;
	  }
	}
      }
      closedir(newsgroupRoot);
      if (errno) {
	int pError = errno;
	errno = 0;
	printf("Error occured while deleting articles: %d\n", pError);
      } else {
	throw NoArticleException();
      }
    }
  }

  Article DiskDatabase::getArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException) {
    string path = rootPath; 
    path += "/";
    path += sizettostring(newsgroupID);
    Article* article;
    rewinddir(root);
    struct dirent* entry;
    DIR* newsgroupRoot = opendir(path.c_str());
    path += "/";
    if (!newsgroupRoot) {
      int pError = errno;
      errno = 0;
      if (pError == ENOENT) {
	 throw NoNewsgroupException();
      } else {
	printf("An error occured, id: %d\n", pError);
      }
    } else {
      while((entry = readdir(newsgroupRoot))) {
	if (entry->d_type == DT_REG) {
	  string str(entry->d_name);
	  size_t sharp = str.find_last_of("#");
	  size_t id = stringtosizet(str.substr(++sharp));
	  if (id == articleID) {
	    ifstream ifs((path + entry->d_name).c_str());
	    if(ifs.good()) {
	      string title;
	      getline(ifs, title);
	      string author;
	      getline(ifs, author);
	      string text;
	      string line;
	      while(getline(ifs, line)) {
		text += line;
	      }
	      article = new Article(id, title, author, text);
	    } else {
	      printf("Could not read %s, ignoring\n", (path +entry->d_name).c_str());
	      
	    }
	    ifs.close();
	    closedir(newsgroupRoot);
	    break;
	  }
	}
      }
      closedir(newsgroupRoot);
      if (errno) {
	int pError = errno;
	errno = 0;
	printf("Error occured while reading article: %d\n", pError);
      } else {
	throw NoArticleException();
      }
    }
    return *article;
  }

}
