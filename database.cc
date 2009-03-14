#include "database.h"


namespace client_server {

    typedef std::map<size_t, Newsgroup>::const_iterator newsgroup_const_iterator;
    typedef std::map<size_t, Newsgroup>::iterator newsgroup_iterator;


    // Return the number of newsgroups in the database
    size_t Database::numberOfNewsgroups() const
    {
	return groups.size();
    }

    // Returns a specified article in a newsgroup.  
    const Article& Database::getArticle(size_t newsgroupID, size_t articleID) const throw(NoNewsgroupException, Newsgroup::NoArticleException)
    {
	newsgroup_const_iterator res = groups.find(newsgroupID);

	if(res == groups.end())
	    throw NoNewsgroupException();
	else
	    return res->second.getArticle(articleID);	
    }

} // End namespace
