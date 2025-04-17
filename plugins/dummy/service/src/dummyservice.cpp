#include <service/dummyservice.h>
#include <util/dbutil.h>

namespace cc
{
namespace service
{
namespace language
{

DummyServiceHandler::DummyServiceHandler(
  std::shared_ptr<odb::database> db_,
  std::shared_ptr<std::string> /*datadir_*/,
  const cc::webserver::ServerContext& context_)
    : _db(db_), _transaction(db_), _config(context_.options)
{
}

void DummyServiceHandler::getDummyString(std::string& str_)
{
  str_ = _config["dummy-result"].as<std::string>();
}

void DummyServiceHandler::getFileTypes(std::vector<std::string>& return_)
{
  return_.push_back("GO");
}

void DummyServiceHandler::getAstNodeInfo(AstNodeInfo& _return, const core::AstNodeId& astNodeId)
{

}

void DummyServiceHandler::getAstNodeInfoByPosition(AstNodeInfo& _return, const core::FilePosition& fpos)
{

}

void DummyServiceHandler::getSourceText(std::string& _return, const core::AstNodeId& astNodeId)
{

}

void DummyServiceHandler::getDocumentation(std::string& _return, const core::AstNodeId& astNodeId)
{

}

void DummyServiceHandler::getProperties(std::map<std::string, std::string> & _return, const core::AstNodeId& astNodeIds)
{

}

void DummyServiceHandler::getDiagramTypes(std::map<std::string, int32_t> & _return, const core::AstNodeId& astNodeId)
{

}

void DummyServiceHandler::getDiagram(std::string& _return, const core::AstNodeId& astNodeId, const int32_t diagramId)
{

}

void DummyServiceHandler::getDiagramLegend(std::string& _return, const int32_t diagramId)
{

}

void DummyServiceHandler::getFileDiagramTypes(std::map<std::string, int32_t> & _return, const core::FileId& fileId)
{

}

void DummyServiceHandler::getFileDiagram(std::string& _return, const core::FileId& fileId, const int32_t diagramId)
{

}

void DummyServiceHandler::getFileDiagramLegend(std::string& _return, const int32_t diagramId)
{

}

void DummyServiceHandler::getReferenceTypes(std::map<std::string, int32_t> & _return, const core::AstNodeId& astNodeId)
{

}

int32_t DummyServiceHandler::getReferenceCount(const core::AstNodeId& astNodeId, const int32_t referenceId)
{

}

void DummyServiceHandler::getReferences(std::vector<AstNodeInfo> & _return, const core::AstNodeId& astNodeId, const int32_t referenceId, const std::vector<std::string> & tags)
{

}

void DummyServiceHandler::getReferencesInFile(std::vector<AstNodeInfo> & _return, const core::AstNodeId& astNodeId, const int32_t referenceId, const core::FileId& fileId, const std::vector<std::string> & tags)
{

}

void DummyServiceHandler::getReferencesPage(std::vector<AstNodeInfo> & _return, const core::AstNodeId& astNodeId, const int32_t referenceId, const int32_t pageSize, const int32_t pageNo)
{

}

void DummyServiceHandler::getFileReferenceTypes(std::map<std::string, int32_t> & _return, const core::FileId& fileId)
{

}

void DummyServiceHandler::getFileReferences(std::vector<AstNodeInfo> & _return, const core::FileId& fileId, const int32_t referenceId)
{

}

int32_t DummyServiceHandler::getFileReferenceCount(const core::FileId& fileId, const int32_t referenceId)
{

}

void DummyServiceHandler::getSyntaxHighlight(std::vector<SyntaxHighlight> & _return, const core::FileRange& range)
{

}

} // language
} // service
} // cc
