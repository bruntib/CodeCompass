#ifndef CC_PARSER_CTAGSRESULTPARSER_H
#define CC_PARSER_CTAGSRESULTPARSER_H

#include <boost/process.hpp>

namespace cc
{
namespace parser
{

class CTags
{
public:
  /**
   * In CodeCompass we don't require users to be aware of all possible tag
   * kinds. So we map kind names to some generic kind types. So it doesn't
   * matter if a tag is method or subroutine when the user is looking for
   * functions.
   *
   * t - Type
   * m - Macro
   * c - Constant
   * f - Function
   * v - Variable
   * l - Label
   * o - Module
   * x - Other
   */
  typedef char Kind;

  struct Tag
  {
    std::string name;
    int line;
    int column;
    Kind kind;
  };

  CTags();
  ~CTags();

  /**
   * This function checks whether ctags binary is correctly installed on the
   * system. It tries to execute "ctags --version" command and returns true if
   * it was successful.
   */
  bool check() const;

  /**
   * Execute CTags on a source file. This function throws and exception if
   * ctags binary is not available. You should call check() function before
   * this one.
   *
   * @param filepath_ A source file path to parse with CTags.
   */
  std::vector<Tag> execute(const std::string& filepath_);

private:
  /**
   * This function parses the description_ of a tag. The description_ is one
   * line of the output of "ctags --fields=-f+Kzn ..." command.
   *
   * @param description_ The format of this description is:
   * <tag_name>\t<file_path>\t<pattern>\t<field>[<field>...]
   */
  Tag parseTag(const std::string& description_) const;

  /**
   * Map tag name to some generic tag kind. See the documentation of Kind for
   * further information.
   */
  Kind tagNameToKind(const std::string& tagName_) const;

  static const std::string _ctagsCmd;

  boost::process::ipstream _output;
  boost::process::opstream _input;

  boost::process::child _proc;
};

}
}

#endif