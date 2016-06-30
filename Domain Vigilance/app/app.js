'use strict'
var
    path = require('path')
    , config = require('./config.js')
    , fs = require('fs')
    , Hapi = require('hapi')
    , Inert = require('inert')
    , util = require('util')
    , uuid = require('uuid')
    ,statehood = require('statehood')
    ,Cookie=require('hapi-auth-cookie')
    ,Auth=require(path.join(config.server_path,'models','Auth'))
    , $ = require('./svr/iwa').$
    ,bcrypt = require('bcrypt')
    //, getRes= require('./modules/search/getRes')($) // iman comment it davin please check it.
//, pth = 'R:\\apps\\ndl\\angular-seed\\app\\'
    , pth = config.public_path
    , pthsrc = config.server_path
    , server
    , io
    , redis = require('socket.io-redis')
    , git = require('git-rev')
    , _ = require('underscore')
    , __sysLog=require(path.join(config.server_path,'models','SystemLog'));

var socketAuth = new statehood.Definitions({
    encoding: 'iron',
    password: config.cookie_key
});
var DimUser = require(path.join(config.server_path,'models','DimUser'));
var ActionLog = require(path.join(config.server_path,'db','classes','ActionLog'));

var Auth = new Auth();

var exec = require('child_process').exec;

//add timestamps in front of console log messages
require( "console-stamp" )( console, {
    pattern : "dd/mm/yyyy HH:MM:ss",
    metadata: function () {
        return ("[" + process.memoryUsage().rss + "]");
    }
});

console.log('*** public html path: ' + pth);
console.log('*** server nodejs path: ' + pthsrc);

var gitCommit = '';
git.short(function (str) {
    gitCommit = str;
    console.log("*** git commit: "+str);
});


setErr();

//process.on('uncaughtException', function(err)
//{
//    console.log('****** global exception handler is envoked ******');
//    console.log(err);
//    console.log('****** end of global exception handler trace ******');
//});

//["log", "warn", "error"].forEach(function(method) {
["log", "error"].forEach(function(method) {
    var oldMethod = console[method].bind(console);
    console[method] = function()
    {
        //var args = [];
        //
        //for (var i = 0; i < arguments.length; i++)
        //{
        //    var el = arguments[i];
        //
        //    if (i == 0)
        //    {
        //        el = (new Date()).toISOString() + ' ' + el;
        //    }
        //
        //    args.push(el);
        //}

        oldMethod.apply(
            console,
            //args
            arguments
        );

        if (io && io.sockets)
        {
            //var txt = util.format(arguments[0], arguments[1] || '', arguments[2] || '', arguments[3] || '', arguments[4] || '');
            var txt = util.format.apply(util, arguments);

            //console.warn('************* args: ', args);
            //console.warn('************* broadcasting: ' + txt);

            io.sockets.emit('cnsl', { msg: txt });
        }
    };
});

server = new Hapi.Server();
var tls = false;

if (fs.existsSync('/etc/ssl/wildcard.corsearchdbs.com/wildcard.corsearchdbs.com.key') &&
    fs.existsSync('/etc/ssl/wildcard.corsearchdbs.com/STAR_corsearchdbs_com.crt') &&
    fs.existsSync('/etc/ssl/wildcard.corsearchdbs.com/COMODORSAAddTrustCA.crt') &&
    fs.existsSync('/etc/ssl/wildcard.corsearchdbs.com/COMODORSADomainValidationSecureServerCA.crt') ) {

        tls = {
            key: fs.readFileSync('/etc/ssl/wildcard.corsearchdbs.com/wildcard.corsearchdbs.com.key'),
            cert: fs.readFileSync('/etc/ssl/wildcard.corsearchdbs.com/STAR_corsearchdbs_com.crt'),
            ca: [
                fs.readFileSync('/etc/ssl/wildcard.corsearchdbs.com/COMODORSAAddTrustCA.crt'),
                fs.readFileSync('/etc/ssl/wildcard.corsearchdbs.com/COMODORSADomainValidationSecureServerCA.crt')
            ]
        };
}


server.connection({
    port: config.server.port,
    labels: ['socket'],
    tls: tls
});
server.connection({
    port: config.server.webport,
    labels: ['web'],
    tls: tls
});

server.register(Inert, function () {}); // for serving directories with hapi
server.register(Cookie, function (err) {
    if (err) throw err;
    server.auth.strategy('session', 'cookie', {
        password: config.cookie_key,
        cookie: 'session-data',
        redirectTo: config.corsearch_url,
        isSecure: false
    });
});

/**
 *
 * User login from HAPI
 * Set our session cookie here
 */
var login = function (req, res) {

    var User=require(path.join(config.server_path,'db','classes','User'));

    if (req.auth.isAuthenticated) {
        return res.redirect(getLoggedInURL(req.info.host));
    }else if (req.method === 'post') {
        console.log("LOGIN");
        if (!req.payload.password || !req.payload.email) {
            return res.redirect('login');
        }
        //User.model.findOne({email: req.payload.email}, function (err, user) {
        User.model.findOne({email: {$regex: new RegExp(req.payload.email, "i")}}, function (err, user) {
            var password=req.payload.password;
            if (err) throw err;
            if (!user) return res.redirect('login');
            var userObj=user.toObject();
            if (bcrypt.compareSync(password, userObj.password)) {
                //auth.login(userObj);
                req.auth.session.set({
                    _id: user._id,
                    //id:user._id,
                    //name:userObj.name,
                    //email:userObj.email,
                    //user:userObj.email,
                    secret:bcrypt.hashSync(userObj._id+userObj.password, bcrypt.genSaltSync(10))});

                return res.redirect(getLoggedInURL(req.info.host, userObj));
            } else {
                return res.redirect('login');
            }
        });
    }else{
        return res.file(path.join(config.login_path,'index.html'));
    }
};

/**
 *
 * User SSO login from HAPI
 * Set our session cookie here
 */
var ssoLogin = function (req, res) {
    var Sso=require(path.join(config.server_path,'db','classes','Sso'));
    var User=require(path.join(config.server_path,'db','classes','User'));
    var currentdate = new Date();
    var time_limit = 30 * 60 * 1000; /* ms */



    Sso.bySsoToken(req.params.token).then(function (sso_res) {
        ActionLog.log(null, null, 'app.js', {
            func: 'ssoLogin',
            sso_res: sso_res,
            request: req.params
        });

        if (sso_res.length != 0) {
            var sso = sso_res[0].toObject();
            if(sso.created_at >= currentdate - time_limit){
                User.byContactId(sso.contact_id).then(function (user_res){
                    if(user_res.length != 0){
                        var user = user_res[0].toObject();
                        //login and redirect user
                        req.auth.session.set({
                            _id: user._id,
                            sso: true,
                            sso_key: req.params.token,
                            secret:bcrypt.hashSync(user._id+user.password, bcrypt.genSaltSync(10))});
                        return res.redirect(getLoggedInURL(req.info.host, user));
                    } else {
                        return res.redirect(config.sso_environment+'/call3.php?key='+req.params.token+'&gotolink=timeout');
                    }
                });
            } else {
                return res.redirect(config.sso_environment+'/call3.php?key='+req.params.token+'&gotolink=timeout');
            }
        } else {
            return res.redirect(config.sso_environment+'/call3.php?key='+req.params.token+'&gotolink=timeout');
        }
    }, function (rej) {
        return res.redirect(config.sso_environment+'/call3.php?key='+req.params.token+'&gotolink=timeout');
    });
};

/**
 * temporary function for redirecting user to correct app url because of browser sync
 */
var getLoggedInURL = function(host, user){
console.log("logged in");

    var getStarted = '';
    if(typeof user !== 'undefined' && user.user_welcomed === false){
        getStarted = '#/get-started';
    }

    if(host.indexOf('.com') > -1){
        return '/'+getStarted;
    }else{
        /** add these two lines to work with port 80 on local**/
        if (host.indexOf(':') < 0 && host.indexOf('localhost') > -1) {
            return 'http://'+host.replace('localhost', 'localhost:3000')+getStarted;
        }
        return 'http://'+host.replace(':3011', ':3000')+getStarted;
    }
};

server.route({
    method: 'GET',
    path:'/public_assets/{param*}',
    handler: {
        directory: {
            path: config.public_path+'/../src/assets/public/',
            listing: true
        }
    }
});

server.route({
    path: '/file/{type}/{token}/{filename}',
    method: 'GET',
    config: {
        handler: function (req, res) {
            let typeMapper={'domains':'domains','report':'report'};
            let type=typeMapper[req.params.type];
            let token=req.params.token;
            let filename=req.params.filename;
            let file = path.join(config.tmp_path,type,token);
            res.file(file,{
                filename:filename
            })
        }
    }
});
server.route({
    method: 'GET',
    path: '/logout',
    config: {
        handler: function (req, res) {
            req.auth.session.clear();
            return res.redirect('login');
        }
    }
});
server.route({
    method: 'GET',
    path: '/ssoLogout',
    config: {
        handler: function (req, res) {
            if (req.state['session-data'].sso_key) {
                let sso_key = req.state['session-data'].sso_key;
                req.auth.session.clear();
                return res.redirect(config.sso_timeout+'&key='+sso_key);
            } else {
                return res.redirect(config.corsearch_url);
            }
        }
    }
});
server.route({
    method: ['GET', 'POST'],
    path: '/login',
    config: {
        handler: login,
        auth: {
            mode: 'try',
            strategy: 'session'
        },
        plugins: {
            'hapi-auth-cookie': {
                redirectTo: false
            }
        }
    }
});

server.route({
    method: ['GET'],
    path: '/ssoLogin/{token}',
    config: {
        handler: ssoLogin,
        auth: {
            mode: 'try',
            strategy: 'session'
        },
        plugins: {
            'hapi-auth-cookie': {
                redirectTo: false
            }
        }
    }
});


server.route({
    method: 'GET',
    path:'/socket.io/{param*}',
    handler: {
        directory: {
            path: 'node_modules/socket.io',
            listing: true
        }
    }

});

// adding this because i am too lazy to replace the images being used in the theme
server.route({
    method: 'GET',
    path:'/images/{param*}',
    handler: {
        directory: {
            path: config.public_path+'/assets/images/',
            listing: true
        }
    }
});

server.route({
    method: 'GET',
    path:'/css/{param*}',
    handler: {
        directory: {
            path: config.public_path+'/assets/css/',
            listing: true
        }
    }
});

/**
 * health check for testing if db is connected
 */
server.route({
    method: 'GET',
    path:'/status',
    handler: function(request, reply) {

        if($.db.mongoose.connection.readyState == 1){
            var healthCheck = $.db.mongoose.model('healthCheck', $.db.ObjectsSchema, 'health_check_log');
            healthCheck.collection.insertOne({
                time: new Date(),
                ip: request.info.remoteAddress
            }, function(err, res){

                var data = {
                    error: err,
                    readyState: $.db.mongoose.connection.readyState,
                    commit: gitCommit
                };


                reply(data);
            });
        }else{
            reply({
                error: "mongo is not connected",
                readyState: $.db.mongoose.connection.readyState,
                commit: gitCommit
            });
        }
    }
});

server.route({
    method: 'GET',
    path:'/login_assets/{param*}',
    handler: {
        directory: {
            path: config.login_path,
            listing: true
        }
    }
});

// 2016-01-04 - Log serving
server.route({
    method: 'GET',
    path: '/log/{param*}',
    handler: {
        directory: {
            path: path.join(__dirname, '..' ,'log'),
            listing: true
        }
    }
});

// temp  diagnostic report route for debug
server.route({
    method: 'GET',
    path: '/storage/{param*}',
    handler: {
        directory: {
            path: config.tmp_path,
            listing: true
        }
    }
});
//mongo db log output routes
/*
server.route({
    method: 'GET',
    path: '/mongolog1',
    config: {
        handler: function (req, reply) {
            var command = 'sudo docker exec mongodb1 tail -500 /var/log/mongodb.log';
            exec(command, function(error, stdout, stderr){
                console.log(stdout);
                reply('<pre>'+stdout+'</pre>');
            });
        }
    }
});
*/
//favicon
server.route(
    { method: 'GET',
        path: '/favicon.ico',
        handler: function(request, reply) {
            reply.file(config.public_path+'/../src/assets/favicons/favicon.ico').code(200).type('image/x-icon');
        }
    });

server.route(
    { method: 'GET',
        path: '/assets/favicons/manifest.json',
        handler: function(request, reply) {
            reply.file(config.public_path+'/../src/assets/favicons/manifest.json');
        }
    });

server.route({
    method: 'GET',
    path:'/{param*}',
    config: { auth: 'session' },
    handler: {
        directory: {
            path: config.public_path,
            listing: true
        }
    }
});

/**
 *
 * for our API calls
 * this is through HAPI port and domain, not gulp. make sure to use right port
 * for prod, after gulp build, the ports should be the same as everything will be served through hapi
 * can't apply max payload options to GET routes
 * ROUTING
 * localhost:3011/api/MODULE/SCRIPT
 * MODULE = the module directory name (search, core, etc)
 * SCRIPT = the file name (if you need to go into directories use a "-". ex. view-controller will look for file view/controller.js)
 */
server.route({
    method: ['POST'], // we need to add auth to this route
    path: '/api/{apiRoute*}',
    config:{
        payload:{
            maxBytes: 8388608
        }
    },
    handler: function(request, reply) {
        var apiRoute = request.params.apiRoute;

        if(!apiRoute || apiRoute == ""){
            return reply({error: "No params set"});
        }

        var apiParams = apiRoute.split('/');

        var apiModule = apiParams[0];
        var apiScript = apiParams[1];
        if(apiScript){
            apiScript = apiScript.replace('-', path.sep);
        }
        var scriptPath = path.join(apiModule, apiScript+'.js');
        scriptPath.replace('../', ''); // not sure if we need this

        var data = {};

        console.log("--------- api call received "+apiRoute);
        $.pushLog('messageApi', 'api call received '+ apiRoute);

        if(!apiModule || apiModule == ''){
            return reply({error: "No module set"});
        }else if(!apiScript || apiScript == ''){
            return reply({error: "No script set"});
        }


        apiParams.splice(0,2); // fix params so we don't have script or module in it
        data.api = {
            remoteAddress: request.info.remoteAddress,
            route: apiRoute,
            params: apiParams,
            module: apiModule,
            script: apiScript,
            path: scriptPath
        };

        data.payload = request.payload;

        var CURRENT_USER = { // create a fake user for now for all API requests
            name: 'API REQUEST',
            ip: request.info.remoteAddress
        };



        try {
            var doRequest = new $.processHapiRequest(data, scriptPath, reply, CURRENT_USER);
        } catch (error) {
            $.pushLog('errorApi', 'api error', error.message);
            console.log("api error");
            console.log(error);
            return reply(error.message).code(400);
        }
    }
});
server.route({
    method: ['GET'], // we need to add auth to this route
    path: '/api/{apiRoute*}',
    handler: function(request, reply) {
        var apiRoute = request.params.apiRoute;

        if(!apiRoute || apiRoute == ""){
            return reply({error: "No params set"});
        }

        var apiParams = apiRoute.split('/');

        var apiModule = apiParams[0];
        var apiScript = apiParams[1];
        if(apiScript){
            apiScript = apiScript.replace('-', path.sep);
        }
        var scriptPath = path.join(apiModule, apiScript+'.js');
        scriptPath.replace('../', ''); // not sure if we need this

        var data = {};

        console.log("--------- api call received "+apiRoute);
        $.pushLog('messageApi', 'api call received '+ apiRoute);

        if(!apiModule || apiModule == ''){
            return reply({error: "No module set"});
        }else if(!apiScript || apiScript == ''){
            return reply({error: "No script set"});
        }


        apiParams.splice(0,2); // fix params so we don't have script or module in it
        data.api = {
            route: apiRoute,
            params: apiParams,
            module: apiModule,
            script: apiScript,
            path: scriptPath
        };

        data.payload = request.payload;

        var CURRENT_USER = { // create a fake user for now for all API requests
            name: 'API REQUEST',
            ip: request.info.remoteAddress
        };



        try {
            var doRequest = new $.processHapiRequest(data, scriptPath, reply, CURRENT_USER);
        } catch (error) {
            $.pushLog('errorApi', 'api error', error.message);
            console.log("api error");
            console.log(error);
            return reply(error.message).code(400);
        }
    }
});




io = require('socket.io')(server.select('socket').listener);
if(config.redis && config.redis.host){
    console.log("*** Redis connected on "+config.redis.host+":"+config.redis.port+" ***");
    io.adapter(redis({ host: config.redis.host, port: config.redis.port, key: 'dv:'+config.environment}));
}else{
    console.log("*** ERROR - No Redis information found. Sockets not setup ***");
}


$.processAPIRequest = function(data, name, cb)
{
    var params = {};
    var pth;
    var lnm = path.join(config.server_path, 'modules', name);

    console.log("process api request "+lnm);
    $.pushLog('messageApi', 'process api request: '+ lnm, data);

    if (fs.existsSync(lnm))
    {
        var _this = this;
        var code = '' + fs.readFileSync(lnm);

        _this.ev = eval;
        _this.data = data;
        _this.$ = $;
        _this.cb = function()
        {
            console.log('*********** got cb');
        };
    }else{
        $.pushLog('errorApi', 'Script not found '+ lnm);
        cb({error: "Script not found"});
    }

    process.nextTick( function()
    {
        try
        {
            eval(code, _this);
        }
        catch(eee)
        {
            $.pushLog('error', 'api error: '+ eee.message);
            cb({error: "Exception: "+eee.message});
        }
    });
};



$.processHapiRequest = function(data, name, cb, USER)
{
    var params = {};
    var pth;
    var lnm = path.join(config.server_path, 'modules', name);

    $.pushLog('messageApi', 'process hapi api request: '+ lnm, data);

    if (fs.existsSync(lnm))
    {
        var _this = this;
        var code = '' + fs.readFileSync(lnm);

        _this.ev = eval;
        _this.data = data;
        _this.$ = $;
        _this.cb = function()
        {
            console.log('*********** got cb');
        };
    }else{
        $.pushLog('errorApi', 'Script not found '+ lnm);
        console.log("script not found "+lnm);
        cb({error: "Script not found"});
    }

    process.nextTick( function()
    {
        try
        {
            console.log("-------------------------- API request - "+name+" --------------------------");
            console.log(data);

            eval(code, _this);

            console.log("----------------------------- complete -----------------------------");
        }
        catch(eee)
        {
            console.log("----------------------------- ERROR -----------------------------");
            console.log(eee);
            $.pushLog('error', 'api error: '+ eee.message);
            cb({error: "Exception: "+eee.message});
        }
    });
};


$.processLocal = function(socket, data, cb)
{
    var _this = {};

    _this.ev = eval;
    _this.$ = $;
    _this.cb = cb;
    _this.socket = socket;

    setTimeout( function()
    {
        try
        {
            eval(data.__code, _this);
        }
        catch(eee)
        {
            if (cb)
            {
                cb(null);
            }
        }
    }, 12);
}

$.processRemote = function(socket, data, cb , USER)
{
    var _this = {};
    var nm = data.__name;
    //var rt = (data.__approot) ? data.__approot + '/' : 'svr/';
    var rt = "";
    var url = (nm) ? 'http://ecatdev.yury.com/' + rt + '/resources/svrjs/' + nm + '?tmp=' + uuid.v4() : null;
    var pth;
    var lnm = path.join(config.server_path, 'modules', nm);

    var exc = function(data, body, cb)
    {
        if (data.__module)
        {
            var pth = path.join(__dirname, 'svrjs');

            if (fs.existsSync(pth) == false)
            {
                fs.mkdirSync(pth);
            }

            var fn = path.join(pth, uuid.v4() + '.js');

            //$.log('js file', fn);

            fs.writeFileSync(fn, body);

            var mod = require(fn);

            mod($, cb);
        }
        else
        {
            fnc(body, cb);
        }
    }

    var fnc = function(body, cb)
    {
        _this.ev = eval;
        _this.$ = $;
        _this.cb = cb;
        _this.socket = socket;

        process.nextTick( function()
        {
            try
            {
                console.log("\n\n-------------------------- "+nm+" --------------------------");
                console.log(data);

                eval(body, _this);

                console.log("----------------------------- complete -----------------------------");
            }
            catch(eee)
            {
                console.log("----------------------------- ERROR -----------------------------");
                console.log(eee);
                $.pushLog('error', 'error '+ eee.message);

                if (cb)
                {
                    cb({error: eee.message});
                }
            }
        });
    }



    if (url == null)
    {
        console.log("code exec");
        var code = data.__code;

        fnc(code, cb);
    }
    else if (fs.existsSync(lnm))
    {

        // WE ARE ONLY  USING THIS SECTION
        //console.log('local call from file: %s', lnm);
        $.pushLog('message', 'socket call from file: '+ lnm, data);

        //var body = '' + fs.readFileSync(lnm);
        //exc(data, body, cb);

        fs.readFile(lnm, function(err, body){
            body = ''+body;
            exc(data, body, cb);
        });

    }
    else
    {
        console.log('remote call url: %s', url);


        console.log("file not found: %s", lnm);
        cb({error: 'file not found'});
        return; // we are not using this



        request(url, function (error, response, body)
        {
            if (!error && response.statusCode == 200)
            {
                exc(data, body, cb);
            }
        })
    }
}

$.io = io;

io.on('connection', function (socket)
{
    /**
     *
     * send environment to browser
     */
     if(config.environment == 'dev'){
         $.push('core.environment', {environment: config.environment, debug: true}, socket, null, true); // send message to client
     }

     $.push('core.version', {commit: gitCommit}, socket, null, true); // send message to client


    /**
     *
     * EVT socket connection
     */
     if(socket.handshake.query.token){
        console.log(" --------------- server socket connection request "+socket.request.connection.remoteAddress);

        if(socket.handshake.query.token == config.socketToken){
            console.log(" --------------- connected");

            // push events
            socket.on("EVT.push", function(event, cb){
                //console.log(" --------------- push event ("+event.event+") received from "+socket.request.connection.remoteAddress);
                $.push(event.event, event.data, event.socket, event.room, event.toSocket);
            });

            // broadcast events
            socket.on("EVT.broadcast", function(event, cb){
                //console.log(" --------------- broadcast event ("+event.name+") received from "+socket.request.connection.remoteAddress);
                io.emit(event.name, event.data);
            });


        }else{
            console.log(" --------------- invalid socket token disconnecting "+socket.request.connection.remoteAddress);
            socket.disconnect();
        }

        return;
     }



    socketAuth.parse(socket.request.headers.cookie, function(err, sessionData, failed) {

        var sessionInfo=sessionData['session-data'];

        if ((typeof sessionInfo) == "undefined" || !sessionInfo) { // user is not signed in

            console.log("INVALID LOGIN");
            $.push('core.login.invalid', {}, socket, null, true); // send message to client

        }else{ // user is signed in

            /**
             *
             * get user information from DB
             * NOTE: we need to update this for all connected sockets if user info is changed
             */
            Auth.loginOnSocket(socket, sessionInfo._id, function(res){

                if(!res){ // user was not found

                    console.log("USER NOT FOUND");
                    $.push('core.login.notfound', {}, socket, null, true); // send message to client

                }else{ // successful login

                    var USER = new DimUser(res); // create our user object here

                    USER.getAllPermission(function(data){
                        let UserObject=USER.user();
                        UserObject.sso= {
                            active: sessionInfo.sso,
                            sso_inbox: config.sso_inbox+'&key='+sessionInfo.sso_key
                        };

                        $.push('core.login.success', UserObject, socket, null, true); // send user information to client
                        console.log("connect: " + socket.id + ': '+socket.request.connection.remoteAddress + ' as '+USER.getName());

                        if(config.environment == 'dev'){
                            $.push('davintest.connected', {total: Auth.getTotalConnectedUsers(), clients: Auth.userData });
                        }
                    });

                    socket.on('j57xyz', function(data, cb)
                    {

                        var actionLogStartTime = new Date();

                        ActionLog.log(USER, socket, data.__name, data, function(actionLogId){

                            $.processRemote(socket, data, function(){

                                cb.apply(this, arguments);

                                ActionLog.saveData(actionLogId, actionLogStartTime, arguments);
                            }, USER);
                        });
                    });


                    socket.on('j57xy', function(data, cb)
                    {
                        $.processLocal(socket, data, cb);
                    });


                    socket.on('disconnect', function () {

                        console.log('<<< client is disconnected');

                        Auth.logoutSocket(socket.id);

                        if(config.environment == 'dev'){
                            $.push('davintest.connected', {total: Auth.getTotalConnectedUsers(), clients: Auth.userData });
                        }
                    });






                    if(config.environment == 'dev'){
                        socket.on('davintest.getConnected', function(data, cb){
                            cb({total: Auth.getTotalConnectedUsers(), clients: Auth.userData });
                        });

                        socket.on('davintest.sendMessage', function(data, cb){
                            if(data.to && data.to._id){
                                var toSocket = Auth.getSocketById(data.to.socketId);
                                if(toSocket){
                                    $.push('davintest.message', {from: USER.getName(), message: data.message}, toSocket, null, true);
                                    cb({message: 'message sent'});
                                }else{
                                    cb({error: 'user not online'});
                                }
                            }else{
                                cb({error: 'no user selected'});
                            }
                        });

                        socket.on('davintest.sendMessageAll', function(data, cb){
                            $.pushLog('chat', (USER.getName() || socket.id) + ': '+ data.message, {ip: socket.request.connection.remoteAddress, client: USER});
                        });
                    }
                }

            });

        }

    });
});






/**
 *
 * push data to the client
 * event - the name of the event
 * data - the data associated with the event
 * socket - optional, the socket of the user to exclude from the event, usually the sender. use null to send to all.
 * room - optional, who to send the data to.
 * toSocket - bool, send to this user only

 *
 * note: capture on client using
 * utilsService.rootScope.$on('push.'+event, function(data){});
 */
$.push = function(event, data, socket, room, toSocket){
    if(event && event != ""){
        var params = {};
        params.event = event;
        params.data = data;

        //console.log(params);

        if(socket){
            if(toSocket){ // send only to this socket
                socket.emit('push', params);
            }else if(room && room != ""){ // send to all clients in room, excluding sender
                socket.broadcast.to(room).emit('push', params);
            }else{ // send to everyone excluding the sender
                socket.broadcast.emit('push', params);
            }
        }else{
            if(room && room != ""){ // send to all clients in room
                io.to(room).emit('push', params);
            }else{ // send to all clients on server
                io.emit('push', params);
            }
        }
    }
};


/** notes for davin
  // sending to sender-client only
 socket.emit('message', "this is a test");

 // sending to all clients, include sender
 io.emit('message', "this is a test");

 // sending to all clients except sender
 socket.broadcast.emit('message', "this is a test");

 // sending to all clients in 'game' room(channel) except sender
 socket.broadcast.to('game').emit('message', 'nice game');

 // sending to all clients in 'game' room(channel), include sender
 io.in('game').emit('message', 'cool game');

 // sending to sender client, only if they are in 'game' room(channel)
 socket.to('game').emit('message', 'enjoy the game');

 // sending to all clients in namespace 'myNamespace', include sender
 io.of('myNamespace').emit('message', 'gg');

 // sending to individual socketid
 socket.broadcast.to(socketid).emit('message', 'for your eyes only');
 */


/**
 *
 * temporary function for sending the console messages to the browser
 * use the push function above
 * i'm only using this so we don't get a lot of console logs from the other push
 */
 $.pushLog = function(type, message, data, CURRENT_USER){
    if(config.environment == 'dev'){
        var pushData = {};
        if(data){
            pushData = _.clone(data);
            if(CURRENT_USER){
                pushData.CURRENT_USER = CURRENT_USER;
            }
        }

        io.emit('server.log', {type: type, message: message, data: data});

    }
 };








server.start(function ()
{
    console.log('*** node server running at:', server.select('socket').info.uri);
    console.log('*** web server running at:', server.select('web').info.uri);
});

function setErr()
{
    process.on('uncaughtException', function (err)
    {
        console.log('*** Uncaught exception: ' + err, true);

        try
        {
            if ($)
            {
                $.err('*** Uncaught exception', err, true);

                if (err.stack)
                {
                    $.err('*** Stacktrace:', err.stack, true);
                }
            }
        }
        catch (e)
        {}
    });

    //var uid2 = uuid.v1();
    //var uid4 = uuid.v4();
    //console.log('uid4: ' + uid4 + '\nuid2: ' + uid2);
}

var setInvoke = function()
{
    var doInvoke = function(nm, data, cb)
    {
        var _this = this;
        var db = $.db.db;
        var Obj = db.model('system.js', $.db.ObjectsSchema);
        var crs = Obj.collection.find({'_id': nm});
        var code = null;

        crs.each(function(coll, el)
        {
            //console.log(el);

            if (el)
            {
                code = '' + el.value.code;

                if (code)
                {
                    var fnc = eval('(' + el.value.code + ')');

                    try
                    {
                        fnc(data, cb)
                    }
                    catch (eee)
                    {
                        cb(eee);
                    }
                }
            }
        });
    }

    $.invoke = doInvoke;
}

var setWF = function ()
{
    $.wf = require(path.join(config.server_path, 'modules', 'wf', 'wf.js'));
    $.wf.reg($, null, function()
    {

    });
}

var setMongoDB = function()
{
    try
    {
        $.db = require(path.join(__dirname, 'svr', 'db.js'));
        $.db.reg($, null, function()
        {
            setWF();
        });
    }
    catch (e)
    {
        $.err('*** error in db setup: %s', e, true);
    }
}

var setSQLDB = function()
{
    try
    {
        $.sql = require(path.join(__dirname, 'svr', 'mssql.js'));
        $.sql.reg($);
    }
    catch (e)
    {
        $.err('*** error in db setup: %s', e, true);
    }
}

var setTvilio = function()
{
    TwilioClient = require('twilio').Client,
        Twiml = require('twilio').Twiml;

    try
    {
        client = new TwilioClient('ACa0cd44f621c0468e9789284dc434f946', '4d40ff2621c2379324daf41736a9ac9f', '415-599-2671');
        phone = client.getPhoneNumber(twp);
        $.log('twilio connected: %s', twp, true);
    }
    catch (eee)
    {
        $.err('**** twilio error: %s', eee);
    }
}

$.sendSMS = function(pn, txt)
{
    var twilio = require('twilio');
    var client = new twilio.RestClient('ACa0cd44f621c0468e9789284dc434f946', 'b4e7b90a01f2a7bd0fbad405f4fc6107');

    client.sms.messages.create({
            to: pn,
            from:'19123859879',
            body: txt
        },
        function(error, message)
        {
            if (!error) {
                console.log('Success! The SID for this SMS message is:');
                console.log(message.sid);

                console.log('Message sent on:');
                console.log(message.dateCreated);
            }
            else
            {
                console.log('There was SMS error:');
                console.log(error);
            }
        });
}

$.sendEmail = function(addrto, addrfrom, subj, text, att, cb)
{

    /*

     Access Key ID:
     AKIAJCM24CNBN6LQ3SHQ
     Secret Access Key:
     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
     */



    var nodemailer = require('nodemailer');
    var ses = require('nodemailer-ses-transport');
    var transporter = nodemailer.createTransport(ses({
        accessKeyId: 'AKIAJCM24CNBN6LQ3SHQ',
        secretAccessKey: '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'
    }));
    transporter.sendMail({
        from: 'aws@yury.com',
        to: 'yury@yury.com',
        subject: 'hello',
        text: 'hello world!'
    });

    return


    var nodemailer = require('nodemailer');
    var smtpTransport = require('nodemailer-smtp-transport');

    var transporter = nodemailer.createTransport({
        service: 'Gmail',
        auth: {
            user: 'gv.yury@gmail.com',
            pass: 'ipeNta001'
        }
    });

    var mailOptions = {
        from: 'gv.yury@gmail.com', // sender address
        to: addrto, // list of receivers
        subject: subj, // Subject line
        text: text, // plaintext body
        html: text // html body
    };

    transporter.sendMail(mailOptions, function(error, info)
    {
        if(error)
        {
            console.log(error);
        }
        else
        {
            console.log('Message sent: ' + info.response);
            console.log(info);
        }

        if (cb)
        {
            cb(error, info);
        }
    });
}

$.sendEmailOld = function(addrto, addrfrom, subj, text, att, cb)
{
    var nodemailer = require('nodemailer');

    try
    {
        if (true)
        {
            smtpTransport = nodemailer.createTransport(
                {
                    host: "Smtpout.secureserver.net", // hostname
                    //secureConnection: true, // use SSL
                    //port: 465, // port for secure SMTP
                    auth:
                    {
                        user: "me@yury.com",
                        pass: "slsIpe01"
                    }
                });
        }
        else
        {
            smtpTransport = nodemailer.createTransport(
                {
                    service: 'Gmail',
                    auth:
                    {
                        user: "yurybo@gmail.com",
                        pass: "ipeNta001!"
                    }
                });
        }

        //$.log('created smtp transport', null, true);

        var mailOptions =
        {
            from: addrfrom,
            to: addrto,
            subject: subj,
            text: text,
            html: text
        }

        if (att)
        {
            mailOptions.attachments = [
                {   // file on disk as an attachment
                    fileName: 'Attachment',
                    path: att
                }
            ]
        }

        //console.log(mailOptions);

        smtpTransport.sendMail(mailOptions, function(error, response)
        {
            if(error)
            {
                $.log('error sending email', error, true);
            }
            else
            {
                $.log('email is sent', response, true);
            }

            smtpTransport.close();

            var res = {};

            res.error = error;
            res.response = response;

            if (cb)
            {
                cb(res);
            }
        });
    }
    catch (eee)
    {
        $.error('Error in sendEmail:', eee, true);
    }
}

setMongoDB();


setInvoke();
var __systemLog=new __sysLog($);
