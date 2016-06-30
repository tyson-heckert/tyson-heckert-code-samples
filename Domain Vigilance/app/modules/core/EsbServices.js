"use strict";
var config = require('../../config.js');
var _ = require('underscore');
var async = require('async');
var path = require('path');
var http = require('http');
var EsbLog = require(path.join(config.server_path,'db','classes','EsbLog'));
class EsbServices {

    static runScan(data, cb) {
        let _this = this;
        _this.sendRequest(data, {
            host: config.esb.public,
            port: config.esb.port,
            path: "/dim2/scan/run",
            method: "POST",
            headers: {
                "content-type": "application/json",
                "Cache-control": "no-cache"
            },
            //body: {"scan_id": data.scan_id,"callback":"Dim2NotifyScanHasFinished"}
            body: {"scan_id": data.scan_id}
        }, cb);
    };

    static sendRequest(data, options, cb) {
        let _this = this;
        //call esb
        _this.callESB(options, function (input, output) {
            EsbLog.create({
                input: input,
                output: output,
                'created_dt':new Date()
            });
            cb(input, output);
        });
    };

    static callESB(options, callback) {
        let processResponse = function (response) {
            let str = '';
            response.on('data', function (chunk) {
                str += chunk;
            });

            response.on('end', function () {
                try{
                    var json = JSON.parse(str);
                    callback(options, json);
                }catch(e){
                    console.log("ERROR: could not parse esb response ", str);
                    callback(options, {response: {status: 'fail'}});
                }

            })
        };
        let req = http.request(options, processResponse);
        if(options.method=="POST"){
            req.write(JSON.stringify(options.body));
        }
        req.end();
    };
}

module.exports = EsbServices;
