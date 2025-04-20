#include <algorithm>
#include <queue>
#include <regex>

#include <util/util.h>
#include <util/logutil.h>

#include <service/goservice.h>

namespace cc
{
namespace service
{
namespace language
{

typedef odb::query<model::GoAstNode> AstQuery;
typedef odb::result<model::GoAstNode> AstResult;
typedef odb::query<model::GoFunction> FuncQuery;
typedef odb::result<model::GoFunction> FuncResult;
typedef odb::query<model::GoMethod> MethodQuery;
typedef odb::result<model::GoMethod> MethodResult;
typedef odb::query<model::GoStruct> StructQuery;
typedef odb::result<model::GoStruct> StructResult;
typedef odb::query<model::GoStructField> FieldQuery;
typedef odb::result<model::GoStructField> FieldResult;
typedef odb::query<model::GoInterface> InterfaceQuery;
typedef odb::result<model::GoInterface> InterfaceResult;
typedef odb::query<model::GoEnum> EnumQuery;
typedef odb::result<model::GoEnum> EnumResult;
typedef odb::query<model::GoVariable> VarQuery;
typedef odb::result<model::GoVariable> VarResult;
typedef odb::query<model::GoConstant> ConstQuery;
typedef odb::result<model::GoConstant> ConstResult;
typedef odb::query<model::GoType> TypeQuery;
typedef odb::result<model::GoType> TypeResult;
typedef odb::query<model::GoStatement> StmtQuery;
typedef odb::result<model::GoStatement> StmtResult;
typedef odb::query<model::File> FileQuery;
typedef odb::result<model::File> FileResult;
typedef odb::query<model::GoPackage> PackageQuery;
typedef odb::result<model::GoPackage> PackageResult;
typedef odb::query<model::GoImport> ImportQuery;
typedef odb::result<model::GoImport> ImportResult;

 //transforms a model::GoAstNode to an AstNodeInfo Thrift object

struct CreateAstNodeInfo
{
  typedef std::map<model::GoAstNodeId, std::vector<std::string>> TagMap;

  CreateAstNodeInfo(const TagMap& tags_ = {}) : _tags(tags_)
  {
  }

  // Returns the Thrift object for this Go AST node.

  cc::service::language::AstNodeInfo operator()(
    const model::GoAstNode& astNode_)
  {
    cc::service::language::AstNodeInfo ret;

    ret.__set_id(std::to_string(astNode_.id));
    ret.__set_entityHash(astNode_.entityHash);
    ret.__set_astNodeType(astNode_.nodeType);
    ret.__set_symbolType(model::symbolTypeToString(astNode_.symbolType));
    ret.__set_astNodeValue(astNode_.name);

    ret.range.range.startpos.line = astNode_.location.range.start.line;
    ret.range.range.startpos.column = astNode_.location.range.start.column;
    ret.range.range.endpos.line = astNode_.location.range.end.line;
    ret.range.range.endpos.column = astNode_.location.range.end.column;

    if (astNode_.location.file)
      ret.range.file = std::to_string(astNode_.location.file.object_id());

    TagMap::const_iterator it = _tags.find(astNode_.id);
    if (it != _tags.end())
      ret.__set_tags(it->second);

    return ret;
  }

  const std::map<model::GoAstNodeId, std::vector<std::string>>& _tags;
};

GoServiceHandler::GoServiceHandler(
  std::shared_ptr<odb::database> db_,
  std::shared_ptr<std::string> datadir_,
  const cc::webserver::ServerContext& context_)
    : _db(db_),
      _transaction(db_),
      _datadir(datadir_),
      _context(context_)
{
}
void GoServiceHandler::getGoString(std::string& str_)
{
  str_ = _context.options["go-result"].as<std::string>();
}


void GoServiceHandler::getFileTypes(std::vector<std::string>& return_)
{
  return_.push_back("GO");
  return_.push_back("Dir");
}

void GoServiceHandler::getAstNodeInfo(AstNodeInfo& _return, const core::AstNodeId& astNodeId)
{

}

void GoServiceHandler::getAstNodeInfoByPosition(AstNodeInfo& _return, const core::FilePosition& fpos)
{

}

void GoServiceHandler::getSourceText(std::string& _return, const core::AstNodeId& astNodeId)
{

}

void GoServiceHandler::getDocumentation(std::string& _return, const core::AstNodeId& astNodeId)
{

}

void GoServiceHandler::getProperties(std::map<std::string, std::string> & _return, const core::AstNodeId& astNodeIds)
{

}

void GoServiceHandler::getDiagramTypes(std::map<std::string, int32_t> & _return, const core::AstNodeId& astNodeId)
{

}

void GoServiceHandler::getDiagram(std::string& _return, const core::AstNodeId& astNodeId, const int32_t diagramId)
{

}

void GoServiceHandler::getDiagramLegend(std::string& _return, const int32_t diagramId)
{

}

void GoServiceHandler::getFileDiagramTypes(std::map<std::string, int32_t> & _return, const core::FileId& fileId)
{

}

void GoServiceHandler::getFileDiagram(std::string& _return, const core::FileId& fileId, const int32_t diagramId)
{

}

void GoServiceHandler::getFileDiagramLegend(std::string& _return, const int32_t diagramId)
{

}

void GoServiceHandler::getReferenceTypes(std::map<std::string, int32_t> & _return, const core::AstNodeId& astNodeId)
{

}

int32_t GoServiceHandler::getReferenceCount(const core::AstNodeId& astNodeId, const int32_t referenceId)
{

}

void GoServiceHandler::getReferences(std::vector<AstNodeInfo> & _return, const core::AstNodeId& astNodeId, const int32_t referenceId, const std::vector<std::string> & tags)
{

}

void GoServiceHandler::getReferencesInFile(std::vector<AstNodeInfo> & _return, const core::AstNodeId& astNodeId, const int32_t referenceId, const core::FileId& fileId, const std::vector<std::string> & tags)
{

}

void GoServiceHandler::getReferencesPage(std::vector<AstNodeInfo> & _return, const core::AstNodeId& astNodeId, const int32_t referenceId, const int32_t pageSize, const int32_t pageNo)
{

}

void GoServiceHandler::getFileReferenceTypes(std::map<std::string, int32_t> & _return, const core::FileId& fileId)
{

}

void GoServiceHandler::getFileReferences(std::vector<AstNodeInfo> & _return, const core::FileId& fileId, const int32_t referenceId)
{

}

int32_t GoServiceHandler::getFileReferenceCount(const core::FileId& fileId, const int32_t referenceId)
{

}

void GoServiceHandler::getSyntaxHighlight(std::vector<SyntaxHighlight> & _return, const core::FileRange& range)
{

}

} // language
} // service
} // cc