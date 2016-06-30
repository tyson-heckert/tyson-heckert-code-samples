"use strict";

var db = require('./db'),
    jsen = require('jsen'),
    fs=require('fs'),
    path=require('path'),
    _ = require('lodash'),
    //Promise = require('promise');
    JSONPath = require('jsonpath-plus'),
    config = require(path.join('..', '..', 'config.js'));


class Model {

    /**
     *
     * get data based on pagination parameters
     * params (object) - parameters (ex. {search_id: 1})
     * pagination - our pagination object
     *              per = -1 for no limit
     * select (array) - optional array columns to select (array)
     * count only - return only count
     */
    static getData(params, pagination, select, countOnly, aux) {
        console.log(params);
        console.log(pagination);
        console.log(select);

        var _this = this;

        if (!pagination) {
            pagination = {};
        }

        var schemaObj;
        //load it as part of module so that it can be chached, each object should own its schema\
        try {
            var schema = config.server_path + '/db/schema/' + this.modelName + '.json';
            schemaObj = JSON.parse(fs.readFileSync(schema, 'utf-8'));
            if(pagination.search && pagination.search.length>0){
                if(Array.isArray(pagination.search)){
                    pagination.search.forEach(function(element,index,array) {
                        var jsonpath =`$..properties[?(@.name=='${element.field}')]`;
                        var schema = JSONPath({'json': schemaObj, 'path': jsonpath});
                        if (schema != null && schema.length > 0) {
                            try {
                                // first element should be the match, infact the lookup should yeild only in 1 hit
                                element.value = _this.fieldSchema[schema[0].type](element.value);
                            } catch (e) {
                                // not all types require conversion, skip the ones that don't require
                            }
                        }
                    });
                }
            }
        }catch(e){
            //    //cases where schema may not exist
            console.log('Schema Object for Model doesnt exist',e);
        }

        var page = parseInt(pagination.page, 10);
        var per = parseInt(pagination.per, 10) || 10;
        var start = (page - 1) * per;
        var search = {};
        var sort = {};
        var i, j;
        var wildcard;
        var cleanValue;

        // additional params
        // note keep separate because we want to use indexes on these columns first
        //TODO Refactor
        if (params.scan_id) {
            params.scan_id = new db.ObjectId(params.scan_id.toString());
        }
        if (params) {
            search = _.cloneDeep(params, function(val){
                if (val instanceof db.ObjectId){
                    return new db.ObjectId(val.toString());
                }
                return val;
            });
        }

        search.$and = [];

        // filters
        if (pagination.filter && pagination.filter.length) {
            // check model for what the filter object looks like then add to search

            if (this.filters) {
                for (i in pagination.filter) {
                    if (this.filters[pagination.filter[i]]) { // if this is a valid filter on the model
                        search = this._addFilter(search, this.filters[pagination.filter[i]].filter);
                    }
                }
            }
        }

        // searches
        if (pagination.search && pagination.search.length) {
            pagination.search.map(function (row) {

                cleanValue = _this.escapeRegExp(row.value);
                // validate per model that we are allowed to search this field
                if (row.field && row.field != ""
                    && cleanValue!=null) {
                    if(!(typeof cleanValue=='string' && cleanValue=="")) {
                        if (row.field == "ANY") {
                            search.$and.push(_this._addWildCardSearch(cleanValue, row.boolean, aux));
                        }
                        else {
                            search = _this._addSearchField(search, row.field, cleanValue, false, row.boolean);
                        }
                    }
                }
            });
        }


        if (search.$and && search.$and.length == 0) { // remove if we don't have any items in here
            delete search.$and;
        }

        //Construct sort query string
        if (pagination.sort && pagination.sort.length) {
            pagination.sort.map(function (row) {
                // validate per model that we can sort by this field, ex. image
                if (row.field && row.direction) {
                    //add on the fly sorting fields for case insenstivity
                    //sort[row.field] = (row.direction === 'asc') ? 1 : -1;
                    _this._caseInSensitiveSort(schemaObj, row, sort);
                }
            });
        } else { // no sort specified, does the model have a default sort?

        }

        // create our mongoose objects
        var qc = this.model.find(search,{"_id":1}).count(); // for counts only "_id" select makes sense ..
        var q = this.model.find(search)
            .sort(sort)
            .skip(start);
        if (per > -1) {
            q.limit(per);
        }


        if (select) {
            q.select(select);
        }

        if (countOnly) {
            return qc.exec()
                .then(function (temp) {
                    return {
                        total: temp
                    }
                }, function (e) {
                console.log(e);
                    throw e;
                });
        } else {
            var start_count_dt = new Date();
            return qc.exec()
                .then(function (count) {
                        var end_count_dt = new Date();
                        console.log('Time For Execution Of Count',(end_count_dt-start_count_dt)/1000)
                        var start_dt = new Date();
//todo: make object for aux
                        return _this._executeAggregatePipleine(search, select, sort, start, per, _this, aux, pagination).then(
                                function (docs) {
                                    var end_dt = new Date();
                                    console.log('Aggregation Execution Time',(end_dt-start_dt)/1000)
                                    return {
                                    total: count,
                                    results: docs
                                }
                            });
                      /* return q.exec()
                                .then(function (results) {
                                   var end_dt = new Date();
                                   console.log('Aggregation Execution Time',(end_dt-start_dt)/1000)
                                    return {
                                        total: count,
                                        results: results
                                    };
                                }, function (e) {
                                    throw e;
                                });*/
                }, function (e) {
                    throw e;
                });
        }
    }

    static _executeAggregatePipleine(search,select,sort,start,per,callerCntx,aux,pagination) {
        var _this = callerCntx;

        var aggPipeline = _this._buildAggregationPipeline(search,select,sort,start,per,_this,aux,pagination);
        //console.log(" --> aggPipeline <--");
        console.log(JSON.stringify(aggPipeline));
        return new Promise(function(resolve,reject){
            _this.model.aggregate(aggPipeline).exec(function (err, docs) {
                return resolve(docs);
            });
        });
    }
    /**
     * helper function for adding search field to search array
     * only wildcard search for now
     *
     * mongo same field search
     * { <field>: { $all: [ <value1> , <value2> ... ] } }
     * { $and: [ { tags: "ssl" }, { tags: "security" } ] }
     *
     * mongo regex search
     * { <field>: { $regex: 'pattern', $options: '<options>' } }
     *
     */
    static _addSearchField(search, field, value, noregex, boolean){
        // only string types to undergo
        if(typeof value=='string') {
            if (noregex) {
                var regexVal = value;
            } else {

                if (boolean) { // apply boolean filters
                    if (boolean == "starts_with") {
                        value = '^' + value;
                    } else if (boolean == "ends_with") {
                        value = value + '$';
                    } else if (boolean == "does_not_contain") {
                        value = '^((?!' + value + ').)*$';
                    }
                }

                var regexVal = {
                    '$regex': value,
                    '$options': 'i'
                };
            }

            if (search[field]) { // if we already have this field, we need to add to the $and array instead
                search.$and.push({[field]: regexVal});
            } else {
                search[field] = regexVal;
            }
        }else{
            search[field]=value;
        }

        return search;
    };

    // stub to over-ride default sort behavior
    static _addDynamicSort(sort,$project){

        var dynamicTextSort =false;
        if(sort){
            let sortFields = Object.keys(sort);
            sortFields.forEach(function(sortField){
                let hits = sortField.match('lower_dynamic');
                if(hits && hits.length > 0){
                    dynamicTextSort=true;
                    // insert the  field into projection for lower case needs
                    let csFieldName = '$'+ sortField.substring(0,sortField.indexOf('_lower_dynamic'));
                    let csFiledObj={};
                    csFiledObj['$toLower']=csFieldName;
                    $project[sortField]=csFiledObj;
                }

            });
        }

    }
    static _buildExtendedProjection(select,sort,pipeline) {

        var $project = {};
       // determine if we have  dynamic chracter/string/text sorting
        this._addDynamicSort(sort,$project)

        // do the projection now for fields
        if(select && select.length >=1){

            let fields = select.split(' ');
            fields.forEach(function(field) {
                if(field.length > 0) {
                    $project[field] = 1;
                }
            });
        }

       if(Object.keys($project).length>=1){
           // add it to the pipline
           pipeline.push({"$project":$project});
       }


       if(sort && Object.keys(sort).length > 0){

            pipeline.push({'$sort':sort});
        }

   }
   // make a pass for each field
   static _caseInSensitiveSort(schemaObj,row,sort) {

       // try and get the field type from  schema and gracefully handle exception
       try {
           //let jsonPath ="$..properties[?(@.name==='"+row.field+"')]";
           //let jsonPath ="$..properties[?(@.type==='text'||@.type==='string'||@.type==='array' && @.name==='"+row.field+"')]";
           let jsonPath ="$..properties[?(@.name==='"+row.field+"')]";
           let sortFieldHits = JSONPath({'json': schemaObj, 'path': jsonPath});
           if (sortFieldHits != null && sortFieldHits.length > 0) {
               //match found, the first element check dynamic and index based sorting rules ..
               var filedMetaData = sortFieldHits[0];
               var sortType = filedMetaData["sortType"];
               // use inbuilt indexing on lower case fields
               if (sortType && sortType == 'indexTxtLowerCase') {

                   sort[row.field + '_lower'] = (row.direction === 'asc') ? 1 : -1;

               }
               else {
                   // the field is not already in lower case in the database
                   if(sortType!='native') {
                       sort[row.field + '_lower' + '_' + 'dynamic'] = (row.direction === 'asc') ? 1 : -1;
                   }
                   else{

                       sort[row.field] = (row.direction === 'asc') ? 1 : -1;
                   }
               }
               return;
           }

       }catch(e){ console.log('Json Path Field identification exception',e)}
       // no change in sort criteria, for models that don't have well defined schema
       sort[row.field] = (row.direction === 'asc') ? 1 : -1;
   }

    static _getDefaultPipeline(){

        return [];
    }
    static _buildAggregationPipeline(search,select,sort,start,per){

       let _this = this;
       let pipeline = _this._getDefaultPipeline(); //initialize an empty  array/pipeline  or custom pipeline
       // set the match condition
       let match = {};
       let fields;

       if(search){
           pipeline.push({'$match':search});
       }
       else{
           // get all documents
           pipeline.push({'$match':{}});
       }

       _this._buildExtendedProjection(select,sort,pipeline);

       // set the skip

       if(start&&start>=0){

           pipeline.push({'$skip':start});
       }

       // set the limit
       if(per && per>=0) {
           pipeline.push({'$limit': per});
       }

       //inject join

       return _this._join(pipeline);


   }
    // default implementation, respective models have to over-ride the default implementation
    static _join(pipeline){

        return pipeline;
    }


    static _addFilter(search, filter) {

        var field;
        var i;
        for (field in filter) { // add each filter to our params
            //unwind the fields first
            this._unwindFieldAndValue(filter[field])
            if(field.toLowerCase!=='$match'){
                if(Array.isArray(filter[field])) {
                    filter[field].forEach(function (element, index, array) {
                        search.$and.push(element)

                    });
                }else {
                    search.$and.push(filter[field])
                }
            }
            else{
                search = this._addSearchField(search, field['field'],field['value'], true);

            }
        }
        return search;
    }


    /**
     * get searchable columns
     */
    static getSearchableColumns(){
        var _this = this;

        if( _this.getColumnsInfo ){
            var columns = _this.getColumnsInfo();
        }else{
            return {};
        }

        var i;
        var searchableColumns = [];

        for(i in columns){
            if(columns[i].searchable == true){
                searchableColumns.push(columns[i]);
            }
        }



        return searchableColumns;
    };

    /**
     *
     * get filters of the model
     * only get specific group if specified
     */
    static getFilters(group){
        var filters = {};

        if(this.filters){
            filters = _.cloneDeep(this.filters, function(val){
                if (val instanceof db.ObjectId){
                    return new db.ObjectId(val.toString());
                }
                return val;
            });
            for(var j in filters){
                filters[j].key = j;
            }
        }

        if(!group || group == ""){ // add an all filter, not sure when to add this yet, maybe never
            filters.all = {
                name: 'All',
                key: 'all',
                default: 1,
                priority: -1,
                filter: {},
                group: 'all',
                multi: 0
            };
        }

        if(group && group != ""){ // only return the filters in the group
            for(var i in filters){
                if(!filters[i].group || filters[i].group != group){
                    delete filters[i];
                }
            }
        }

        return filters;
    };


    static all () {
        var _this = this;
        _this.query= _this.model.find({ $or: [ { deleted_at: { $exists: false } }, { deleted_at: null } ] });
        return _this.query.exec();
    };

    static byKey (key, value, requiresObjectId) {
        if(requiresObjectId){
            value = new db.ObjectId(value)
        }

        this.query=this.model.where(key, value).limit(50);

        return this.query.exec();
    }

    static byParamsPaged(findBy, sortBy, start, take, select){
        var _this = this;

        if(findBy.scan_id){
            findBy.scan_id = new db.ObjectId(findBy.scan_id);
        }


        _this.query = _this.model.find(findBy).sort(sortBy).skip(start).limit(take);

        if(select){
            _this.query.select(select);
        }

        return _this.query.exec();
    }

    static byId(id){

        return this.byKey( '_id', id,true)
    }

    static archive (data) {
        var _this = this;

        /*
        return new Promise(function (resolve, reject) {
            data.deleted_at = new Date();
            _this.updateById( _this.string2ObjectKeys(data), function (err, res){
                console.log(err);
                console.log(res);

                if (err) reject(err);
                else resolve(res);
            });
        });
        */

        data.deleted_at = new Date();
        data.updated_at = new Date();

        return _this.updateById( _this.string2ObjectKeys(data), function (err, res){
            if (err) reject(err);
            else resolve(res);
        });

    }

    static remove(data){
        try{
            var _this = this;

            var value = new db.ObjectId(data._id);

            return _this.model.remove({'_id': value});
        }
        catch (e){
            console.log(e.message);
            console.log(e.details);

        }
    }

    static save (data,options) {
        let _this = this;

        data.updated_at = new Date();

        // multi doc persistance no key is passed, key is generated by  mongo for each doc in the array
        if(Array.isArray(data)){

            data.forEach(function(element,index,array){
                _this.string2ObjectKeys(element)
            })
            // save the array of objects
            console.log('about to call create')
            try {
                //console.log(this)
                return _this.create(data, options);
            }catch(e){console.log(e)}
        }
        // key presence
        if(data._id){

            return _this.updateById( _this.string2ObjectKeys(data))
        }
        else { // update the doc
            //console.log("created " + data);
            return _this.create(_this.string2ObjectKeys(data), options);

        }
    }

    static updateWithUpsert (query, obj) {
        let _this = this;
        try {
            return _this.model.update(query, obj, {upsert: true});
        } catch (e) {
            console.log(e.message);
        }
    }

    static count ( params) {
        this.query=this.model.count(this.string2ObjectKeys(params));
        return this.query.exec();
    }

    static validate(params){
        try {
            let schemaJson=path.join(__dirname,'..','schema',this.modelName+'.json');
            if (!this.schema && !fs.existsSync(schemaJson)) throw new Error('Schema not Found!');
            if (!this.schema) this.schema=JSON.parse(fs.readFileSync(schemaJson,'utf-8'));
            let validate = jsen(this.schema);
            if (!validate(params)) {
                let error=new Error();
                error.name='Validation Error';
                error.message='validation failed';
                error.details=validate.errors;
                throw error;
            }
        } catch (e) {
            throw e
        }
    }
    static string2ObjectKeys(obj) {

        if (this.hasOwnProperty("keys") &&Array.isArray(this.keys)) {
            this.keys.forEach(function(element, index, array) {
                {

                    if (obj.hasOwnProperty(element)) {
                        //default key
                        if(element==="_id" && obj[element]===undefined){

                            obj[element] = new db.ObjectId()

                        }
                        else {
                            if(!(obj[element]==undefined) && typeof obj[element] ==="string") {
                                obj[element] = new db.ObjectId(obj[element])
                            }
                        }
                    }
                }
            })
        }
        return obj

    }


}

// generic template that will map fields to their types defined in JSON schema definitions
//can be exapnded to other types, though javascript does automatic type conversion, while passing to mongoose driver type conversion | match
// to the bson type is not done automatically, the caller has to pass the exact type
Model.fieldSchema = {

    'integer':function(val){

        return Number(val);
    }

}

module.exports=Model;

