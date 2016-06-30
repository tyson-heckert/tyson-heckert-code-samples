/**
 *
 * user object
 */
'use strict';

var Cookie=require('hapi-auth-cookie');
var path=require('path');
var async=require('async');
var User=require(path.join('..','db','classes','User'));
var mongoose = require('mongoose');
var _ = require('lodash');
var UserGroups=require(path.join('..','db','classes','UserGroups'));
var Brands=require(path.join('..','db','classes','Brand'));
var deasync=require('deasync');


/**
 *
 * default user methods
 * Not sure where to put this
 */
var DimUser = function(res){

    this._userData = res;
    //this.permissionArray = {read:[],write:[],delete:[],edit:[]};
    this.permissionArray = null;

    this.getID = function(){
        if(this._userData._id){
            return new mongoose.ObjectId(this._userData._id.toString());
        }else{
            return null;
        }
    };



    this.getName = function(){
        return this._userData.name;
    };

    this.getEmail = function(){
        return this._userData.email;
    };

    this.getAccountID = function(){
        return this._userData.account_id;
    };


    /**
     *
     * for everything else
     * usage: var phone = USER.user().phone;
     */
    this.user = function(){
        var data = _.clone(this._userData);
        data.allPermissions = _.clone(this.permissionArray);

        return data;
    };

    /**
     *
     * we need to make a few permission functions here
     */
    this.hasAccess = function(objects, level,securityScope){
        if (Array.isArray(objects)) objects=objects[0];
        if(typeof Object.getPrototypeOf(objects).toObject !=='undefined') objects=objects.toObject();
        var scope=this.permissionArray[level];
        if (securityScope.toLowerCase() =='account') {
            if (objects.account_id == this.user().account_id ) return true;
        } else if (securityScope.toLowerCase() =='brand') {
            if (this.user().role.replace(/ /g, "_").toLowerCase() === 'super_administrator') return true;
            if (typeof objects.brand_id === "undefined" || !objects.brand_id) return false;
            if (scope.indexOf(objects.brand_id.toString()) >= 0) return true;
        }
        return false;
    };


    /**
     * return lists of brands user has permission base on permission level
     * @param level
     */
    this.getPermissions = function(level,getPerm) {
        var allPermission=[];
        var userPermissions=this.user().permission;
        var userGroups=this.user().group;
        async.parallel({
            up:function(callback) {
                for (var ws in userPermissions) {
                    if (typeof userPermissions[ws] !="undefined" &&
                        userPermissions[ws] &&
                        typeof userPermissions[ws][level] !="undefined" &&
                        userPermissions[ws][level] == true
                    ) {

                        if (allPermission.indexOf(ws) == -1) {
                            allPermission.push(ws);
                        }
                    }
                }
                callback(null,null);
            },
            ug:function(callback){
                UserGroups.model.find({_id:{$in:userGroups}},callback)
            }
        }, function(err,res){
            if (err) throw err;
            for (var g in res.ug) {
                var ugObj=res.ug[g].toObject();
                for (var ws in ugObj.ws) {
                    if (typeof ugObj.ws[ws] !="undefined" &&
                        ugObj.ws[ws] &&
                        typeof ugObj.ws[ws][level] !="undefined" &&
                        ugObj.ws[ws][level] == 1
                    ) {
                        if (allPermission.indexOf(ws) == -1) {
                            allPermission.push(ws);
                        }
                    }
                }
            }
            getPerm(allPermission)

        });
    };

    this.getPermissionSync=function(level) {
            var perms;
            this.getPermissions(level,function(getPerm) {
                perms=getPerm;
            });
        while(perms === undefined) {
            deasync.runLoopOnce();
        }
        return perms;

    };

    /**
     *
     * @param getPerm
     * @param reload - get new copy from DB
     */
    this.getAllPermission=function(getPerm, reload) {
        var _that=this;

        if(!reload && this.permissionArray){
            return _.clone(this.permissionArray);
        }
        if (this.user().role.replace(/ /g, "_").toLowerCase() =='account_administrator' || 1==1) {
            Brands.model.find({account_id:new mongoose.ObjectId(this.user().account_id.toString())},{_id:1,name:1},function(err,res) {
                    var permArray = [];
                    for (var i in res) {
                        permArray.push(res[i]._id.toString())
                    }
                    var permissions = {
                        write: permArray,
                        edit: permArray,
                        delete: permArray,
                        read: permArray
                    }
                    _that.permissionArray = permissions;
                    getPerm(permissions);
                }
            )
        } else {
            async.parallel({
                read: function (callback) {
                    _that.getPermissions('read', function (rc) {
                        callback(null, rc)
                    });
                },
                write: function (callback) {
                    _that.getPermissions('write', function (wc) {
                        callback(null, wc)
                    });
                },
                edit: function (callback) {
                    _that.getPermissions('edit', function (ec) {
                        callback(null, ec)
                    });
                },
                delete: function (callback) {
                    _that.getPermissions('delete', function (dc) {
                        callback(null, dc)
                    });
                }
            }, function (err, res) {
                _that.permissionArray = res;

                getPerm(res)
            })
        }
    };


    /*
     * get allowed brand ids
     */
    this.getAllowedIds = function(level){
        var brands=[];
        for (var i in this.permissionArray[level]) {
            brands.push(new mongoose.ObjectId(this.permissionArray[level][i].toString()));
        }
        return brands;
    };

};


module.exports = DimUser;