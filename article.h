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
	Article(const std::string& title, const std::string& author, const std::string& text);

	size_t getID() const;

	const std::string& getTitle() const;

	const std::string& getAuthor() const;

	const std::string& getText() const;


    private:
	static size_t ID_count;

	// Unique identification number
	const size_t ID;

	const std::string title;

	const std::string author;

	// The article text
	const std::string text;
    };

} // End namespace

#endif
