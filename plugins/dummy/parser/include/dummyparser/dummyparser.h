#ifndef CC_PARSER_DUMMYPARSER_H
#define CC_PARSER_DUMMYPARSER_H

#include <parser/abstractparser.h>
#include <parser/parsercontext.h>

namespace cc
{
namespace parser
{

class DummyParser : public AbstractParser
{
public:
  DummyParser(ParserContext& ctx_);
  virtual ~DummyParser();
  virtual bool parse() override;

private:
  static bool accept(const std::string& path_);
  static bool parseToJson(const std::string& path_);

  const std::string _goSourceType;
};

} // parser
} // cc

#endif // CC_PLUGINS_PARSER_DUMMYPARSER_H
