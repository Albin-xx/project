#ifndef DATABASE_INTERFACE_H
#define DATABASE_INTERFACE_H

#include <vector>
#include "newsgroup.h"
#include "article.h"

namespace client_server {

  
  class NewsgroupExistsException{};
  class NoNewsgroupException{};

  /*
   * Defines the base class for the Database classes.
   */
  class DatabaseInterface{
	
  public:

    // Return the number of newsgroups in the database
    virtual size_t numberOfNewsgroups() const;
	
    // Return a list of the identification numbers of the newsgroups
    // in the database in increasing order.
    virtual std::vector<size_t> listNewsgroupIDs() const;

    // Return a list of the names of the newsgroups in the database,
    // ordered by identification numbers.
    virtual std::vector<std::string> listNewsgroupNames() const;

    // Return a list of the identification numbers and names of the
    // newsgroups in the database ordered by increasing ID numbers
    virtual std::vector<std::pair<size_t, std::string> > listNewsgroups() const;

    // Create a newsgroup.
    virtual void createNewsgroup(std::string name) throw(NewsgroupExistsException);
	
    // Delete a newsgroup.
    virtual void deleteNewsgroup(size_t ID) throw(NoNewsgroupException);

    // Return a list of identification numbers and titles of the articles
    // in the newsgroup with the specified ID number if it exists.
    virtual std::vector<std::pair<size_t, std::string> > listArticles(size_t newsgroupID)  const throw(NoNewsgroupException);

    // Add an article to the specified newsgroup.
    virtual void createArticle(size_t newsgroupID, Article& article) throw(NoNewsgroupException);

    // Delete an article in the specified newsgroup.
    virtual void deleteArticle(size_t newsgroupID, size_t articleID)  throw(NoNewsgroupException, NoArticleException);

    // Returns a specified article in a newsgroup.  
    virtual const Article& getArticle(size_t newsgroupID, size_t articleID) const throw(NoNewsgroupException, NoArticleException);

  private:

  };

} // End namespace

#endif
