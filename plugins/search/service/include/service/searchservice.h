#ifndef CC_SERVICE_SEARCHSERVICE_H
#define CC_SERVICE_SEARCHSERVICE_H

#include <vector>
#include <memory>

#include <odb/database.hxx>
#include <xapian.h>
#include <projectservice/projectservice.h>
#include <webserver/servercontext.h>
#include <SearchService.h>

namespace cc
{
namespace service
{
namespace search
{

class SearchServiceHandler : virtual public SearchServiceIf
{
public:
  // TODO: Server context should contain odb::database and the location of
  // project directory.
  SearchServiceHandler(
    std::shared_ptr<odb::database> db_,
    std::shared_ptr<std::string> datadir_,
    const cc::webserver::ServerContext& context_);

  void search(
    SearchResult& return_,
    const SearchParams& params_) override;

  // TODO: File search should go through project.thrift.
  void searchFile(
    FileSearchResult& return_,
    const SearchParams& params_) override;

  void getSearchTypes(std::vector<SearchType>& return_) override;

  void pleaseStop() override;

  void suggest(
    SearchSuggestions& return_,
    const SearchSuggestionParams& params_) override;

private:
  static std::vector<LineMatch> getMatches(
    const std::string& fileContent_,
    const std::string& token_,
    const core::FileId& fileId_);

  core::ProjectServiceHandler _projectHandler;
  Xapian::Database _searchDb;
};

}
}
}

#endif
