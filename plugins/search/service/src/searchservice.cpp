#include <cctype>
#include <algorithm>
#include <sstream>

#include <service/searchservice.h>

namespace cc
{
namespace service
{
namespace search
{

SearchServiceHandler::SearchServiceHandler(
  std::shared_ptr<odb::database> db_,
  std::shared_ptr<std::string> datadir_,
  const cc::webserver::ServerContext& context_)
    : _projectHandler(db_, datadir_, context_),
      _searchDb(*datadir_ + "/search", Xapian::DB_OPEN)
{
}

std::vector<LineMatch> SearchServiceHandler::getMatches(
  const std::string& fileContent_,
  const std::string& token_,
  const core::FileId& fileId_)
{
  // TODO: What if token is found multiple times in a line?
  std::vector<LineMatch> lineMatches;

  std::istringstream content(fileContent_);
  std::string line;

  for (int lineNo = 1; std::getline(content, line); ++lineNo)
  {
    std::string::const_iterator pos = std::search(
      line.begin(), line.end(), token_.begin(), token_.end(),
      [](char c1, char c2) { return std::tolower(c1) == std::tolower(c2); });

    if (pos == line.end())
      continue;

    LineMatch match;

    match.text = line;
    match.range.file = fileId_;

    match.range.range.startpos.line = lineNo;
    match.range.range.startpos.column = pos - line.begin() + 1;
    match.range.range.endpos.line = lineNo;
    match.range.range.endpos.column
      = match.range.range.startpos.column + token_.length();

    lineMatches.push_back(match);
  }

  return lineMatches;
}

void SearchServiceHandler::getSearchTypes(std::vector<SearchType>& return_)
{
  ::cc::service::search::SearchType type;

  type.__set_id(::cc::service::search::SearchOptions::SearchInSource);
  type.__set_name("Text search");
  return_.push_back(type);

  type.__set_id(::cc::service::search::SearchOptions::SearchInDefs);
  type.__set_name("Definition search");
  return_.push_back(type);

  type.__set_id(::cc::service::search::SearchOptions::SearchForFileName);
  type.__set_name("File name search");
  return_.push_back(type);

  type.__set_id(::cc::service::search::SearchOptions::FindLogText);
  type.__set_name("Log search");
  return_.push_back(type);
}

void SearchServiceHandler::pleaseStop()
{

}

void SearchServiceHandler::search(
  SearchResult& return_,
  const SearchParams& params_)
{
  Xapian::QueryParser queryParser;
  Xapian::Query query = queryParser.parse_query(params_.query);
  Xapian::Enquire enquire(_searchDb);
  enquire.set_query(query);

  Xapian::doccount offset = 0, max = 10;
  if (params_.__isset.range)
  {
    offset = params_.range.start;
    max = params_.range.maxSize;
  }

  Xapian::MSet result = enquire.get_mset(offset, max);
  for (Xapian::MSetIterator i = result.begin(), e = result.end(); i != e; ++i)
  {
    SearchResultEntry entry;

    _projectHandler.getFileInfo(entry.finfo, i.get_document().get_data());

    std::string fileContent;
    _projectHandler.getFileContent(fileContent, entry.finfo.id);

    for (Xapian::TermIterator ti = enquire.get_matching_terms_begin(i),
      tiEnd = enquire.get_matching_terms_end(i); ti != tiEnd; ++ti)
    {
      entry.matchingLines = getMatches(fileContent, *ti, entry.finfo.id);
    }

    return_.results.push_back(entry);
  }

  return_.totalFiles = 10;
}

void SearchServiceHandler::searchFile(
  FileSearchResult& return_,
  const SearchParams& params_)
{
}

void SearchServiceHandler::suggest(
  SearchSuggestions& return_,
  const SearchSuggestionParams& params_)
{
}

}
}
}
