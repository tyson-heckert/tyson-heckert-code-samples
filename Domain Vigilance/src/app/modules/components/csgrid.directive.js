(function(){

    var app = angular.module('app.components');

    app.directive("csGrid", ['utilsService', 'socketService', 'uiGridConstants', csGrid]);
    app.directive("csGridRowTemplate", csGridRowTemplate);
    app.directive("csRightClick", ['$parse', csRightClick]);

    /**
     * @csdoc csGrid
     * @type directive
     * @name cs grid
     * @description
     * Directive displaying a table using UI Grid
     * test line 2
     * @contributors davin, arron
     * @modified 2015-11-24
     * @tags grid, uigrid, table
     * @attributes
     * grid-data: an array of the data that should be displayed
     * grid-call-server: function that updates data above to current params (optional)
     * grid-options: optional ui grid options (use ui grid docs) test
     * grid-pagination: optional the directive will update this value for your server side code
     * grid-get-data: event triggered when the grid requires new data (contains grid-pagination)
     * grid-calculate-height: function called when view changes, return height for grid
     *
     * Directive displaying a table using UI Grid
     * grid-data - an array of the data that should be displayed
     * grid-call-server - function that updates data above to current params (optional)
     * grid-options - optional ui grid options (use ui grid docs)
     * grid-pagination - optional the directive will update this value for your server side code
     * grid-get-data - event triggered when the grid requires new data (contains grid-pagination)
     * grid-calculate-height - function called when view changes, return height for grid
     */
    function csGrid(utilsService, socketService, uiGridConstants){
        return {
            restrict: 'AE',
            template:
            '<div ui-grid="gridOptions" '+
            '     ui-grid-resize-columns ui-grid-move-columns ui-grid-auto-resize ui-grid-pinning' +
            '     ng-style="getTableStyle()"' +
            '     ng-class="{\'cs-expanded-view\': !gridCompactView}"' +
            '></div>',
            link:function(scope, el, attr){

            },
            scope: {
                gridData: '=',
                gridOptions: '=',
                gridPagination: '=',
                gridCallServer: '=',
                gridCalculateHeight: '=',
                gridClick: '=',
                gridRightClick: '=',
                gridViewChanged: '=',
                gridCompactView: '='
            },
            controller: app.classy.controller({
                inject: ['$scope', '$http', '$interval', '$timeout', 'utilsService', 'socketService', '$mdToast'],

                data:{
                    gridOptionsDefault: {}, // default options for csGrid directive
                    gridOptionsCopy: {}, // default options passed into csGrid directive
                    gridPaginationCopy: {}, // default pagination passed into csGrid directive
                    skipUpdatePagination: false // flag for skipping watch func
                },

                init: function()
                {
                    var _this = this;
                    //console.log("dim grid loadedasdfasdfasdfasdf");
                    _this.$scope.scrollbarHeight = _this.$scope.getScrollBarWidth();

                    _this.$scope.gridOptionsCopy = angular.copy(_this.$scope.gridOptions); // save grid options passed into directive
                    _this.$scope.gridPaginationCopy = angular.copy(_this.$scope.gridPagination); // save grid options passed into directive

                    _this.$scope.gridOptionsDefault = {
                        useExternalSorting: true,
                        enableColumnResizing: true,
                        enableRowSelection: false,
                        enableFullRowSelection: false,
                        enableSelectAll: false,
                        excessRows: 100,
                        excessColumns: 10,
                        //fastWatch: true,
                        onRegisterApi: function( gridApi ) {
                            _this.$scope.gridApi = gridApi;

                            _this.$scope.gridApi.core.on.sortChanged( _this.$scope, _this.$scope.gridSortChanged );
                            _this.$scope.gridApi.core.on.columnVisibilityChanged( _this.$scope, _this.$scope.columnVisibilityChanged);
                            _this.$scope.gridApi.colMovable.on.columnPositionChanged(_this.$scope, _this.$scope.gridColumnPositionChanged);
                            _this.$scope.gridApi.colResizable.on.columnSizeChanged(_this.$scope, _this.$scope.gridColumnSizeChanged);
                            _this.$scope.gridApi.pinning.on.columnPinned(_this.$scope, _this.$scope.gridColumnPinned);

                            _this.$scope.gridApi.grid.appScope = _this.$scope.$parent; // reassign appscope to the parent scope
                            _this.$scope.gridApi.grid.csGridScope = _this.$scope;
                        },
                        rowTemplate : '<cs-grid-row-template></cs-grid-row-template>'
                        //appScopeProvider: _this.$scope.$parent // stupid thing does not work
                    };

                    _this.$scope.paginationDefault = {
                        "page": 1,
                        "per": 25,
                        "sort": [],
                        "filter": [],
                        "search": []
                    };

                    if(!_this.$scope.gridData){
                        _this.$scope.gridData = [];
                    }

                    _this.$scope.setupGridOptions();
                    _this.$scope.triggerCallServer(); // let's initialize the grid with some data

                },

                watch: {
                    '{object}gridData': 'updateGridData',
                    '{object}gridOptions': 'updateGridOptions',
                    '{object}gridPagination': 'updatePagination',
                    'gridCompactView': 'updateView'
                },

                methods: {

                    // setup UI grid options
                    setupGridOptions: function(){
                        var _this = this;
                        var i, j;
                        var sortPriority = 0;


                        if(_this.$scope.gridOptionsDefault){
                            var tempOptions = angular.merge(_this.$scope.gridOptionsCopy, _this.$scope.gridOptionsDefault);
                        }else{
                            var tempOptions = _this.$scope.gridOptionsCopy;
                        }

                        if(_this.$scope.gridOptionsDefault){
                            var tempPagination = angular.merge(_this.$scope.paginationDefault, _this.$scope.gridPagination);
                        }else{
                            var tempPagination = _this.$scope.paginationDefault;
                        }

                        //if(!_this.$scope.gridPagination){
                        //    _this.$scope.gridPagination = angular.copy(_this.$scope.paginationDefault);
                        //}


                        //_this.$scope.gridOptions = tempOptions;
                        //_this.$scope.gridPagination = tempPagination;
                        for(i in tempOptions){
                            _this.$scope.gridOptions[i] = tempOptions[i];
                        }
                        for(i in tempPagination){
                            _this.$scope.gridPagination[i] = tempPagination[i];
                        }

                        _this.$scope.updateView();
                    },

                    // put new data into the grid
                    updateGridData: function(){
                        var _this = this;

                        if(_this.$scope.gridApi){
                            _this.$scope.gridOptions.data = _this.$scope.gridData;
                        }
                    },

                    // set up view (compact or regular)
                    updateView: function(){
                        var _this = this;

                        if(_this.$scope.gridOptions){
                            if(_this.$scope.gridCompactView){
                                _this.$scope.gridOptions.rowHeight = 30;
                                _this.$scope.gridOptions.headerRowHeight = 30;
                            }else{
                                _this.$scope.gridOptions.rowHeight = 60;
                                _this.$scope.gridOptions.headerRowHeight = 60;
                            }
                        }
                    },

                    // setup pagination
                    // this doesn't fire an data update event, it only updates the view (sort arrows, etc)
                    updatePagination: function(newValue, oldValue){
                        var _this = this;

                        var field, dir, columns, i;
                        var priority = 1;


                        if(!_this.$scope.skipUpdatePagination){ // skip this if gridSortChanged function updated pagination
                            // update sorting here

                            if(_this.$scope.gridApi){
                                if( _this.$scope.gridApi.grid.columns &&  _this.$scope.gridApi.grid.columns.length){ // check if grid is already loaded with columns
                                    if(_this.$scope.gridPagination && _this.$scope.gridPagination.sort && _this.$scope.gridPagination.sort.length){ // if pagination exists

                                        _this.$scope.gridApi.grid.resetColumnSorting();

                                        for(i in _this.$scope.gridPagination.sort){ //set the sorts here

                                            field = _this.$scope.gridPagination.sort[i].field;
                                            dir = (_this.$scope.gridPagination.sort[i].direction == uiGridConstants.ASC) ? uiGridConstants.ASC : uiGridConstants.DESC;


                                            columns =  _this.$scope.gridApi.grid.columns.filter(function (column) { // get column by field name
                                                return column.colDef.field === field;
                                            });

                                            if(columns && columns.length){ // if column found, apply the sort
                                                columns[0].sort.priority = priority++;
                                                columns[0].sort.direction = dir;
                                                //_this.$scope.gridApi.grid.sortColumn(columns[0], dir, first); // this will fire an event, so we can't use it
                                            }
                                        }
                                    }else{ // no sorting pagination, reset the sorting
                                        _this.$scope.gridApi.grid.resetColumnSorting();
                                    }

                                   // _this.$scope.triggerCallServer();
                                }
                            }
                        }else{
                            _this.$scope.skipUpdatePagination = false;
                        }
                    },


                    gridSortChanged: function(grid, sortColumns){
                        var _this = this;

                        var sortCols = [];
                        var dir, col, i;

                        if(sortColumns.length > 0){ // create our version of the sort array
                            for(i in sortColumns){
                                dir = sortColumns[i].sort ? sortColumns[i].sort.direction : null;
                                col = sortColumns[i].field;
                                sortCols.push({field: col, direction: dir});
                            }
                        }

                        if(!angular.equals(sortCols, _this.$scope.gridPagination.sort)){
                            _this.$scope.skipUpdatePagination = true;

                            _this.$scope.gridPagination.sort = sortCols;
                            _this.$scope.gridPagination.page = 1;
                            _this.$scope.triggerCallServer();
                        }
                    },

                    gridColumnSizeChanged: function(colDef, deltaChange){
                        var _this = this;

                        //console.log("gridColumnSizeChanged");
                        _this.$scope.triggerViewChanged();
                    },

                    gridColumnPositionChanged: function(colDef, originalPosition, newPosition){
                        var _this = this;

                        //console.log("gridColumnPositionChanged");
                        _this.$scope.triggerViewChanged();
                    },

                    gridColumnPinned: function(colDef){
                        var _this = this;

                        //console.log("gridColumnPinned");
                        _this.$scope.triggerViewChanged();
                    },

                    gridColumnVisibilityChanged: function(colDef){
                        var _this = this;

                        //console.log("gridColumnVisibilityChanged");
                        _this.$scope.triggerViewChanged();
                    },


                    /**
                     * called when pagination changes. maybe better to use watcher on parent controller
                     * need this if we want a refresh button where pagination does not change
                     */
                    triggerCallServer: function(){
                        var _this = this;

                        return;

                        if(_this.$scope.gridCallServer){
                            _this.$scope.gridCallServer(_this.$scope.gridPagination);
                        }
                        //console.log("dim grid call server");
                    },

                    triggerViewChanged: function(){
                        var _this = this;

                        if(_this.$scope.gridViewChanged){
                            _this.$scope.gridViewChanged(_this.$scope.getColumns());
                        }
                        //console.log("cs grid view changed:%o", _this.$scope.getColumns());
                    },

                    getColumns: function(){
                        var _this = this;

                        if(_this.$scope.gridApi){
                            return _this.$scope.gridApi.grid.columns;
                        }else{
                            return [];
                        }
                    },

                    // called when table height should be recalculated
                    getTableStyle: function(){
                        var _this = this;

                        var rowHeight = _this.$scope.gridOptions ? _this.$scope.gridOptions.rowHeight : 30; // your row height
                        var headerHeight = _this.$scope.gridOptions ? _this.$scope.gridOptions.headerRowHeight : 30; // your header height
                        var footerHeight = _this.$scope.gridOptions.gridFooterHeight ? _this.$scope.gridOptions.gridFooterHeight : 30; // your footer height
                        var scrollBarHeight = _this.$scope.scrollbarHeight; // your scrollbarHeight height
                        var minHeight = 345;
                        var calcHeight = 0;

                        if(_this.$scope.gridCalculateHeight){ // if a height function is available, use this function instead
                            calcHeight = _this.$scope.gridCalculateHeight();
                        }else{
                            if(_this.$scope.gridOptions && _this.$scope.gridOptions.data){
                                calcHeight = ((_this.$scope.gridOptions.data.length * rowHeight) + headerHeight + scrollBarHeight + footerHeight);
                                if(calcHeight < minHeight){
                                    calcHeight = minHeight;
                                }
                            }else{
                                calcHeight = minHeight;
                            }
                        }

                        return {
                            height: calcHeight + 'px'
                        }
                    },

                    getScrollBarWidth: function() {
                        var $outer = $('<div>').css({visibility: 'hidden', width: 100, overflow: 'scroll'}).appendTo('body'),
                            widthWithScroll = $('<div>').css({width: '100'}).appendTo($outer).outerWidth(true);
                        $outer.remove();
                        return 100 - widthWithScroll;
                    },

                    rowClick: function($event, row, col){
                        var _this = this;
                        var item = angular.copy(row.entity);
                        delete item.$$hashKey;

                        if(_this.$scope.gridClick){
                            _this.$scope.gridClick(item, col.field, $event);
                        }
                    },

                    rowRightClick: function($event, row, col){
                        var _this = this;
                        var item = angular.copy(row.entity);
                        delete item.$$hashKey;

                        if(_this.$scope.gridRightClick){
                            _this.$scope.gridRightClick(item, col.field, $event);
                        }
                    }


                }

            })
        };
    }

    /**
     * Helper directive for csGrid
     * Contains the template for the row of each grid
     */
    function csGridRowTemplate(){
        return {
            restrict: 'E',
            template:
            //'<div class="cs-grid-row" ng-class="{\'is-selected\': row.entity.isSelected, \'is-hover\': row.isHover}">' +
            '<div class="cs-grid-row">' +
            '    <div ng-repeat="(colRenderIndex, col) in colContainer.renderedColumns track by col.uid"' +
            '    ui-grid-one-bind-id-grid="rowRenderIndex + \'-\' + col.uid + \'-cell\'"' +
            '    class="ui-grid-cell"' +
            //'    ng-class="{ \'ui-grid-row-header-cell\': col.isRowHeader}"' +
            //'    role="{{col.isRowHeader ? \'rowheader\' : \'gridcell\'}}"' +
            //'    cs-right-click="grid.csGridScope.rowRightClick($event, row, col)"' +
            '    ng-click="grid.csGridScope.rowClick($event, row, col)"'+
            //'    ng-mouseenter="row.isHover = true"'+
            //'    ng-mouseleave="row.isHover = false"'+
            '    ui-grid-cell>' +
            '    </div>' +
            '</div>',
            link:function(scope, el, attrs){

            }
        };
    }


    function csRightClick($parse){
        return function(scope, element, attrs) {
            var fn = $parse(attrs.csRightClick);
            element.bind('contextmenu', function(event) {
                scope.$apply(function() {
                    //event.preventDefault();
                    fn(scope, {$event:event});
                });
            });
        };
    }



})();

