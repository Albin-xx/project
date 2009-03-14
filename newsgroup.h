#ifndef NEWSGROUP_H
#define NEWSGROUP_H

#include <string>
#include <map>
#include "article.h"

namespace client_server {
    

  class NoArticleException{};

  /*
   * Describes a newsgroup. A Newsgroup contains a number of Articles related to the same topic.
   */
  class Newsgroup{

  public:
    typedef std::map<size_t, Article>::const_iterator article_const_iterator;
    typedef std::map<size_t, Article>::iterator article_iterator;

    // Create a new, empty Newsgroup. All Newsgroups must have a name.
    Newsgroup(std::string name);

    // Default constructor 
    Newsgroup();

    const std::string getName() const;

    const size_t getID() const;

    const size_t numberOfArticles() const;

    // Return an iterator to the first article in the newsgroup.
    // The articles are ordered by date (ID number), oldest first.
    article_const_iterator firstArticle() const;

    // Return an iterator to one past the last article in the newsgroup.
    // The articles are ordered by date (ID number), oldest first.
    article_const_iterator endArticles() const;

    // Returns a reference to the article with ID-number ID if it exists in the newsgroup.
    const Article& getArticle(size_t ID) const throw(NoArticleException);

    // Add an article to the newsgroup.
    void addArticle(const Article& article);
    
    // Delete an article from the newsgroup.
    void deleteArticle(size_t ID) throw(NoArticleException);


  private:
    static size_t ID_count;

    // Unique identification number.
    const size_t ID;
    
    // Name - uniqueness to be determined by the database.
    const std::string name;
    
    // A map containing all the Articles.
    std::map<size_t, Article> articles;
  };


} // End namespace
#endif
