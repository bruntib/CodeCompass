#include <fstream>
#include <functional>

#include <boost/filesystem.hpp>

#include <xapian.h>

#include <util/logutil.h>
#include <util/parserutil.h>

#include <model/file.h>
#include <model/file-odb.hxx>

#include <parser/sourcemanager.h>
#include <searchparser/searchparser.h>

#include "ctagsResultParser.h"

namespace cc
{
namespace parser
{

namespace fs = boost::filesystem;

SearchParser::SearchParser(ParserContext& ctx_) : AbstractParser(ctx_)
{
  fs::path searchDbPath =
    fs::path(ctx_.options["workspace"].as<std::string>()) /
    ctx_.options["name"].as<std::string>() /
    std::string("search");

  _searchDb = Xapian::WritableDatabase(
    searchDbPath.string(),
    Xapian::DB_CREATE_OR_OVERWRITE);
}

// TODO: In the old CodeCompass version there was a postParse() call which made
// suggestion database.
bool SearchParser::parse()
{
  for (const std::string& path :
    _ctx.options["input"].as<std::vector<std::string>>())
  {
    using namespace std::placeholders;

    LOG(info) << "Search parse path: " << path;

    util::iterateDirectoryRecursive(
      path,
      std::bind(&SearchParser::parseFile, this, _1));
  }

  return true;
}

bool SearchParser::parseFile(const std::string& file_)
{
  // TODO: Support --search-skip-directory. This function should return false
  // on these directories so their content is not iterated at all.
  // TODO: Do we need libmagic to determine file type? I don't think so,
  // because non-source files give no result with CTags. Maybe it's useful for
  // determining language.
  // TODO: Check read permission.

  if (!shouldHandle(file_))
    return true;

  CTags ctags;
  std::vector<CTags::Tag> tags = ctags.execute(file_);

  // TODO: In the old CodeCompass version the file was persisted in the end.
  // TODO: We should add a new parseStatus: "only in search index". Or is it
  // too specific? Do we need to indicate the fact that the file is search
  // indexed?
  model::FilePtr file = _ctx.srcMgr.getFile(file_);

  // File content is not available in case of non-plaintext and non-regular
  // files. Files without content don't need to be indexed.
  if (!file->content)
    return true;

  std::ifstream fileStream(file_);
  std::string fileContent(
    std::istreambuf_iterator<char>{fileStream},
    std::istreambuf_iterator<char>{});
  fileStream.close();

  Xapian::TermGenerator gen;
  Xapian::Document doc;

  doc.set_data(std::to_string(file->id));
  doc.add_boolean_term(file->path);

  gen.set_document(doc);
  gen.index_text(fileContent);

  // TODO: Do we need ":" separator?
  for (const CTags::Tag& tag : tags)
    gen.index_text(tag.name, 1, std::string(1, tag.kind));

  _searchDb.add_document(doc);

  return true;
}

bool SearchParser::shouldHandle(const std::string& file_) const
{
  // TODO: In the old CodeCompass version there were some skipped extensions:
  // .doc, .rtf, .html, etc.
  // TODO: In the old CodeCompass version too large files (i.e. >1M) were not
  // indexed. I think this restriction is not necessary.
  // TODO: Non-text files (like binaries) should be excluded.
  if (!fs::is_regular_file(file_))
    return false;
  if (fs::is_directory(file_))
    return false;

  return true;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern "C"
{
  boost::program_options::options_description getOptions()
  {
    boost::program_options::options_description description("Search Plugin");
    return description;
  }

  std::shared_ptr<SearchParser> make(ParserContext& ctx_)
  {
    return std::shared_ptr<SearchParser>(new SearchParser(ctx_));
  }
}
#pragma clang diagnostic pop

}
}
