"use strict";
var path=require('path');
var config = require(path.join('..','..','config'));
var db = require('./db');
var Model = require('./Model');
var Domain = require(path.join(config.server_path, 'db', 'classes', 'Domain'));
var Brand = require(path.join(config.server_path, 'db', 'classes', 'Brand'));
var User = require(path.join(config.server_path, 'db', 'classes', 'User'));
var PurgedScans = require(path.join(config.server_path, 'db', 'classes', 'PurgedScans'));
var EsbServices = require(path.join(config.server_path,'modules','core','EsbServices'));
var async = require('async');
var AggPipeLineFactory = require(path.join(config.server_path,'db','server','aggregation','PipeFactory'));


class Scan extends Model {

    static byID(ID){
        return super.byKey('_id', ID, true);
    }
    static byCustomerId (customer_id) {

        return super.byKey( 'customer_id', customer_id, true);
    };

    static byBrandId (brand_id) {

        return super.byKey( 'brand_id', brand_id, true);
    };

    static byUserId (user_id) {

        return super.byKey( 'user_id', user_id, true);
    };

    static byScanStatus (status) {

        return super.byKey('scan_status', status);
    };

    static _getDefaultPipeline(){

        var aggCntx = {};
        aggCntx.type = "Scan";
        return AggPipeLineFactory.getPipeline(aggCntx); // load the default pipeline for this model
    }


    // stub to over-ride default sort behavior
    static _addDynamicSort(sort,$project){
        var arrayMap = {"trademark_keywords":"first_keyword"}
        var dynamicTextSort =false;
        if(sort){
            let sortFields = Object.keys(sort);
            sortFields.forEach(function(sortField){
                let hits = sortField.match('lower_dynamic');
                if(hits && hits.length > 0){
                    dynamicTextSort=true;
                    // insert the  field into projection for lower case needs
                    let csFieldName = sortField.substring(0,sortField.indexOf('_lower_dynamic'));
                    //for array's pick only the first token or element / converted thru special aggregation pipeline
                    if (arrayMap[csFieldName]){
                        csFieldName =arrayMap[csFieldName];
                    }
                    let csFiledObj={};
                    csFiledObj['$toLower']='$'+csFieldName;
                    $project[sortField]=csFiledObj;
                }

            });
        }

    }

    static getColumnsInfo(col, value) {
        var columns = {
            _id: {
                name: '_id',
                class: '',
                field: '_id',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            total_keywords_return: {
                name: 'Total Keywords Return',
                class: '',
                field: 'total_keywords_return',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            account_id: {
                name: 'Account ID',
                class: '',
                field: 'account_id',
                img: false,
                filter: true,
                default_width: 45,
                type: 'ObjectID',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            user_id: {
                name: 'User ID',
                class: '',
                field: 'user_id',
                img: false,
                filter: true,
                default_width: 45,
                type: 'ObjectID',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            name: {
                name: 'Reference Title',
                class: '',
                field: 'name',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: true,
                colorable: false,
                category: 'Scan'
            },
            brand: {
                name: 'Client',
                class: '',
                field: 'brand.name',
                img: false,
                filter: true,
                default_width: 45,
                type: 'ObjectID',
                sortable: true,
                searchable: true,
                colorable: false,
                category: 'Scan'
            },
            trademark_keywords: {
                name: 'Search Term',
                class: '',
                field: 'trademark_keywords',
                img: false,
                filter: true,
                default_width: 45,
                type: 'array',
                sortable: true,
                searchable: true,
                colorable: false,
                category: 'Scan'
            },
            include_domains: {
                name: 'Included Domains',
                class: '',
                field: 'include_domains',
                img: false,
                filter: true,
                default_width: 45,
                type: 'array',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            exclude_domains: {
                name: 'Excluded Domains',
                class: '',
                field: 'exclude_domains',
                img: false,
                filter: true,
                default_width: 45,
                type: 'array',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            type: {
                name: 'Search Type',
                class: '',
                field: 'searchType',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: true,
                colorable: false,
                category: 'Scan'
            },
            tld_settings: {
                name: 'Tld_settings',
                class: '',
                field: 'tld_settings',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            last_scan_date: {
                name: 'Last Scan Date',
                class: '',
                field: 'last_scan_date',
                img: false,
                filter: true,
                default_width: 45,
                type: 'date',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            next_scan_date: {
                name: 'Next Scan Date',
                class: '',
                field: 'next_scan_date',
                img: false,
                filter: true,
                default_width: 45,
                type: 'date',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            schedule_rate: {
                name: 'Schedule Rate',
                class: '',
                field: 'schedule_rate',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            scan_status: {
                name: 'Status',
                class: '',
                field: 'scan_status',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: true,
                colorable: false,
                category: 'Scan'
            },
            refresh_scan: {
                name: 'Refresh Search',
                class: '',
                field: 'refresh_scan',
                img: false,
                filter: true,
                default_width: 45,
                type: 'boolean',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            api_call: {
                name: 'Api Call',
                class: '',
                field: 'api_call',
                img: false,
                filter: true,
                default_width: 45,
                type: 'ObjectID',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            last_action: {
                name: 'Last Action',
                class: '',
                field: 'last_action',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            last_action_by: {
                name: 'Last Action By',
                class: '',
                field: 'last_action_by',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            last_action_date: {
                name: 'Last Action Date',
                class: '',
                field: 'last_action_date',
                img: false,
                filter: true,
                default_width: 45,
                type: 'date',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            total_domains: {
                name: 'Total Domains',
                class: '',
                field: 'total_domains',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            total_new: {
                name: 'Total New',
                class: '',
                field: 'total_new',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            schedule_start_date: {
                name: 'Schedule Start Date',
                class: '',
                field: 'schedule_start_date',
                img: false,
                filter: true,
                default_width: 45,
                type: 'date',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            schedule_end_date: {
                name: 'Schedule End Date',
                class: '',
                field: 'schedule_end_date',
                img: false,
                filter: true,
                default_width: 45,
                type: 'date',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            status: {
                name: 'Status',
                class: '',
                field: 'status',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            created_by: {
                name: 'Created By',
                class: '',
                field: 'created_by',
                img: false,
                filter: true,
                default_width: 45,
                type: 'text',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            created_at: {
                name: 'Created',
                class: '',
                field: 'created_at',
                img: false,
                filter: true,
                default_width: 45,
                type: 'date',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            updated_at: {
                name: 'Updated At',
                class: '',
                field: 'updated_at',
                img: false,
                filter: true,
                default_width: 45,
                type: 'date',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            deleted_at: {
                name: 'Deleted At',
                class: '',
                field: 'deleted_at',
                img: false,
                filter: true,
                default_width: 45,
                type: 'date',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            },
            quick_domain_id: {
                name: 'quick_domain_id',
                class: '',
                field: 'quick_domain_id',
                img: false,
                filter: true,
                default_width: 45,
                type: 'ObjectID',
                sortable: true,
                searchable: false,
                colorable: false,
                category: 'Scan'
            }
        };
        for(var i in columns){
            columns[i].key = i;
        }

        if(col && columns[col] != undefined){
            if(value && columns[col][value] != undefined){
                return columns[col][value];
            }if(value){
                return null;
            }else{
                return columns[col];
            }
        }else if(col){
            return null;
        }
        return columns;
    };
}

Scan.modelName = 'Scan';
Scan.collection = 'scans';
Scan.model = db.model('scan', db.objectSchema, 'scans');
Scan.keys = ["_id", "account_id", "user_id"];

Scan.requiredColumns = [
    '_id',
    'name',
    'scan_status',
    'searchType',
    'total_domains',
    'total_processing',
    'total_keywords_return',
    'brand',
    'brand_id',
    'trademark_keywords',
    'updated_at',
    'user_id',
    'schedule_name',
    'schedule_rate',
    'schedule_start_date',
    'schedule_end_date',
    'quick_domain_id'
];


/**
 *
 * scan statuses
 */
Scan.statuses = [
    // 0
    {
        key: 'searching',
        value: 'Searching'
    },

    // 1
    {
        key: 'filter_domains',
        value: 'Filter Domains'
    },

    // 2
    {
        key: 'collecting_data',
        value: 'Collecting Data'
    },

    // 3
    {
        key: 'ready',
        value: 'Ready'
    },

    // 4
    {
        key: 'error',
        value: 'Error'
    },

    //5
    {
        key:'resubmit',
        value:'Resubmit'
    }

];


/**
 * filters for this model
 * dashboardFilterDeclarations
 */
Scan.filters = {
    // 0
    searching: { // test filter with OR statement
        name: Scan.statuses[0].value,
        default: 0, //on or off
        priority: 1,
        filter: {
            $match:
            {
                "field":"scan_status","value": Scan.statuses[0].value
            }
        },
        group: 'all', // filter group by default all
        clearAll: 0, // able to select multiple? default off
        clearGroup: 1
    },

    // 1
    filter_domains: {
        name: Scan.statuses[1].value,
        default: 0,
        priority: 2,
        filter: {
            $match:
            {
                "field":"scan_status","value": Scan.statuses[1].value
            }
            // I removed this as the second or was breaking the code - mo
            //$and: {
            //    $or: [
            //        {scan_status: Scan.statuses[1].value},
            //        {scan_status: 'Filter Results'}
            //        //{"field": "scan_status", "value": Scan.statuses[1].value},
            //        //{"field": "scan_status", "value": 'Filter Results'}
            //    ]
            //}
        },
        group: 'all', // filter group by default all
        clearAll: 0, // able to select multiple? default off
        clearGroup: 1
    },

    // 3
    collecting_data: {
        name: Scan.statuses[2].value,
        default: 0,
        priority: 3,
        filter: {

            $match:
            {
                "field":"scan_status","value": Scan.statuses[2].value
            }

        },
        group: 'all', // filter group by default all
        clearAll: 0, // able to select multiple? default off
        clearGroup: 1
    },

    // 4
    ready: {
        name: Scan.statuses[3].value,
        default: 0,
        priority: 4,
        filter: {
            $and: {
                $or: [
                    {scan_status: Scan.statuses[3].value},
                    {scan_status: 'Ready for Review'}
                    //{"field": "scan_status", "value": Scan.statuses[1].value},
                    //{"field": "scan_status", "value": 'Filter Results'}
                ]
            }
        },
        group: 'all', // filter group by default all
        clearAll: 0, // able to select multiple? default off
        clearGroup: 1
    }
};

Scan.getPriority = function(total_domains){
    if(total_domains > 100000){
        return 1;
    }else if(total_domains > 10000){
        return 2;
    }else if(total_domains > 2000){
        return 3;
    }else if(total_domains > 1000){
        return 4;
    }else if(total_domains > 100){
        return 5;
    }else if(total_domains > 50){
        return 6;
    }else if(total_domains > 25){
        return 7;
    }else if(total_domains > 10){
        return 8;
    }else{
        return 10;
    }

};

Scan.edit=function(scan_id, formData, USER,__systemLog,callback){
    if(scan_id){
        Scan.model.findById(scan_id,function(err,res) {
            if (err) return callback({error: err});
            if (!res) return callback({error: 'Search not found'});
            var scan = res.toObject();
            if (USER.user().role.toLowerCase() == 'super administrator' || scan.account_id == USER.user().account_id) {
                //Brand.model.findById(data.brand_id,function(err,res) {
                //    if (err) return callback({error: err});
                //    if (!res) return callback({error: 'Search not found'});
                //    formData.brand = res.toObject();
                //    Scan.update({_id: scan_id}, formData);
                //    return callback({message:'Search updated'});
                //});
                Scan.update({_id: scan_id}, formData);
                return callback({message:'Search updated'});
            } else {
                return callback({error:'Permission Denied'});
            }
        });
    }else{
        return callback({error: 'Update failed.'});
    }
};

module.exports = Scan;
