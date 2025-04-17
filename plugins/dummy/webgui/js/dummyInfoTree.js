require([
  'codecompass/model',
  'codecompass/viewHandler',
  'codecompass/util'],
function (model, viewHandler, util) {
  model.addService('dummyservice', 'DummyService', LanguageServiceClient);

  // function createRootNode(elementInfo) {
  //   return {
  //     id: 'root',
  //     name: 'blabla'
  //   };
  // }

  // var dummyInfoTree = {
  //   render: function (elementInfo) {
  //     console.log("xxxxxx", elementInfo);
  //     var ret = [];

  //     ret.push({
  //       id: 'blabla',
  //       name: 'valami',
  //       hasChildren: false
  //     });

  //     return ret;
  //   }
  // };

  // viewHandler.registerModule(dummyInfoTree, {
  //   type: viewHandler.moduleType.InfoTree,
  //   service: model.dummyservice
  // });
});