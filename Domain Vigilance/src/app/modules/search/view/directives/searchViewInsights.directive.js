(function(){

    var app = angular.module('app.modules.search');

    app.directive("searchViewInsights", ['socketService', 'utilsService', '$state', searchViewInsights]);

    function searchViewInsights(socketService, utilsService){
        return {
            restrict:'AE',
            templateUrl: 'app/modules/search/view/templates/searchViewInsights.template.html',

            controller: app.classy.controller({
                inject: ['$scope', '$state'],

                data:{},

                init: function()
                {
                    var _this = this;

                    if(!_this.$scope.scanData){
                        _this.$scope.searchID = _this.$state.params.id;
                    } else {
                        _this.$scope.searchID = _this.$scope.scanData._id;
                    }

                    _this.$scope.insights = {};
                    _this.$scope.loadInsights(['whois.registrant_name', 'expiring', 'traffic']);

                },

                watch: {
                  'setArchiveLoading':'reloadInsights'
                },

                methods: {


                    reloadInsights: function(){
                        var _this = this;

                        var insights = [];

                        angular.forEach(_this.$scope.insights, function(value, key){
                              insights.push(key);
                            console.log(key);
                          });

                        _this.$scope.insights = {};
                        _this.$scope.loadInsights(insights, true);
                    },

                    loadInsights: function(insightTypes){
                        var _this = this;


                        angular.forEach(insightTypes, function(value, key){
                            if(_this.$scope.insights[value]){
                                insightTypes.splice(key, 1);
                            }
                        });


                        if(insightTypes.length <= 0){
                            return;
                        }

                        var params = {};
                        params.func = 'loadInsights';
                        params.searchID = _this.$scope.searchID;
                        params.insightTypes = insightTypes;

                        socketService.ws.evalurl('search/searchController.js', params, function(res) {
                            if(res.error){
                                utilsService.toast(res.error, 'error');
                            } else {
                                // Append any new insights to existing insights object, don't override
                                angular.forEach(res, function(value, key){
                                   _this.$scope.insights[key] = value;
                                });
                            }
                        });
                    },

                    searchInsightFilter: function(key,search){
                        var _this = this;

                        var filter = {
                            value: search,
                            field: key
                        }
                        _this.$scope.pagination.search.push(filter);
                        _this.$scope.pagination.page = 1;

                    }
                }

            })
        };
    }

})();