/**
 *
 * a generic controller
 * davin testing for all types
 * should be copied out to separate controllers
 * no validation, just showing everything in DB
 * thanks
 */

var db = $.db.db;
var path = require('path');
var Obj = db.model('domains', $.db.ObjectsSchema);
var _ = require('lodash');
var async = require('async');
var Domain = require(path.join(config.server_path,'db','classes','Domain'));
var DomainLookup = require(path.join(config.server_path,'db','classes','DomainLookup'));
var Scan = require(path.join(config.server_path,'db','classes','Scan'));
var Activity=require(path.join(config.server_path,'db','classes','Activity'));
var User=require(path.join(config.server_path,'db','classes','User'));
var Brand=require(path.join(config.server_path,'db','classes','Brand'));
var Squatter=require(path.join(config.server_path,'db','classes','Squatter'));
var Role=require(path.join(config.server_path,'db','classes','Role'));
var Account = require(path.join(config.server_path,'db','classes','Account'));
var SolrCore = require(path.join(config.server_path,'solr','classes','SolrCore'));
var brandSecurityLevel=['Scan'];
var accountSecurityLevel=['User','Brand'];

var timeStart = Date.now();

var getDomainByScanID = function(){

    var params={};

    if (data.params) {
        params = data.params;
    }

    if(data.type){

        eval(data.type).getDomainsByScanId(data.scan_id, data.select)
            .then(function(results){

                __systemLog.create(USER.getID(),"View "+data.type,null,'Export Scan');
                console.log('result is ready')
                cb(results);
            });
    }else{
        cb({error: 'type not passed.'});
    }
};

/**
 *
 * get all of type
 * type - Scan, Domain, Activity, etc
 * pagination - the pagination object
 *
 */
var getData = function()
{
    var params={};

    if (data.params) {
        params = data.params;
    }

    if(data.type){

        // Add extra filters based on user permissions for data types here
        params=checkPermission(params);
        //console.log(params);

        eval(data.type).getData(params, data.pagination, data.select)
        .then(function(results){
            __systemLog.create(USER.getID(),"View "+data.type,null,data);

            // sending time
            results.time = Date.now() - timeStart;
            cb(results);
        });
    }else{
        cb({error: 'type not passed.'});
    }

};


/**
 *
 * get domain details
 * type - Scan, Domain, Activity, etc
 * id - domain id that we are need to get data for
 */
var getResult = function()
{

    if(data.type && data.type != ""){
        if(data.id && data.id != ""){

            var id = new db.ObjectId(data.id.toString());


            eval(data.type).byId(id)
            .then(function(data){
                if(data && data.length){
                    // check if user has access to this scan
                    cb({
                        data: data[0]
                    });
                }else{
                    cb({error: 'not found.'});
                }
            });
        }else{
            cb({error: 'id not passed.'});
        }
    }else{
        cb({error: 'type not passed.'});
    }
};

/**
 *
 * get filter information
 * should include totals based on pagination also, can add later
 * type - Scan, Domain, Activity, etc
 * params - additional params for totals (scan_id, etc)
 */
var getFilters = function(){
    if(data.type && data.type != ""){

        var filters = eval(data.type).getFilters(data.group);
        var asyncCalls = [];
        var i;

        filters = _.cloneDeep(filters);


        if(data.params){ // not sure where we are going to send this from yet
            var params = data.params;
        }else{
            var params = {};
        }

        params=checkPermission(params)

        for(i in filters){
            addFilterAsync(params, filters[i]);
        }

        async.parallel(asyncCalls, function(err, result) {

            cb({
                filters: filters,
                time: Date.now() - timeStart
            });
        });


    }else{
        cb({error: 'type not passed.'});
    }

    // helper function for adding filters to async
    function addFilterAsync(params, filter){
        var pagination = {};

        pagination.filter = [filter.key];

        asyncCalls.push(
            function(asyncCallback){
                eval(data.type).getData(params, pagination, null, true) // we are not including pagination
                .then(function(res){
                    //filters[filter.key].results = res;
                    filters[filter.key].total = res.total;

                    asyncCallback(null, filter);
                });
            }
        );
    }
};




/**
 *
 * get autocomplete search
 * searchType - Scan, Domain, Activity, etc
 * params - additional params for totals (scan_id, etc)
 */
var getAutocomplete = function(){

    // check user privileges
    var user = USER.user();
    var userRole=user.role.replace(/ /g, "_").toLowerCase();
    var lock = user.account_unlock;

    if(data.searchType && data.searchType != ""){
        var params = [];
        var dataObject = eval(data.searchType);
        if(!dataObject){
            cb({error: 'data type not found'});
        }


        if(data.params){ // not sure where we are going to send this from yet
            for(var i in data.params){
                params.push({
                    field: i,
                    value: data.params[i]
                });
            }
        }else{
            params.push({"field":data.searchField,"value":data.searchText});
        }
        //insert the brands user has access to only if not super user
        if(!(userRole=='super_administrator' && lock==true)) {
            params.push({"field": "brand._id", "value": USER.getAllowedIds('read')})
        }
        var fields;
        if(data.searchField){
            if(data.searchField != "ANY"){
                fields = [data.searchField];
            }
        }

        SolrCore.nameSpace  = data.searchType;
        SolrCore.collection = dataObject.collection;

        SolrCore.suggestAsyouType(data.searchText, params, fields)
        .then(function(resolve){
            cb(resolve);
        }).catch(function(e){
            console.log("solr error");
            console.log(e.message);
            cb({error: e.message});
        });


    }else{
        cb({error: 'autocomplete type not passed.'});
    }
};

/**
 *
 * get the columns for data type
 * dataType - data type to get columns for
 */
var getColumns = function(){
    if(data.dataType && data.dataType != ""){

        var dataObject = eval(data.dataType);
        if(!dataObject){
            cb({error: 'data type not found'});
        }

        var columns = dataObject.getSearchableColumns();
        cb({
            columns: columns
        });
    }else{
        cb({error: 'autocomplete type not passed.'});
    }
};


var selectAll = function(){
    var params={};
    if (data.params) {
        params = data.params;
    }

    if(data.type){
        eval(data.type).selectAll(params, data.pagination)
            .then(function(results){

                cb(results);
            });
    }else{
        cb({error: 'type not passed.'});
    }
};

var archiveData = function()
{

    var params={};
    if (data.params) {
        params = data.params;
    }

    if(data.type){
        eval(data.type).archive(data.archiveData)
            .then(function(results){
                __systemLog.create(USER.getID(),"View "+data.type,null,data.archiveData);
                cb(results);
            });
    }else{
        cb({error: 'type not passed.'});
    }

};


var checkPermission=function(params) {
    if (!USER.user().role) return cb({error:'There is no Role set for your user.'})
    var userRole=USER.user().role.replace(/ /g, "_").toLowerCase();
    //TODO refactor remove hard coded role name and use action permissions
    var allowedIds = USER.getAllowedIds('read');

    //overrideLock use for getting data when we pass account_id manually or we want to override the user default account
    if(userRole == 'super_administrator' && USER.user().account_unlock && !data.overrideLock){
        return params;
    }

    if (brandSecurityLevel.indexOf(data.type) >= 0) {
        if (params.brand_id) {
            if (!params["$and"]) {
                params["$and"] = [];
            }
            params["$and"].push({brand_id: {$in: allowedIds}});
        } else {
            params.brand_id = {$in: allowedIds};
        }
    }

    if (accountSecurityLevel.indexOf(data.type) >= 0) {

        if (params.account_id) {
            if (!params["$and"]) {
                params["$and"] = [];
            }
            //params["$and"].push({account_id: db.ObjectId(params.account_id.toString())});
            params.account_id=db.ObjectId(params.account_id.toString());
        } else {
            if ((userRole!== 'super_administrator') && data.type=='Brand') {
                params._id={$in:allowedIds}
            } else {
                params.account_id = db.ObjectId(USER.user().account_id.toString());
            }
        }
    }
    return params;
}

/**
 *
 * mongoexport --db dim3 --host pub.qa.mongo3.navigatorch.com --port 27017 -q "{scan_id:ObjectId('56cb79409d4e1af311d30d7f')}"
 * --fields name,whois.registrant_name,whois.registrant_email,whois.registrar,whois.dns,whois.ip,whois.expires_date
 * --out /home/parallels/Desktop/mo4.csv --collection domains --type=csv
 * export the query
 * fields : the fields you need to be on the export should be an array ex:["_id","name"]
 * path : path to save file on the server
 * type : collection to search on it
 * query : query to run on mongo
 * fileType : csv or json
 */
var exportQuery = function () {
    var downloadableCollection=['domains','Scan'];

    var exec = require('child_process');
    let fields=data.fields.join();
    let token=USER.getID()+Date.now();
    let query=data.query;
    let columnNames=data.columnNames;
    let type=data.type;
    let fileType=data.fileType;
    let mongodb=config.mongo.url;
    //cleanup db so mongoexport can connect
    if (downloadableCollection.indexOf(type) < 0) return cb('Permission Denied');
    let outPath=path.join(config.tmp_path,type,token);
    let dbUrl=mongodb.replace('mongodb://','').split(',').pop(0).split('/').shift(0).split(':').shift(0);

    //console.log("mongoexport --db "+config.mongo.db+" --host "+dbUrl+" --port "+config.mongo.port+" -q '"+
    //query + "' --fields "+fields + " --collection "+type + " --out "+ outPath+ " --type="+fileType);

    exec.exec("mongoexport --db "+config.mongo.db+" --host "+dbUrl+" --port "+config.mongo.port+" -q '"+
    query + "' --fields "+fields + " --collection "+type + " --out "+ outPath+ " --type="+fileType ,
    function(error, stdout, stderr) {

        exec.exec("sed -i.bak '1s/^.*$/" + columnNames + "/' " + outPath,
            function(error1, stdout1, stderr1) {
                cb({
                    fileUrl: config.base_url + "/file/"+type+'/'+token+'/'+data.filename,
                    message: 'Your file is ready.'
                });
            });
    });

};

switch(data.func){

    case 'archiveData':
        archiveData();
        break;
    case 'getData':
        getData();
        break;
    case 'getResult':
        getResult();
        break;
    case 'getFilters':
        getFilters();
        break;
    case 'getAutocomplete':
        getAutocomplete();
        break;
    case 'getColumns':
        getColumns();
        break;
    case 'selectAll':
        selectAll();
        break;
    case 'getDomainByScanID':
        getDomainByScanID();
        break;
    case 'export':
        exportQuery();
        break;

    default:
        cb({
            error: 'function not found'
        });
}
