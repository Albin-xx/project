#ifndef DATABASE_INTERFACE_H
#define DATABASE_INTERFACE_H

#include <vector>
#include "newsgroup.h"
#include "article.h"
#include <string>

namespace client_server {

  
  class NewsgroupExistsException{};
  class NoNewsgroupException{};

  /*
   * Defines the base class for the Database classes.
   */
  class DatabaseInterface{
	
  public:

    virtual ~DatabaseInterface() {};

    // Return the number of newsgroups in the database
    virtual size_t numberOfNewsgroups() const = 0;

    // Return a list of the identification numbers and names of the
    // newsgroups in the database ordered by increasing ID numbers
    virtual std::vector<Newsgroup> listNewsgroups() const = 0;

    // Create a newsgroup.
    virtual void createNewsgroup(std::string name) throw(NewsgroupExistsException) = 0;
	
    // Delete a newsgroup.
    virtual void deleteNewsgroup(size_t ID) throw(NoNewsgroupException) = 0;

    // Return a list of identification numbers and titles of the articles
    // in the newsgroup with the specified ID number if it exists.
    virtual std::vector<Article> listArticles(size_t newsgroupID) throw(NoNewsgroupException) = 0;

    // Add an article to the specified newsgroup.
    virtual void addArticle(size_t newsgroupID, const std::string& title, const std::string& author, const std::string& text) throw(NoNewsgroupException) = 0;

    // Delete an article in the specified newsgroup.
    virtual void deleteArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException) = 0;

    // Returns a specified article in a newsgroup.  
    virtual Article getArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException) = 0;

  private:

  };

} // End namespace

#endif
