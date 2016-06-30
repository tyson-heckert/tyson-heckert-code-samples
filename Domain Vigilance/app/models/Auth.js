/**
 * Created by Iman Nassirian on 12/1/15.
 */
'use strict';

var Cookie=require('hapi-auth-cookie');
var path=require('path');
var async=require('async');
var User=require(path.join('..','db','classes','User'));
var mongoose = require('mongoose');
var _ = require('underscore');
var UserGroups=require(path.join('..','db','classes','UserGroups'));


class Auth  {
    constructor() {
        //this.userList = {};

        this.sockets = {}; // for storing our socket objects
        this.userData = {}; //for storing our users
    }
    /*
    isLogin(user) {
        if (this.userList[user]) return true;
        return false;
    }

    login(user){
        if (this.userList[user._id] && this.userList[user._id].setLogin) return this.userList[user];
        this.userList[user._id]={};
        this.userList[user._id].id=user._id;
        this.userList[user._id].name= user.name;
        this.userList[user._id].email=user.email;
        this.userList[user._id].group=user.group;
        this.userList[user._id].permission=user.permission;
        this.userList[user._id].setLogin=true;
    }

    logi//nWithSession(userId) {
        var _this=this;
        // if (!this.userList[userId]) //this.userList[userId]={};
        this.userList[us//erId].id=userId;
        console.log(this.userList)
        User.model.findById(userId,function(err,res) {
        if (err) throw err;
            if (res) _this.login(res.toObject());
        })

    }

    logout(){
        delete this.userLogin
    }



    setSocketId(userId,socket){
        if (typeof this.userList[userId].socketId == "undefined" || !Array.isArray(this.userList[userId].socketId || this.userList[userId].socketId.length <= 0 )) {
            this.userList[userId].socketId=[];
        }

        if(this.userList[userId].socketId.indexOf(socket) == -1){
            this.userList[userId].socketId.push(socket);
        }

    }



*/



    /**
     *
     * Set up current user on socket
     * also update user information if it already exists
     */
    loginOnSocket(socket, userId, onLogin){
        var socketId = socket.id;
        var _this = this;


        if(socketId && socketId != ''){

            User.model.findById( userId, function(err,res) { // get user data from the DB
                if (err) throw err;

                if (res){ // user found
                    res = res.toObject();

                    res.socketId = socket.id;
                    res.ip = socket.request.connection.remoteAddress;
                    delete res.password;

                    _this.userData[socketId] = res;
                    _this.sockets[socket.id] = socket;

                    onLogin(res);
                }else{ // user not found
                    onLogin(null);
                }
            });

        }else{
            onLogin(null);
        }
    };


    /**
     *
     * get the signed in user on this socket
     */
    getUserOnSocket(socket){

        if(socketId && socketId != ''){
            if(this.userData[socketId] && typeof this.userData[socketId] != "undefined"){
                return this.userData[socketId];
            }else{
                return null;
            }
        }else{
            return null;
        }
    };

    /**
     *
     * get socket by id
     */
     getSocketById(socketId){

        if(this.sockets[socketId]){
            return this.sockets[socketId];
        }else{
            return null;
        }
    };

    /**
     *
     * is there a validated user on this socket
     */
     isLoggedIn(socketId){

        if(socketId && socketId != ''){
            if(this.userData[socketId] && typeof this.userData[socketId] != "undefined"
               && this.userData[socketId]._id && this.userData[socketId]._id != ''){ // just checking if there's a valid id for now
                return true;
            }else{
                return false;
            }
        }else{
            return false;
        }
     };

    /**
     *
     * Signout
     */
     logoutSocket(socketId){

        if(socketId && socketId != ''){
            if(this.userData[socketId]){
                delete this.userData[socketId];
            }

            if(this.sockets[socketId]){
                delete this.sockets[socketId];
            }
        }

        return true;
     };

    /**
     *
     * signout of all user session everywhere
     */
     logoutUser(userId){

     };

    /**
     *
     * get connected users
     * note: contains all connected sockets on this server, not cleaned up either
     */
     getConnectedUsers(){
         return this.userData;
     };

    /**
     *
     * get total connected users
     * note: contains all connected sockets on this server, not cleaned up either
     */
    getTotalConnectedUsers(){
        var total = 0;
        for(var i in this.userData){
            total++;
        }
        return total;
    };




}

module.exports=Auth;