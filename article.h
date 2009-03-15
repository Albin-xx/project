#ifndef ARTICLE_H
#define ARTICLE_H

#include<string>

namespace client_server {

  /*
   * Describes an article in a newsgroup.
   */
  class Article{
	
  public:
    // Create an article. All articles must contain a title, author and some text.
    Article(const size_t id, const std::string& title, const std::string& author, const std::string& text);

    Article(const size_t id, const std::string& title);

    size_t getID() const;

    const std::string& getTitle() const;

    const std::string& getAuthor() const;

    const std::string& getText() const;


  private:
    static size_t ID_count;

    // Unique identification number
    size_t ID;

    std::string title;

    std::string author;

    // The article text
    std::string text;
  };

} // End namespace

#endif
