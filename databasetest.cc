#include "databaseinterface.h"
#include "diskdatabase.h"
#include "database.h"
#include "util.h"
#include <utility>
#include <vector>
#include <string>
using namespace std;
using namespace client_server;

int main(int argc, char* argv[]) {
  if(argc < 2)
  {
      printf("Usage: databasetest storage directory \n");
      printf("storage = 0 to use in-memory database, any other number for disk version\n");
      printf("directory = path to directory where disk version will store information\n");
      return 0;
  }
  bool diskversion = static_cast<bool>(stringtosizet(argv[1]));
  DatabaseInterface* database;
  if(diskversion)
  {
    if (argc > 2) {
      database = new DiskDatabase(argv[2]);
    } else {
      database = new DiskDatabase();
    }      
   }
   else
   {
     database = new Database();
   }
   
  printf(" ------- NEWSGROUPS --------- \n");

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
  
  printf(" -------- ARTICLES ---------- \n");
  try {
    printf("Get non-existing article with id #1 from existing newsgroup #0\n");
    Article art = database->getArticle(0,1);
    printf("Error, could fetch a non-existing article!!\n");
  } catch (NoArticleException ex) {
    printf("Database threw NoArticleException, as expected!\n");
  }

  try {
    printf("Get non-existing article with id #1 from non-existing newsgroup #3\n");
    Article art = database->getArticle(3,1);
    printf("Error, could fetch a non-existing article from a non-existing newsgroup!!!\n");
  } catch (NoNewsgroupException ex) {
    printf("Database threw NoNewsgroupException , as expected!\n");
  }

  try {
    printf("Create article with non-existing newsgroup #3\n");
    database->addArticle(3,string("blub"), string("blab"), string("bleb"));
    printf("Error, could create an article with a non-existing newsgroup!!!\n");
  } catch (NoNewsgroupException ex) {
    printf("Database threw NoNewsgroupException , as expected!\n");
  }

  
  printf("Create article with existing newsgroup #0\n");
  database->addArticle(0,string("blub1"), string("blab1"), string("bleb1"));

  printf("Create article with existing newsgroup #1\n");
  database->addArticle(1,string("blub2"), string("blab2"), string("bleb2"));

  printf("Create article with existing newsgroup #1\n");
  database->addArticle(1,string("blub3"), string("blab3"), string("bleb3"));

  printf("Create article with existing newsgroup #1\n");
  database->addArticle(1,string("blub4"), string("blab4"), string("bleb4"));

  printf("Get first article with id 1 from newsgroup #0\n");
  Article res1 = database->getArticle(0,1);

  printf("Inserted == Returned:\n");
  printf("Title: blub1 == %s \n", res1.getTitle().c_str());
  printf("Author: blab1 == %s \n", res1.getAuthor().c_str());
  printf("Text: bleb1 == %s \n", res1.getText().c_str());

  printf("List articles in newsgroup #1\n");

  vector<Article> articles = database->listArticles(1);
  vector<Article>::iterator ita = articles.begin();
  while(ita != articles.end()) {
    printf("Article #%d\nTitle: %s\nAuthor: %s\nText: %s\n",
	   static_cast<int>((*ita).getID()),
	   (*ita).getTitle().c_str(),
	   (*ita).getAuthor().c_str(),
	   (*ita).getText().c_str());
    ++ita;
  }

  try {
    printf("Delete non-existing article with id #2 from existing newsgroup #0\n");
    database->deleteArticle(0,2);
    printf("Error, could delete a non-existing article!!\n");
  } catch (NoArticleException ex) {
    printf("Database threw NoArticleException, as expected!\n");
  }

  try {
    printf("Delete non-existing article with id #1 from non-existing newsgroup #3\n");
    database->getArticle(3,1);
    printf("Error, could delete a non-existing article from a non-existing newsgroup!!!\n");
  } catch (NoNewsgroupException ex) {
    printf("Database threw NoNewsgroupException , as expected!\n");
  }

  printf("Delete existing article with id #1 from existing newsgroup #0\n");
  database->deleteArticle(0,1);
 
  printf("Delete newsgroups: (with articles!)\n");
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
  
  delete database;
  return 0;
}
