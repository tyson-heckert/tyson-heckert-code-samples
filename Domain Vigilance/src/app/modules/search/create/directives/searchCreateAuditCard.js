(function(){

    var app = angular.module('app.modules.search');

    app.directive("searchCreateAuditCard", ['$state','utilsService','socketService', '$mdDialog','$rootScope', searchCreateAuditCard]);


    /**
     *
     * Create a new System Generated Scan
     * This is for finding Typos Partials Exact Match etc.
     */

    function searchCreateAuditCard($state,utilsService,socketService, $mdDialog, $rootScope){
        return {
            restrict: 'AE',
            templateUrl:'app/modules/search/create/templates/searchCreateAuditCard.tmpl.html',

            scope: {
                validFiles:'=',
                formData:'='
            },
            link:function(scope, el, attrs,ctrl){

                scope.separator=[32,9,13,188,186];
                scope.manualList = [];

                scope.formData = {
                    list: [],
                    files: [],
                    domains: []
                };
                scope.$watch('textList',function(newVal){
                    scope.domainValidationError = '';
                    if (newVal) {
                        var invalid = false;
                        var tmpTextList = newVal.split(/[\s,\;]+/);
                        for (var i in tmpTextList) {
                            if (!scope.validateDomain(tmpTextList[i])) invalid = true;
                        }
                        scope.manualListForm.textList.$invalid = invalid;

                    }
                });

                $rootScope.$on('$stateChangeSuccess', function(event, toState, toParams, fromState, fromParams){
                    scope.resetAuditForm();
                });

                scope.addDomain = function(chip){
                    scope.manualList = [];
                    scope.formData.list.push(chip);
                }

                scope.uploadListModal = function(){
                    scope.manualListForm.$setSubmitted();
                    if(scope.formData.domains.length <= 0 && !scope.textList){
                        return false;
                    }
                    if(scope.manualListForm.textList.$modelValue){
                        scope.formData.list = [];
                        var domainList = scope.manualListForm.textList.$modelValue.split(/[\s,\;]+/);
                        for (var i in domainList) {
                            var domainToInsert=domainList[i].replace(/\uFFFD/g, '').replace(/ /g,'');

                            if (scope.validateDomain(domainToInsert)) {
                                if (scope.formData.list.indexOf(domainToInsert) < 0) {
                                    scope.formData.list.push(domainToInsert);
                                }
                            }
                            else if(domainToInsert != ''){
                                scope.manualListForm.textList.$invalid = true;
                                scope.domainValidationError = 'Please use the following format: corsearch.com.';
                                return;
                            }
                        }
                    }

                    // TODO: finish hiding
                    $(".help-trigger-wrapper").each(function(){
                        $(this).removeClass('clickHovered');
                        $(this).removeClass('stayHovered');
                        $(this).removeClass('hovered');
                    });

                    $mdDialog.show({
                        template: "" +
                        '<md-dialog class="modalScope" aria-label="Modal Directive" style="min-width: 25vw;">' +
                        '<md-toolbar>' +
                            '<div class="md-toolbar-tools">' +
                            '<h2>{{ formData.currentStep == 2 ? "ORDER COMPLETE" : "DOMAIN AUDIT" }}</h2>' +
                            '<span flex></span>' +
                            '<md-button class="md-icon-button close-modal-x" ng-click="closeModal()">' +
                                '<md-icon class="zmdi zmdi-close" aria-label="Close dialog"></md-icon>' +
                            '</md-button>' +
                            '</div>' +
                            '</md-toolbar>' +
                        '<md-dialog-content aria-label="Modal Content">' +
                        '<div search-create-audit-modal assoc-data="formData"></div>' +
                        '  </md-dialog-content>' +
                        '</md-dialog>',
                        scope: scope,
                        preserveScope: true,
                        fullscreen: true,
                        clickOutsideToClose: scope.clickToClose

                        //controller: scope.modalController
                    }).then(function() {
                        scope.resetAuditForm();
                    }, function(){
                        scope.resetAuditForm();
                    });

                };

                scope.validateDomain = function(domain) {
                    //TODO: This will blow out any domains after an invalid one, don't do that
                    if (domain.match(/^([^;\"\s,~`\<\>\/\\\@\!#\$\%^\&\*\(\)\{\}\|\?\]\_\[\.]+)\.([^\.;\"\s,~`\<\>\/\\\@\!#\$\%^\&\*\(\)\{\}\|\?\]\_\[]+){1,18}\.{0,1}([^\.;\"\s,~`\<\>\/\\\@\!#\$\%^\&\*\(\)\{\}\|\?\]\_\[]+)$/i)) return true;
                    //if (domain.match(/^(?!:\/\/)([a-zA-Z0-9]+\.)?[a-zA-Z0-9][a-zA-Z0-9-]+\.[a-zA-Z]{2,18}?$/i)) return true;

                };
                scope.closeModal = function() {
                    scope.resetAuditForm();
                    $mdDialog.cancel();

                };

                scope.resetAuditForm = function() {

                    // reset upload card
                    scope.textList = '';
                    scope.manualListForm.textList.$setViewValue('');
                    scope.manualListForm.textList.$modelValue = '';
                    scope.manualListForm.$setPristine();
                    scope.manualListForm.$setUntouched();
                    scope.validFiles = [];
                    scope.formData.domains = [];
                    scope.formData.files = [];
                    scope.formData.list = [];

                };

            }
        };
    }


})();