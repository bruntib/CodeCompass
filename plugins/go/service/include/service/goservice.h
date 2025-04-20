#ifndef CC_SERVICE_LANGUAGE_GOSERVICE_H
#define CC_SERVICE_LANGUAGE_GOSERVICE_H

#include <memory>
#include <vector>
#include <map>
#include <unordered_set>
#include <string>

#include <boost/program_options/variables_map.hpp>

#include <odb/database.hxx>

#include <LanguageService.h>

#include <model/goastnode.h>
#include <model/goastnode-odb.hxx>
#include <model/goentity.h>
#include <model/goentity-odb.hxx>
#include <model/gofunction.h>
#include <model/gofunction-odb.hxx>
#include <model/gomethod.h>
#include <model/gomethod-odb.hxx>
#include <model/gostruct.h>
#include <model/gostruct-odb.hxx>
#include <model/gointerface.h>
#include <model/gointerface-odb.hxx>
#include <model/goenum.h>
#include <model/goenum-odb.hxx>
#include <model/gotype.h>
#include <model/gotype-odb.hxx>
#include <model/govariable.h>
#include <model/govariable-odb.hxx>
#include <model/goconstant.h>
#include <model/goconstant-odb.hxx>
#include <model/gostatement.h>
#include <model/gostatement-odb.hxx>
#include <model/gopackage.h>
#include <model/gopackage-odb.hxx>
#include <model/goimport.h>
#include <model/goimport-odb.hxx>

#include <util/odbtransaction.h>
#include <util/graph.h>
#include <webserver/servercontext.h>

namespace cc
{
namespace service
{
namespace language
{

class GoServiceHandler : virtual public LanguageServiceIf
{
public:
  GoServiceHandler(
    std::shared_ptr<odb::database> db_,
    std::shared_ptr<std::string> datadir_,
    const cc::webserver::ServerContext& context_);

  void getGoString(std::string& str_);

  void getFileTypes(std::vector<std::string> & _return);

  void getAstNodeInfo(AstNodeInfo& _return, const core::AstNodeId& astNodeId);

  void getAstNodeInfoByPosition(AstNodeInfo& _return, const core::FilePosition& fpos);

  void getSourceText(std::string& _return, const core::AstNodeId& astNodeId);

  void getDocumentation(std::string& _return, const core::AstNodeId& astNodeId);

  void getProperties(std::map<std::string, std::string> & _return, const core::AstNodeId& astNodeIds);

  void getDiagramTypes(std::map<std::string, int32_t> & _return, const core::AstNodeId& astNodeId);

  void getDiagram(std::string& _return, const core::AstNodeId& astNodeId, const int32_t diagramId);

  void getDiagramLegend(std::string& _return, const int32_t diagramId);

  void getFileDiagramTypes(std::map<std::string, int32_t> & _return, const core::FileId& fileId);

  void getFileDiagram(std::string& _return, const core::FileId& fileId, const int32_t diagramId);

  void getFileDiagramLegend(std::string& _return, const int32_t diagramId);

  void getReferenceTypes(std::map<std::string, int32_t> & _return, const core::AstNodeId& astNodeId);

  int32_t getReferenceCount(const core::AstNodeId& astNodeId, const int32_t referenceId);

  void getReferences(std::vector<AstNodeInfo> & _return, const core::AstNodeId& astNodeId, const int32_t referenceId, const std::vector<std::string> & tags);

  void getReferencesInFile(std::vector<AstNodeInfo> & _return, const core::AstNodeId& astNodeId, const int32_t referenceId, const core::FileId& fileId, const std::vector<std::string> & tags);

  void getReferencesPage(std::vector<AstNodeInfo> & _return, const core::AstNodeId& astNodeId, const int32_t referenceId, const int32_t pageSize, const int32_t pageNo);

  void getFileReferenceTypes(std::map<std::string, int32_t> & _return, const core::FileId& fileId);

  void getFileReferences(std::vector<AstNodeInfo> & _return, const core::FileId& fileId, const int32_t referenceId);

  int32_t getFileReferenceCount(const core::FileId& fileId, const int32_t referenceId);

  void getSyntaxHighlight(std::vector<SyntaxHighlight> & _return, const core::FileRange& range);

private:
  std::shared_ptr<odb::database> _db;
  std::shared_ptr<std::string> _datadir;
  util::OdbTransaction _transaction;
  const cc::webserver::ServerContext& _context;
};

} // language
} // service
} // cc

#endif // CC_SERVICE_LANGUAGE_GOSSERVICE_H