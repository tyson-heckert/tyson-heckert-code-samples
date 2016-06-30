/**
 * Created by Iman Nassirian on 11/9/15.
 */

(function() {

    var Module = angular.module('app.components');
    Module.directive('csBrand',['socketService','$log','$timeout', '$q',function(socketService,$log,$q,$timeout){
        return {
            restrict:'AE',
            templateUrl:'app/modules/components/template/csBrandPicker.tmpl.html',
            scope:{
                //selectedBrand:'=brands',
                addNew:'=',
                formData:'=',
                isRequired: '=',
                brandList: '='
            },
            require: 'ngModel',
            link:function(scope,el,attr,ngModelCtrl){
                ngModelCtrl.$touched=false;

                ngModelCtrl.$formatters.push(function(modelValue) {
                    if (modelValue) scope.selectedBrand=modelValue._id;
                    return modelValue || {};
                });
                ngModelCtrl.$render = function() {
                    scope.formData   = ngModelCtrl.$viewValue;
                };
                scope.noCache=false;
                //scope.selectedBrand=null;
                scope.brandName=null;
                scope.isDisabled=false;
                scope.brandList=[];
                scope.isRequired = scope.isRequired ? scope.isRequired : false;
                scope.addNewBrandFlag = false;
                scope.newBrandText = '';


                scope.init=function(){
                    scope.getBrands('firstLoad');
                    scope.selectCorrectBrand(scope.selectedBrand);
                };

                scope.getBrands=function(type){
                    var params={}
                    params.type='Brand';
                    params.func = 'getData';
                    params.select='_id name account_id'
                    params.pagination={
                        page:1,
                        per:10000,
                        sort:[{field:'name',direction:'asc'}]
                    }
                    socketService.ws.evalurl('core/genericController.js', params, function(res) {
                        scope.brandList=res.results;
                        scope.$apply();
                        if (type == 'refresh') {
                                    scope.selectedBrand=scope.selectedBrandId;
                            scope.$apply();
                                    return;
                        }
                    });
                };

                scope.addNewBrand = function(){

                    if(!scope.addNewBrandFlag){
                        return false;
                    }

                    var params = {};
                    params.func = 'save';
                    params.entity = {name: scope.newBrandText, depth:0};

                    socketService.ws.evalurl('settings/brandsController.js', params, function(res) {
                        if (!res.error) {
                            scope.selectedBrandId=res._id;
                            //scope.selectedBrand = '';
                            scope.newBrandText = '';
                            scope.getBrands('refresh');
                            scope.hasError=false;
                            scope.addNewBrandFlag = false;
                        } else {
                            scope.hasError = true;
                            scope.hasErrorStyle='border-bottom: 1px solid #e5202e; color: #e5202e;';
                            if(typeof res.error === 'string'){
                                scope.errorMessage = res.error;
                            }else{
                                scope.errorMessage = res.error[0];
                            }

                        }
                        scope.$apply();
                    });

                };
                scope.hasErrorStyle='';
                scope.callBack=function(){
                    scope.getBrands('refresh');
                }
                scope.selectCorrectBrand=function(val) {
                    if (!val) return {};
                    for (var i in scope.brandList) {
                        if (scope.brandList[i]._id == val ){
                            return scope.brandList[i]
                        }
                    }
                    return {};

                }

                scope.$watch('newBrandText',function(newVal){
                    if(newVal){
                        scope.hasError=false;
                        scope.hasErrorStyle='';
                    }
                })

                scope.$watch('selectedBrand',function(){
                    ngModelCtrl.$setViewValue(scope.selectCorrectBrand([scope.selectedBrand]));
                    var isValid=false;
                    if (scope.selectedBrand && scope.selectedBrand != '+addnew'){
                        isValid=true;
                    }
                    ngModelCtrl.$setValidity('required', isValid);
                    ngModelCtrl.$errorMessage='Client is required';
                });

                scope.brandSelectChange = function(){
                    if(scope.selectedBrand == '+addnew'){
                        scope.addNewBrandFlag = true;
                        scope.hasError=false;
                    } else{
                        scope.addNewBrandFlag = false;
                    }

                    //console.log(scope.selectedBrand);
                };

                scope.showAddNewForm=function(){
                    scope.addNewForm=true;
                };
                $timeout(function(){
                    var select=el.find('select');
                    select.bind('blur',function(){
                        if (!ngModelCtrl.$touched) {
                            ngModelCtrl.$touched=true;
                        }
                    })
                });

                scope.refreshBrandList=function(data){
                    scope.selectedBrandId=data['object']['_id'];
                    scope.getBrands('refresh');
                    console.log(data);
                    //scope.selectedBrand=data['object']['_id'];
                };
                 scope.init();

                // Delay the add so we can properly check for a cancel
                scope.blurAdd = function(){
                    setTimeout(function(){scope.addNewBrand()}, 500);
                };

                //scope.selectedBrand=scope.brandList[0];
            }

        }
    }]);
})();