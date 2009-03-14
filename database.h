#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include "newsgroup.h"
#include "article.h"
#include "databaseinterface.h"

namespace client_server {

  /*
   * Class Database.
   */
  class Database : public DatabaseInterface{
	
  public:

    typedef std::map<size_t, Newsgroup>::const_iterator newsgroup_const_iterator;
    typedef std::map<size_t, Newsgroup>::iterator newsgroup_iterator;

    Database(){}

    // Return the number of newsgroups in the database
    size_t numberOfNewsgroups() const;
	
    // Return a list of the identification numbers of the newsgroups
    // in the database in increasing order.
    std::vector<size_t> listNewsgroupIDs() const;

    // Return a list of the names of the newsgroups in the database,
    // ordered by identification numbers.
    std::vector<std::string> listNewsgroupNames() const;

    // Return a list of the identification numbers and names of the
    // newsgroups in the database ordered by increasing ID numbers
    std::vector<std::pair<size_t, std::string> > listNewsgroups() const;

    // Create a newsgroup.
    void createNewsgroup(std::string name) throw(NewsgroupExistsException);
	
    // Delete a newsgroup.
    void deleteNewsgroup(size_t ID) throw(NoNewsgroupException);

    // Return a list of identification numbers and titles of the articles
    // in the newsgroup with the specified ID number if it exists.
    std::vector<std::pair<size_t, std::string> > listArticles(size_t newsgroupID)  const throw(NoNewsgroupException);

    // Add an article to the specified newsgroup.
    void createArticle(size_t newsgroupID, Article& article) throw(NoNewsgroupException);

    // Delete an article in the specified newsgroup.
    void deleteArticle(size_t newsgroupID, size_t articleID)  throw(NoNewsgroupException, NoArticleException);

    // Returns a specified article in a newsgroup.  
    const Article& getArticle(size_t newsgroupID, size_t articleID) const throw(NoNewsgroupException, NoArticleException);

  private:
    std::map<size_t, Newsgroup> groups;


  };

} // End namespace

#endif