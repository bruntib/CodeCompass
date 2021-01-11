#include <cstring>
#include <functional>
#include <vector>

#include <boost/algorithm/string.hpp>

#include <util/logutil.h>

#include "ctagsResultParser.h"

namespace cc
{
namespace parser
{

// TODO: What about other languages?
// TODO: Do we need --languages?
const std::string CTags::_ctagsCmd =
  "ctags "
  "--filter "
  "--fields=-f+Kzn "
  "--C-kinds=+cdefglmnpstuvx "
  "--C++-kinds=+cdefglmnpstuvx "
  "--Java-kinds=+l "
  "--Perl-kinds=+d "
  "--SQL-kinds=+dlr ";

CTags::CTags() : _proc(
  _ctagsCmd,
  boost::process::std_out > _output,
  boost::process::std_in < _input)
{
}

CTags::~CTags()
{
  _proc.wait();
}

// TODO: This function is not needed because the constructor throws an error.
bool CTags::check() const
{
  try
  {
    boost::process::child proc("ctags --version");
    proc.wait();
  }
  catch (const boost::process::process_error& pe)
  {
    LOG(warning) << "CTags is not installed!";
    return false;
  }

  return true;
}

std::vector<CTags::Tag> CTags::execute(const std::string& filepath_)
{
  std::vector<CTags::Tag> tags;

  _input << filepath_ << '\n';
  // TODO: Is it good that we close the input stream?
  _input.close();
  _input.pipe().close();

  std::string line;
  while (std::getline(_output, line))
    tags.push_back(parseTag(line));

  return tags;
}

CTags::Tag CTags::parseTag(const std::string& description) const
{
  // TODO: Workaround for tags starting with "operator " in C++.

  Tag tag;

  const char* from = description.c_str();
  const char* to = std::strchr(from, '\t');
  tag.name = std::string(from, to);

  from = to + 1; to = std::strchr(from, '\t'); // File name.
  from = to + 1; to = std::strchr(from, '\t'); // Line pattern.

  for (from = to + 1; (to = std::strchr(from, '\t')); from = to + 1)
    if (boost::starts_with(from, "kind:"))
      tag.kind = tagNameToKind(from + std::strlen("kind:"));
    else if (boost::starts_with(from, "line:"))
      tag.line = std::atoi(from + std::strlen("line:"));

  return tag;

/*
  Tag tag;

  std::vector<std::string> parts;
  boost::split(
    parts,
    boost::trim_copy(description),
    [](char c) { return c == '\t'; },
    boost::token_compress_on);

  tag.name = parts[0];

  // CTags output contains the source code line of the given tag. Format of
  // pattern is a regex literal:
  // /^int main()$/;
  // The position of "main" tag is found by std::string::find(), but we need to
  // subtract 2 because of /^ and add 1 because we're not indexing from 0.
  tag.column = parts[2].find(tag.name) - 1;

  // Format of "description" is:
  // <tag_name>\t<file_path>\t<pattern>\t<field>[<field>...]
  // Fields start at position 3 in the vector.
  //
  // std::strlen() is used for avoiding magic numbers. This is optimized out by
  // the compiler, so there is no actual computation behind it.
  for (std::size_t i = 3; i < parts.size(); ++i)
    if (boost::starts_with(parts[i], "kind:"))
      tag.kind = tagNameToKind(parts[i].substr(std::strlen("kind:")));
    else if (boost::starts_with(parts[i], "line:"))
      tag.line = std::stoi(parts[i].substr(std::strlen("line:")));

  return tag;
*/
}

CTags::Kind CTags::tagNameToKind(const std::string& tagName_) const
{
  typedef std::vector<std::string> StrVec;

  // TODO: name, tparam, header, annotation, unknown,
  // and others from ctags --list-kinds-full
  static const StrVec modules{
    "alias", "component", "module", "namespace", "package", "program",
    "using"};
  static const StrVec macros {
    "macro", "macroparam"};
  static const StrVec labels {
   "label"};
  static const StrVec types {
   "class", "enum", "Exception", "interface", "record", "struct", "table",
   "typedef", "union"};
  static const StrVec constants {
   "enumerator", "null"};
  static const StrVec functions {
   "function", "method", "prototype", "procedure", "subroutine"};
  static const StrVec variables {
    "externvar", "local", "member", "parameter", "variable"};

  auto contains = [](const StrVec& v, const std::string& s)
  {
    return std::find(v.begin(), v.end(), s) != v.end();
  };

  if (contains(modules, tagName_))
    return 'o';
  else if (contains(macros, tagName_))
    return 'm';
  else if (contains(labels, tagName_))
    return 'l';
  else if (contains(types, tagName_))
    return 't';
  else if (contains(constants, tagName_))
    return 'c';
  else if (contains(functions, tagName_))
    return 'f';
  else if (contains(variables, tagName_))
    return 'v';

  return 'x';
}

}
}