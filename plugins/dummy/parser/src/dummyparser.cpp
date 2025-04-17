#include <dummyparser/dummyparser.h>

#include <boost/filesystem.hpp>

#include <parser/sourcemanager.h>
#include <util/logutil.h>
#include <util/parserutil.h>

#include <memory>

namespace cc
{
namespace parser
{

DummyParser::DummyParser(ParserContext& ctx_)
  : AbstractParser(ctx_),
    _goSourceType("GO")
{
}

bool DummyParser::accept(const std::string& path_)
{
  std::string ext = boost::filesystem::extension(path_);
  return ext == ".go";
}

bool DummyParser::parseToJson(const std::string& path_)
{
  std::string output = path_ + ".json";

  std::string command = "exast";
  command += ' ' + path_;
  command += ' ' + output;

  std::system(command.c_str());

  return true;
}

bool DummyParser::parse()
{
  for (const std::string& path :
    _ctx.options["input"].as<std::vector<std::string>>())
  {
    LOG(info) << "DummyParser parse path: " << path;
    util::iterateDirectoryRecursive(path, [this](const std::string& path) {
      if (!accept(path))
        return true;

      model::FilePtr file = _ctx.srcMgr.getFile(path);
      file->parseStatus = model::File::PSFullyParsed;
      file->type = this->_goSourceType;
      _ctx.srcMgr.updateFile(*file);

      return parseToJson(path);
    });
  }
  return true;
}

DummyParser::~DummyParser()
{
}

/* These two methods are used by the plugin manager to allow dynamic loading
   of CodeCompass Parser plugins. Clang (>= version 6.0) gives a warning that
   these C-linkage specified methods return types that are not proper from a
   C code.

   These codes are NOT to be called from any C code. The C linkage is used to
   turn off the name mangling so that the dynamic loader can easily find the
   symbol table needed to set the plugin up.
*/
// When writing a plugin, please do NOT copy this notice to your code.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern "C"
{
  boost::program_options::options_description getOptions()
  {
    boost::program_options::options_description description("Dummy Plugin");

    description.add_options()
        ("dummy-arg", po::value<std::string>()->default_value("Dummy arg"),
          "This argument will be used by the dummy parser.");

    return description;
  }

  std::shared_ptr<DummyParser> make(ParserContext& ctx_)
  {
    return std::make_shared<DummyParser>(ctx_);
  }
}
#pragma clang diagnostic pop

} // parser
} // cc
