#include "article.h"

namespace client_server {

  size_t Article::ID_count = 0;  // Initialize the identification number counter

  Article::Article(const size_t id, const std::string& title, const std::string& author, const std::string& text)
    : ID(id), title(title), author(author), text(text)
  {}

  Article::Article(const size_t id, const std::string& title)
    : ID(id), title(title), author(), text() {}

  size_t Article::getID() const
  {
    return ID;
  }

  const std::string& Article::getTitle() const
  {
    return title;
  }

  const std::string& Article::getAuthor() const
  {
    return author; 
  }

  const std::string& Article::getText() const 
  { 
    return text;
  }


} // End namespace
