#include <algorithm>
#include <queue>
#include <regex>

#include <util/util.h>
#include <util/logutil.h>

#include <service/goservice.h>
#include "diagram.h"

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

void GoServiceHandler::getAstNodeInfo(
  AstNodeInfo& return_,
  const core::AstNodeId& astNodeId_)
{
  return_ = _transaction([this, &astNodeId_]() {
    return CreateAstNodeInfo()(queryGoAstNode(astNodeId_));
  });
}

void GoServiceHandler::getAstNodeInfoByPosition(
  AstNodeInfo& return_,
  const core::FilePosition& fpos_)
{
  _transaction([&, this](){
    //--- Query nodes at the given position ---//

    AstResult nodes = _db->query<model::GoAstNode>(
      AstQuery::location.file == std::stoull(fpos_.file) &&
      // StartPos <= Pos
      ((AstQuery::location.range.start.line == fpos_.pos.line &&
        AstQuery::location.range.start.column <= fpos_.pos.column) ||
       AstQuery::location.range.start.line < fpos_.pos.line) &&
      // Pos < EndPos
      ((AstQuery::location.range.end.line == fpos_.pos.line &&
        AstQuery::location.range.end.column > fpos_.pos.column) ||
       AstQuery::location.range.end.line > fpos_.pos.line));

    //--- Select innermost clickable node ---//

    model::Range minRange(model::Position(0, 0), model::Position());
    model::GoAstNode min;

    for (const model::GoAstNode& node : nodes)
    {
      if (node.location.range < minRange)
      {
        min = node;
        minRange = node.location.range;
      }
    }

    return_ = _transaction([this, &min](){
      return CreateAstNodeInfo(getTags({min}))(min);
    });
  });
}

void GoServiceHandler::getSourceText(
  std::string& return_,
  const core::AstNodeId& astNodeId_)
{
  return_ = _transaction([this, &astNodeId_](){
    model::GoAstNode astNode = queryGoAstNode(astNodeId_);

    if (astNode.location.file)
      return cc::util::textRange(
        astNode.location.file.load()->content.load()->content,
        astNode.location.range.start.line,
        astNode.location.range.start.column,
        astNode.location.range.end.line,
        astNode.location.range.end.column);

    return std::string();
  });
}

void GoServiceHandler::getDocumentation(std::string& _return, const core::AstNodeId& astNodeId)
{

}

void GoServiceHandler::getProperties(std::map<std::string, std::string> & _return, const core::AstNodeId& astNodeIds)
{

}

void GoServiceHandler::getDiagramTypes(
  std::map<std::string, int32_t>& return_,
  const core::AstNodeId& astNodeId_)
{
  std::vector<AstNodeInfo> definitions;
  getReferences(definitions, astNodeId_, DEFINITION, {});

  core::AstNodeId astNodeId = astNodeId_;
  if (!definitions.empty())
    astNodeId = definitions.front().id;

  model::GoAstNode node = queryGoAstNode(astNodeId);

  switch (node.symbolType)
  {
    case model::GoAstNode::SymbolType::Function:
      return_["Function call diagram"] = FUNCTION_CALL;
      break;
  }
}

void GoServiceHandler::getDiagram(
  std::string& return_,
  const core::AstNodeId& astNodeId_,
  const int32_t diagramId_)
{
  util::Graph graph = returnDiagram(astNodeId_, diagramId_);

  if (graph.nodeCount() != 0)
    return_ = graph.output(util::Graph::SVG);
}

util::Graph GoServiceHandler::returnDiagram(
  const core::AstNodeId& astNodeId_,
  const std::int32_t diagramId_)
{
  GoDiagram diagram(_db, _datadir, _context);
  util::Graph graph;

  switch (diagramId_)
  {
    case FUNCTION_CALL:
      diagram.getFunctionCallDiagram(graph, astNodeId_);
      break;
  }

  return graph;
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

void GoServiceHandler::getReferenceTypes(
  std::map<std::string, int32_t>& return_,
  const core::AstNodeId& astNodeId_)
{
  model::GoAstNode node = queryGoAstNode(astNodeId_);

  return_["Definition"] = DEFINITION;
  return_["Usage"] = USAGE;

  switch (node.symbolType)
  {
    case model::GoAstNode::SymbolType::Function:
      return_["This calls"] = THIS_CALLS;
      return_["Callee"] = CALLEE;
      return_["Caller"] = CALLER;
      break;
  }
}

int32_t GoServiceHandler::getReferenceCount(
  const core::AstNodeId& astNodeId_,
  const int32_t referenceId_)
{
  model::GoAstNode node = queryGoAstNode(astNodeId_);

  return _transaction([&, this]() -> std::int32_t {
    switch (referenceId_)
    {
      case DEFINITION:
        return queryGoAstNodeCount(astNodeId_,
          AstQuery::symbolType == model::GoAstNode::SymbolType::Function ||
          AstQuery::symbolType == model::GoAstNode::SymbolType::Package ||
          AstQuery::symbolType == model::GoAstNode::SymbolType::Type ||
          AstQuery::symbolType == model::GoAstNode::SymbolType::Variable);

      case USAGE:
        return queryGoAstNodeCount(astNodeId_);

        case THIS_CALLS:
        return queryCallsCount(astNodeId_);

      case CALLS_OF_THIS:
        return queryGoAstNodeCount(astNodeId_,
          AstQuery::symbolType == model::GoAstNode::SymbolType::Reference);

      case CALLEE:
      {
        std::int32_t count = 0;

        std::set<std::uint64_t> defHashes;
        for (const model::GoAstNode& call : queryCalls(astNodeId_))
        {
          model::GoAstNode node = queryGoAstNode(std::to_string(call.id));
          defHashes.insert(node.entityHash);
        }

        if (!defHashes.empty())
          count += _db->query_value<model::GoAstCount>(
            AstQuery::entityHash.in_range(
              defHashes.begin(), defHashes.end()) &&
            AstQuery::location.range.end.line != model::Position::npos).count;

        return count;
      }

      case CALLER:
      {
        std::vector<AstNodeInfo> references;
        getReferences(references, astNodeId_, CALLER, {});
        return references.size();
      }
    }
  });
}

void GoServiceHandler::getReferences(
  std::vector<AstNodeInfo>& return_,
  const core::AstNodeId& astNodeId_,
  const int32_t referenceId_,
  const std::vector<std::string>& tags_)
{
  std::vector<model::GoAstNode> nodes;

  _transaction([&, this](){
    switch (referenceId_)
    {
      case DEFINITION:
        nodes = queryDefinitions(astNodeId_);
        break;

      case USAGE:
        nodes = queryGoAstNodes(astNodeId_);
        break;

      case THIS_CALLS:
        nodes = queryCalls(astNodeId_);
        break;

      case CALLS_OF_THIS:
        nodes = queryGoAstNodes(
          astNodeId_,
          AstQuery::symbolType == model::GoAstNode::SymbolType::Reference);
        break;

      case CALLEE:
        for (const model::GoAstNode& call : queryCalls(astNodeId_))
        {
          core::AstNodeId astNodeId = std::to_string(call.id);
          std::vector<model::GoAstNode> defs = queryDefinitions(astNodeId);
          nodes.insert(nodes.end(), defs.begin(), defs.end());
        }

        std::sort(nodes.begin(), nodes.end());
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());

        break;

      case CALLER:
        for (const model::GoAstNode& astNode : queryGoAstNodes(
          astNodeId_,
          AstQuery::symbolType == model::GoAstNode::SymbolType::Reference))
        {
          const model::Position& start = astNode.location.range.start;
          const model::Position& end   = astNode.location.range.end;

          AstResult result = _db->query<model::GoAstNode>(
            AstQuery::symbolType == model::GoAstNode::SymbolType::Function &&
            // Same file
            AstQuery::location.file == astNode.location.file.object_id() &&
            // StartPos >= Pos
            ((AstQuery::location.range.start.line == start.line &&
              AstQuery::location.range.start.column <= start.column) ||
             AstQuery::location.range.start.line < start.line) &&
            // Pos > EndPos
            ((AstQuery::location.range.end.line == end.line &&
              AstQuery::location.range.end.column > end.column) ||
             AstQuery::location.range.end.line > end.line));

          nodes.insert(nodes.end(), result.begin(), result.end());
        }

        std::sort(nodes.begin(), nodes.end());
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());

        break;
    }

    return_.reserve(nodes.size());
    _transaction([this, &return_, &nodes](){
      std::transform(
        nodes.begin(), nodes.end(),
        std::back_inserter(return_),
        CreateAstNodeInfo(getTags(nodes)));
    });
  });
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

std::vector<model::GoAstNode> GoServiceHandler::queryDefinitions(
  const core::AstNodeId& astNodeId_)
{
  return queryGoAstNodes(
    astNodeId_,
    AstQuery::symbolType == model::GoAstNode::SymbolType::Function ||
    AstQuery::symbolType == model::GoAstNode::SymbolType::Package ||
    AstQuery::symbolType == model::GoAstNode::SymbolType::Type ||
    AstQuery::symbolType == model::GoAstNode::SymbolType::Variable);
}

model::GoAstNode GoServiceHandler::queryGoAstNode(
  const core::AstNodeId& astNodeId_)
{
  return _transaction([&, this]() {
    model::GoAstNode node;

    if (!_db->find(std::stoull(astNodeId_), node))
    {
      core::InvalidId ex;
      ex.__set_msg("Invalid GoAstNode ID");
      ex.__set_nodeid(astNodeId_);
      throw ex;
    }

    return node;
  });
}

std::vector<model::GoAstNode> GoServiceHandler::queryGoAstNodes(
  const core::AstNodeId& astNodeId_,
  const AstQuery& query_)
{
  model::GoAstNode node = queryGoAstNode(astNodeId_);

  AstResult result = _db->query<model::GoAstNode>(
    AstQuery::entityHash == node.entityHash &&
    AstQuery::location.range.end.line != model::Position::npos &&
    query_);

  return std::vector<model::GoAstNode>(result.begin(), result.end());
}

std::size_t GoServiceHandler::queryGoAstNodeCount(
  const core::AstNodeId& astNodeId_,
  const AstQuery& query_)
{
  model::GoAstNode node = queryGoAstNode(astNodeId_);

  model::GoAstCount q = _db->query_value<model::GoAstCount>(
    AstQuery::entityHash == node.entityHash &&
    AstQuery::location.range.end.line != model::Position::npos &&
    query_);

  return q.count;
}

std::vector<model::GoAstNode> GoServiceHandler::queryCalls(
  const core::AstNodeId& astNodeId_)
{
  std::vector<model::GoAstNode> nodes = queryDefinitions(astNodeId_);

  if (nodes.empty())
    return nodes;

  model::GoAstNode node = nodes.front();
  AstResult result = _db->query<model::GoAstNode>(astCallsQuery(node));

  nodes = std::vector<model::GoAstNode>(result.begin(), result.end());

  return nodes;
}

std::size_t GoServiceHandler::queryCallsCount(
  const core::AstNodeId& astNodeId_)
{
  std::vector<model::GoAstNode> nodes = queryDefinitions(astNodeId_);

  if (nodes.empty())
    return std::size_t(0);

  model::GoAstNode node = nodes.front();

  return _db->query_value<model::GoAstCount>(astCallsQuery(node)).count;
}

std::map<model::GoAstNodeId, std::vector<std::string>>
GoServiceHandler::getTags(const std::vector<model::GoAstNode>& nodes_)
{
  std::map<model::GoAstNodeId, std::vector<std::string>> tags;

  for (const model::GoAstNode& node : nodes_)
  {
    tags[node.id];
  }

  return tags;
}

odb::query<model::GoAstNode> GoServiceHandler::astCallsQuery(
  const model::GoAstNode& astNode_)
{
  const model::Position& start = astNode_.location.range.start;
  const model::Position& end = astNode_.location.range.end;

  return (AstQuery::location.file == astNode_.location.file.object_id() &&
    AstQuery::symbolType == model::GoAstNode::SymbolType::Reference &&
    AstQuery::defId + "LIKE" + AstQuery::_val("\%Function\%") &&
    // StartPos >= Pos
    ((AstQuery::location.range.start.line == start.line &&
      AstQuery::location.range.start.column >= start.column) ||
     AstQuery::location.range.start.line > start.line) &&
    // Pos > EndPos
    ((AstQuery::location.range.end.line == end.line &&
      AstQuery::location.range.end.column < end.column) ||
     AstQuery::location.range.end.line < end.line));
}

} // language
} // service
} // cc