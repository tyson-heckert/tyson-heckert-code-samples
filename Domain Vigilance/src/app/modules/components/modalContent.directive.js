/**
 * Created by Tyson Heckert on 12/3/15.
 */
var app = angular.module('app.components');

app.directive('modalContent', ['$mdDialog', function($mdDialog) {

    return {
        scope: {
            templateUrl: '=',
            assocData: '=',
            modalController: '=',
            clickToClose: '=',
            paddedModal: '=',
            invokeOnDone:'&',
            directiveName: '=',
            modalHeader: '=',
        },
        link: function (scope, el, attr) {

            el.bind("click", function(){

                //debugger;
              scope.openModal();
            });
            scope.openModal = function(ev) {
                if (attr.templateUrl) {
                    $mdDialog.show({
                        templateUrl: scope.templateUrl,
                        scope: scope,
                        preserveScope: true,
                        controller: scope.modalController,
                        clickOutsideToClose: scope.clickToClose

                }).then(function($state) {
                        if ($state == 'done') {
                            if (angular.isDefined(attr.invokeOnDone)) {
                                scope.invokeOnDone();
                            }
                        }
                    });
                } else {
                    $mdDialog.show({
                        template: "" +
                        '<md-dialog class="modalScope" aria-label="Modal Directive" style="min-width: 25vw; '+attr.customStyle+'">' +
                        '' +
                        '<md-toolbar ng-if="modalHeader">' +
                        '<div class="md-toolbar-tools">' +
                        '<h2 style="text-transform:uppercase">{{modalHeader}}</h2>' +
                        '<span flex></span>'+
                        '<md-button class="md-icon-button" ng-click="closeModal()">'+
                        '<md-icon class="zmdi zmdi-close" aria-label="Close dialog"></md-icon>'+
                        '</md-button>' +
                        '</div>' +
                        '</md-toolbar>' +
                        '  <md-dialog-content ng-class="{\'padded-content-page\': paddedModal}"  aria-label="Modal Content">' +
                            '<div '+attr.directiveName+' assoc-data="assocData"></div>' +
                        '  </md-dialog-content>' +
                        '</md-dialog>',
                        scope: scope,
                        preserveScope: true,
                        fullscreen: true,
                        clickOutsideToClose: scope.clickToClose

                        //controller: scope.modalController
                    }).then(function($state) {
                        if ($state.state == 'done') {
                            if (angular.isDefined(attr.invokeOnDone)) {
                                scope.invokeOnDone({data:$state});
                            }
                        }
                    });
                }


            };
            scope.closeModal = function(state) {
                if (!state) state='';
                $mdDialog.hide(state);
            };

        }
    }
}]);