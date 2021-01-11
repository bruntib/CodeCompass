#ifndef CC_PARSER_SEARCHPARSER_H
#define CC_PARSER_SEARCHPARSER_H

#include <parser/abstractparser.h>
#include <parser/parsercontext.h>

namespace cc
{
namespace parser
{

class SearchParser : public AbstractParser
{
public:
  SearchParser(ParserContext& ctx_);

  virtual bool parse() override;

private:
  bool parseFile(const std::string& file_);
  bool shouldHandle(const std::string& file_) const;

  Xapian::WritableDatabase _searchDb;
};

}
}

#endif
