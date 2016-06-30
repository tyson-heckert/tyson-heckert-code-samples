/**
 *
 * searchController handles search for testing
 */
"use strict";
var db = $.db.db;
var Obj = db.model('dim_scans', $.db.ObjectsSchema);

var _ = require('underscore');
var async = require('async');
var path = require('path');
var Scan = require(path.join(config.server_path,'db','classes','Scan'));
var Domain = require(path.join(config.server_path,'db','classes','Domain'));
//var TransactionHelper = require('./TransactionHelper');
var User = require(path.join(config.server_path,'db','classes','User'));
var TransactionHelper = require(path.join(config.server_path, 'db', 'classes', 'TransactionHelper'));
var DomainLookup = require(path.join(config.server_path, 'db', 'classes', 'DomainLookup'));
var Activity=require(path.join(config.server_path,'db','classes','Activity'));
var EsbServices = require(path.join(config.server_path,'modules','core','EsbServices'));
var EsbJobLog = require(path.join(config.server_path,'db', 'classes', 'EsbJobLog'));
var EsbLog = require(path.join(config.server_path,'db', 'classes', 'EsbJobLog'));
//var SearchEmails = require(path.join(config.server_path,'modules','search','SearchEmails'));
var AggPipeLineFactory = require(path.join(config.server_path,'db','server','aggregation','PipeFactory'));


/**
 *
 *  domains
 * scan_id
 * selected = 1 or 0
 */
var chooseDomain = function(){

    var params = {}; // params to find domains by
    var update = {}; // params to update domains to
    // remove excluded domains from operation
    params.exclude = {$ne:true};
    if(data.scan_id){

        // verify scan status and permissions

        // need to add domain log entries and/or activity feed

        params.scan_id = new db.ObjectId(data.scan_id.toString());

        if(data.selected == 1){ // choose a domain
            params.chosen = {
                $ne: 1
            };
            params.selected = 1;

            update = {
                $set: {
                    chosen: 1,
                    selected: 0
                }
            };
        }else{ // un choose the domain
            params.chosen = 1;
            params.selected = 1;

            update = {
                $set: {
                    chosen: 0,
                    selected: 0
                }
            };
        }



        DomainLookup.model.update(params, update,  {multi: true})
        .then(function(res){
            $.push('search.purchase.chooseDomains', {scan_id: data.scan_id}, socket);

            cb( {
                total: res.nModified,
                results: res
            })
        });

    } else {
        cb({error: 'scan id not passed.'});
    }
};


var loadInsights = function() {

    var insights = {};

    if(!data.insightTypes){
        return cb({error:"No insight types provided"});
    }

    if(!data.searchID){
        return cb({error:"No search ID provided"});
    }

    async.each(data.insightTypes, function(item, callback) {
        getInsight(item, data.searchID).then(function(res){
            console.log(res);
            insights[item] = res;
            callback();
        });
    }, function done() {
        if(insights) {
            return cb(insights);
        } else{
            return cb({error: 'Insights failed to load'});
        }
    });

};

var getInsight = function(insightType, searchID) {

    let aggCntx = {};

    // Every insight needs a search id
    aggCntx['@scan_id@']=searchID;

    // Default to normal insight
    var type = 'Insights';

    // Change params based on the actual insight type
    if(insightType == 'expiring'){
        type = 'InsightsExpiring';
        aggCntx['@now_date@'] = new Date().toISOString();
    } else if(insightType == 'traffic'){
        type = 'InsightsTraffic';
    } else{
        aggCntx['@field@'] = insightType;
        aggCntx['@field_path@'] = insightType; // has the full path
    }

    aggCntx['type'] = type; // load the appropriate aggregation file

    return Domain.model.aggregate(AggPipeLineFactory.getPipeline(aggCntx)).allowDiskUse(true).exec();
};


switch(data.func){

    case 'chooseDomain':
        chooseDomain();
        break;


    case 'loadInsights':
        loadInsights();
        break;


    default:
        cb({
            error: 'function not found'
        });
}
