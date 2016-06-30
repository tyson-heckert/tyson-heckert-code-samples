(function() {
    var app = angular.module('app.components');

    app.directive('csReverseWhoisSearchDialog', ['$mdDialog', 'socketService', 'utilsService', '$state', csReverseWhoisSearchDialogController]);

    function csReverseWhoisSearchDialogController($mdDialog, socketService, utilsService, $state){
            return {
                restrict: 'AE',
                scope: {
                    openModal: '=',
                    domain: '=',
                    keywords: '=',
                    rwConfigs:'=',
                    pricingEnabled:'='
                },
                controller:app.classy.controller({
                    name: 'csReverseWhoisSearchDialogController',

                    inject: ['$scope', '$rootScope', '$http', '$interval', '$filter', '$sce', '$window', '$timeout', 'utilsService', '$mdDialog', 'socketService', '$state', '$stateParams'],

                    data: {
                        searchableFields : [
                            {val: 'all', name: 'Entire Record'},
                            {val: 'name', name: 'Registrant Name'},
                            {val: 'email', name: 'Registrant Email'},
                            {val: 'comp_name', name: 'Registrant Org'},
                            {val: 'phone', name: 'Registrant Phone'},
                            {val: 'address', name: 'Registrant Address'}]
                    },
                    init: function() {
                        var _this = this;
                        _this.$scope.scanData = JSON.parse(window.localStorage.getItem('_this_scan_'));
                        if(!_this.$scope.scanData){
                            _this.$scope.openReverseWhoisMenu = function(){ utilsService.toast('No Scan Data Available', 'error')};
                            return false;
                        }

                        if(location.hostname != 'dv.corsearchdbs.com'){
                            _this.$scope.pricingEnabled = true;
                        }

                    },

                    watch: {
                        '{object}openModal':'openDialogModal'
                    },

                    methods: {
                        /**
                         * open dialog modal
                         */
                        cleanConfigs:function(configs){
                            var newConfigs=[];
                            for (var i in configs) {
                                if (configs[i]['term']) {
                                    newConfigs.push(configs[i])
                                }
                            }
                            if (newConfigs.length ==0) {
                                newConfigs=[{term:"",field:"all",field_name:"Entire Record",op:"or"}]
                            }
                            return newConfigs;

                        },
                        openDialogModal: function(){
                            var _this = this;

                            if(location.hostname != 'dv.corsearchdbs.com'){
                                _this.$scope.pricingEnabled = true;
                            }

                            if (!_this.$scope.rwConfigs || _this.$scope.rwConfigs.length <= 0) {
                                _this.$scope.reverseWhois_configs = [{
                                    term: angular.copy(_this.$scope.keywords) || '',
                                    field: 'all',
                                    field_name: 'Entire Record',
                                    op: 'or'
                                }];
                            } else {

                                _this.$scope.reverseWhois_configs=_this.$scope.cleanConfigs(angular.copy(_this.$scope.rwConfigs));
                            }
                            if (_this.$scope.openModal) {
                                if(_this.$scope.domain){
                                    _this.$scope.search_domain = _this.$scope.domain.name;
                                }
                                $mdDialog.show({
                                    templateUrl: 'app/modules/components/template/csReverseWhoisSearchDialog.tmpl.html',
                                    scope: _this.$scope,
                                    preserveScope: true,
                                    escapeToClose: false,
                                }).then(function ($state) {
                                    _this.$scope.openModal = false;
                                    if (typeof $state  === 'undefined'|| !$state) {
                                        _this.$scope.openModal = false;
                                    }
                                    _this.$scope.keywords='';
                                    _this.$scope.reverseWhois_config={};
                                });
                            } else{
                                return false;
                            }
                        },

                        fieldMapper: function(){
                            var _this = this;

                            var field= _this.$scope.selectedField;
                            var mapper={'whois.registrant_name':'name',
                                'whois.registrant_email' :'email',
                                'whois.registrant_phone' : 'phone',
                                'whois.registrant_address' : 'address'};

                            return mapper[field] ? mapper[field] : 'all';
                        },

                        /**
                         * Menu dialog
                         * Other options can be add in the list
                         * Create Reverse Search is the only for the moment.
                         * @param $mdOpenMenu
                         * @param ev
                         */
                        openReverseWhoisMenu: function ($mdOpenMenu, ev) {
                            originatorEv = ev;
                            $mdOpenMenu(ev);
                        },

                        addSearchTerm: function(){
                            var _this = this;

                            if(_this.$scope.reverseWhois_configs.length < 5){
                                _this.$scope.reverseWhois_configs.push({
                                    term: '',
                                    field: 'all',
                                    field_name: 'Entire Record',
                                    op:'or'
                                });
                            }
                        },

                        removeSearchTerm: function(indexVal){
                            var _this = this;
                            _this.$scope.reverseWhois_configs.splice(indexVal, 1);
                            if (_this.$scope.reverseWhois_configs.length == 0) {
                                _this.$scope.reverseWhois_configs.push({
                                    term: '',
                                    field: 'all',
                                    op:'or'
                                });
                            }
                        },

                        /**
                         * Cancel Search
                         */
                        abort: function(){
                            var _this=this;
                            $mdDialog.hide();
                            _this.$scope.openModal=false;
                        },

                        updateReadableField: function($index){
                            var _this=this;

                            angular.forEach(_this.$scope.searchableFields, function(value, key){
                                if(value.val == _this.$scope.reverseWhois_configs[$index].field){
                                    _this.$scope.reverseWhois_configs[$index].field_name = value.name
                                }
                            });
                        },


                        /**
                         * Submit Reverse Processing Whois
                         * Engineered by Iman
                         */
                        submit: function(){
                            var _this = this;

                            // Additional validation safeguard
                            if(_this.$scope.reverseWhoisDialogForm.$invalid){
                                return false;
                            }

                            var keywords=[];
                            console.log(_this.$scope.reverseWhois_configs);
                            angular.forEach(_this.$scope.reverseWhois_configs, function(value, key){
                                if (keywords.indexOf(value.term.toLowerCase()) == -1 )keywords.push(value.term.toLowerCase());
                            });

                            /**
                             * Scan object taxonomy
                             * @type {{keyword: (*|columns.keyword|{name, class, field, img, filter, default_width, type, sortable, searchable, colorable, category, visible, required}|string|Array), name: (*|columns.keyword|{name, class, field, img, filter, default_width, type, sortable, searchable, colorable, category, visible, required}|string|Array), referenceCode: (*|columns.keyword|{name, class, field, img, filter, default_width, type, sortable, searchable, colorable, category, visible, required}|string|Array), fields: string, trademark_keywords: Array, brand: string}}
                             */
                            var scan = {

                                /**
                                 * ReverseWhois Config
                                 */
                                rw_query:        _this.$scope.reverseWhois_configs,

                                /**
                                 * Search name
                                 */
                                name:           _this.$scope.reverseWhois_config.search_name,

                                /**
                                 * Search reference code
                                 * Required value and defined in the manual create form
                                 */
                                referenceCode:  _this.$scope.reverseWhois_config.search_name,

                                /**
                                 * Required array
                                 * For the purpose of this widget is not needed to fill the array
                                 */
                                trademark_keywords: keywords,

                                /**
                                 * Brand information
                                 */
                                brand:          _this.$scope.brand,

                                /**
                                 * Billing code
                                 */
                                billing_code:   _this.$scope.search_billing_code,

                                /**
                                 * Required function name
                                 * For the purpose of this widget function name does not change.
                                 */
                                func:           'reverseWhoisForm'
                            };


                            var search = new Promise(function(resolve, reject) {
                                socketService.ws.evalremote('search/create_search.js', scan, function (res) {
                                    if (res.error) reject(new Error(res.error));

                                    resolve(res);
                                });
                            }).then(function(response){
                                _this.$scope.openModal=false;

                                _this.$mdDialog.hide();

                                if(_this.$state.current.name == 'triangular.admin-default.get-started'){
                                    _this.$state.go('triangular.admin-default.dashboard');
                                } else {
                                    _this.utilsService
                                            .toast('<a href="/">Search has been successfully created.<br/>Click here to go to home</a>', 'success');
                                }

                            }).catch( function(error){

                            });
                        }

                    }
                }),
                link: function(scope, elements, attributes){}
            }
        }

})();
