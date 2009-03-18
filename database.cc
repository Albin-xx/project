#include "database.h"
#include <functional>
#include <algorithm>

namespace client_server {

    Database::Database()
    {
	newsgroupCount = 0;
    }

    // Return the number of newsgroups in the database
    size_t Database::numberOfNewsgroups() const
    {
	return groups.size();
    }
    
    // Return a list of the identification numbers and names of the
    // newsgroups in the database ordered by increasing ID numbers
    std::vector<Newsgroup> Database::listNewsgroups() const
    {
	std::vector<Newsgroup> res;
	std::map<size_t, Newsgroup>::const_iterator it;
	for(it = groups.begin(); it != groups.end(); ++it)
	{
	    res.push_back(it->second);
	}
	
	return res;
    }

    // Used in the function below. Somewhat complicated because the Newsgroup names should be unique as well and
    // not just the ID numbers.
    struct compName : public std::binary_function
    <std::pair<size_t, Newsgroup>, std::string, bool> {
	bool operator() (const std::pair<size_t, Newsgroup>& p, const std::string& name) const {
	    return p.second.getName() == name;
	}
    };

    // Create a newsgroup.
    void Database::createNewsgroup(std::string name) throw(NewsgroupExistsException)
    {
	// Have to check if the name has been used earlier. Map:s find is only defined for searching through keys. 
	std::map<size_t, Newsgroup>::iterator res = find_if(groups.begin(), groups.end(), bind2nd(compName(), name));

	if(res != groups.end())
	    throw NewsgroupExistsException();
	
	Newsgroup group(newsgroupCount++, name);
	groups.insert(std::make_pair(group.getID(), group));
    }


    // Delete a newsgroup.
    void Database::deleteNewsgroup(size_t ID) throw(NoNewsgroupException)
    {
	std::map<size_t, Newsgroup>::iterator res = groups.find(ID);

	if(res == groups.end())
	    throw NoNewsgroupException();
	else
	    groups.erase(res);
    }



    // Return a list of identification numbers and titles of the articles
    // in the newsgroup with the specified ID number if it exists.
    std::vector<Article> Database::listArticles(size_t newsgroupID) throw(NoNewsgroupException)
    {
	std::map<size_t, Newsgroup>::iterator res = groups.find(newsgroupID);

	if(res == groups.end())
	    throw NoNewsgroupException();

	std::vector<Article> out;
	std::map<size_t, Article>::const_iterator it;

	for(it = res->second.firstPair(); it != res->second.endPair(); ++it)
	    out.push_back(it->second);

	return out;
    }


    // Add an article to the specified newsgroup.
  void Database::addArticle(size_t newsgroupID, const std::string& title, const std::string& author, const std::string& text) throw(NoNewsgroupException)
    {
      // FIXME o_O
      Article article(0, title, author, text);
	std::map<size_t, Newsgroup>::iterator res = groups.find(newsgroupID);

	if(res == groups.end())
	    throw NoNewsgroupException();

	res->second.addArticle(article);
    }

    // Delete an article in the specified newsgroup.
    void Database::deleteArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException)
    {
	std::map<size_t, Newsgroup>::iterator res = groups.find(newsgroupID);

	if(res == groups.end())
	    throw NoNewsgroupException();

	res->second.deleteArticle(articleID);
    }


    // Returns a specified article in a newsgroup.  
    Article Database::getArticle(size_t newsgroupID, size_t articleID) throw(NoNewsgroupException, NoArticleException)
    {
	std::map<size_t, Newsgroup>::const_iterator res = groups.find(newsgroupID);
	
	if(res == groups.end())
	    throw NoNewsgroupException();
	else
	    return res->second.getArticle(articleID);	
    }
    
} // End namespace

