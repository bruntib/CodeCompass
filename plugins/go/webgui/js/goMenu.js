require([
  'dojo/topic',
  'dijit/Menu',
  'dijit/MenuItem',
  'dijit/PopupMenuItem',
  'codecompass/astHelper',
  'codecompass/model',
  'codecompass/urlHandler',
  'codecompass/viewHandler'],
function (topic, Menu, MenuItem, PopupMenuItem, astHelper, model, urlHandler, viewHandler) {
  model.addService('goservice', 'GoService', LanguageServiceClient);

  var getdefintion = {
    id: 'go-text-definition',
    render: function (nodeInfo, fileInfo) {
      return new MenuItem({
        label: 'Jump to definition',
        accelKey: 'ctrl - click',
        onClick: function () {
          if (!nodeInfo || !fileInfo)
            return;

          astHelper.jumpToDef(nodeInfo.id, model.goservice);

          if (window.gtag) {
            window.gtag ('event', 'jump_to_def', {
              'event_category' : urlHandler.getState('wsid'),
              'event_label' : urlHandler.getFileInfo().name
                  + ': '
                  + nodeInfo.astNodeValue
            });
          }
        }
      });
    }
  };

  viewHandler.registerModule(getdefintion, {
    type : viewHandler.moduleType.TextContextMenu,
    service : model.goservice
  });

  var infoTree = {
    id : 'go-text-infotree',
    render : function (nodeInfo, fileInfo) {
      return new MenuItem({
        label : 'Info Tree',
        onClick : function () {
          if (!nodeInfo || !fileInfo)
            return;

          topic.publish('codecompass/infotree', {
            fileType : fileInfo.type,
            elementInfo : nodeInfo
          });

          if (window.gtag) {
              window.gtag ('event', 'info_tree', {
              'event_category' : urlHandler.getState('wsid'),
              'event_label' : urlHandler.getFileInfo().name
                  + ': '
                  + nodeInfo.astNodeValue
            });
          }
        }
      });
    }
  };

  viewHandler.registerModule(infoTree, {
    type : viewHandler.moduleType.TextContextMenu,
    service : model.goservice
  });
});