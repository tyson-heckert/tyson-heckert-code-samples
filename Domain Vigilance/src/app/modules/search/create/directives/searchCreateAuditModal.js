(function(){

    var app = angular.module('app.modules.search');

    app.directive("searchCreateAuditModal", ['$state','utilsService','socketService', '$mdDialog','$timeout',searchCreateAuditModal]);


    /**
     *
     * Create a new System Generated Scan
     * This is for finding Typos Partials Exact Match etc.
     */

    function searchCreateAuditModal($state,utilsService,socketService, $mdDialog,$timeout){
        return {
            restrict: 'AE',
            templateUrl:'app/modules/search/create/templates/searchCreateAuditModal.tmpl.html',

            scope: {
                assocData:'='
            },
            link:function(scope, el, attrs,ctrl){

                scope.search = {};
                scope.domainCount = 0;

                scope.priceDetail={};
                scope.hasPrice=false;
                scope.getPrice=function(){
                    socketService.ws.evalurl('settings/usersController.js',{priceModel:'domain_audit',func:'getPrice'},function(res){
                        if (res.error) return utilsService.toast(res.error,'error');
                        scope.priceDetail=res;
                        scope.hasPrice=true;
                        console.log(res)
                        scope.$apply();
                    })
                };
                //scope.getPrice();
                console.log(scope.assocData);

                if(scope.assocData.list.length)
                    scope.domainCount += scope.assocData.list.length;

                if(scope.assocData.domains.length){

                    console.log("original arr length");
                    console.log(scope.assocData.domains[0].length);

                    var newArr = [],
                        origArr = scope.assocData.domains[0],
                        origLen = scope.assocData.domains[0].length,
                        found, x, y;

                    for (x = 0; x < origLen; x++) {
                        found = undefined;
                        for (y = 0; y < newArr.length; y++) {
                            if (origArr[x] === newArr[y]) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            newArr.push(origArr[x]);
                        }
                    }

                    scope.assocData.domains[0] = newArr;
                    console.log("updated arr length");
                    console.log(scope.assocData.domains[0].length);

                    scope.domainCount += scope.assocData.domains[0].length;
                }


                //scope.domainTotalCost = scope.domainCount * scope.priceDetail.price;
                scope.domainTotalCost = scope.domainCount * 0.25;


                console.log(scope.assocData.list.length);
                console.log(scope.assocData.domains.length);

                scope.$watch(scope.assocData, function(){
                    scope.search.uploadedList = scope.assocData;
                });
               // scope.$watch('hasPrice',function(){
               //     if (scope.hasPrice) {
               //         scope.domainTotalCost = scope.domainCount * scope.priceDetail.price;
               //     }
               // })

                scope.currentStep = 1;

                scope.result;

                scope.submit = function(){

                    //if(scope.search.referenceCode == "" || !scope.search.referenceCode){
                    //    scope.refCode = "NA";
                    //} else {
                    //    scope.refCode = scope.search.referenceCode;
                    //}

                    //scope.submitLoader = true;

                    var params=scope.search;
                    params.func='saveSearch';

                    socketService.ws.evalurl('search/create_search.js', params, function(res) {
                        if(res.error){
                            scope.errorMessage = res.error;
                        }
                        else{
                            scope.result = res;
                            scope.switchModal();
                            //scope.close();
                            //$state.go('triangular.admin-default.search-view', {id: res});
                        }

                    });

                };

                scope.switchModal = function(){

                    scope.currentStep = 2;
                    scope.assocData.currentStep = 2;
                    console.log("getting here with " + scope.currentStep);
                    scope.$apply()
                };



                scope.close = function(){
                    //scope.close();
                    $state.go('triangular.admin-default.search-view', {id: scope.result});

                    var state = {state: 'done'};
                    $mdDialog.hide(state);
                };

            }
        }
    }


})();