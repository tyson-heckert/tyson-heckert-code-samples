(function()
{
    'use strict';

    angular
        .module('app.modules.core')
        .factory('socketService', socketService);

    socketService.$inject = ['$http', 'utilsService', 'CS_CONFIG', 'triMenu', '$timeout'];

    function socketService($http, utilsService, CS_CONFIG, triMenuProvider, $timeout)
    {
        var _this = this;

        var socketURL = CS_CONFIG.socketUrl;

        var socket = io('//' + socketURL);


        _this.isConnected = false; // is the user currently connected
        _this.hasConnected = false; // has the user ever connected successfully
        _this.errorMessage = '';
        _this.loginReconnect = false;

        // test code for when firewall blocks WS
        $timeout(function(){

            if(!_this.hasConnected
            || !utilsService.CURRENT_USER._id){

                _this.isConnected = false;
                utilsService.rootScope.$broadcast('ws.socket.connected');

                _this.errorMessage = "Unable to connect. Retrying with different protocol...";
                utilsService.rootScope.$broadcast('socket.error_message');



                socket.disconnect();
                socket.rememberUpgrade = false;
                socket.io.opts.transports = ['polling'];
                socket.connect();


                $timeout(function(){

                    if(!_this.hasConnected){
                        _this.errorMessage = "Unable to connect. Please check your internet connection.";
                        utilsService.rootScope.$broadcast('socket.error_message');

                        socket.disconnect();
                        socket.rememberUpgrade = true;
                        socket.io.opts.transports = ['polling', 'websocket'];
                        socket.connect();
                    }else{ // connected but not receiving data
                        _this.errorMessage = "Unable to connect. Please check your firewall settings.";
                        utilsService.rootScope.$broadcast('socket.error_message');
                    }
                }, 5000);

            }else{

            }
        }, 5000);



        socket.on('connect', function()
        {
            console.log('>>> connected');

            _this.isConnected = true;
            _this.hasConnected = true;

            utilsService.rootScope.$broadcast('ws.socket.connected');
        });

        //socket.on('cnsl', function(msg)
        //{
        //    console.log(msg);
        //});
        //
        socket.on('reconnect', function(socket)
        {
            console.log('<<<>>> re-connected');
            _this.isConnected = true;
            utilsService.rootScope.$broadcast('socket.reconnected');

        });

        socket.on('disconnect', function(socket)
        {
            console.log('<<<>>> disconnected');
            _this.isConnected = false;
            utilsService.rootScope.$broadcast('socket.disconnected');
            utilsService.rootScope.$broadcast('ws.socket.connected');

            _this.errorMessage = "Not connected. Please check your internet connection.";
            utilsService.rootScope.$broadcast('socket.error_message');


        });

        socket.on('case.updated', function(tid)
        {
            console.log('case.updated: ' + tid);

            utilsService.rootScope.$broadcast('case.updated', {tid: tid});
        });

        /**
         *
         * receive push notifications from the server
         */
        socket.on('push', function(data)
        {
            if(utilsService.debug){
                console.log('push-' + data.event+':%o', data.data);
            }

            if(data.event == 'core.login.notfound'){ // davin is putting this here until we make a login page
                _this.isConnected = true;
                utilsService.rootScope.$broadcast('ws.socket.connected');

                utilsService.toast('Invalid Login: User not found', 'error');
                utilsService.toast('Redirect user to logout page: '+CS_CONFIG.logoutUrl);
                window.location = 'http://'+CS_CONFIG.logoutUrl;

            }else if (data.event == 'core.login.invalid'){
                _this.isConnected = true;
                utilsService.rootScope.$broadcast('ws.socket.connected');

                utilsService.toast('Invalid login: Invalid', 'error');
                utilsService.toast('Redirect user to logout page: '+CS_CONFIG.logoutUrl);
                window.location = 'http://'+CS_CONFIG.logoutUrl;

            }else if (data.event == 'core.login.success'){
                _this.isConnected = true;
                utilsService.rootScope.$broadcast('ws.socket.connected');

                utilsService.toast('Welcome '+data.data.name);
                // save user data here.
                utilsService.CURRENT_USER = data.data;

                utilsService.drawMenu({provider: triMenuProvider, role: data.data.role});
                utilsService.rootScope.$apply();
                utilsService.rootScope.$broadcast('user.update');

                if(_this.loginReconnect){
                    utilsService.rootScope.$broadcast('user.reconnect');
                }
                _this.loginReconnect = true;

            }else if(data.event == 'core.environment'){
                utilsService.environment = data.data.environment;
                utilsService.debug = data.data.debug;
            }else if(data.event == 'core.version'){
                if(data.data.commit && data.data.commit != ''){
                    if(VER != '<!-- gitCommit -->' && VER != data.data.commit){
                        utilsService.rootScope.$broadcast('core.version.update', data.data.commit);
                    }
                    VER = data.data.commit;
                }
            }else{

                utilsService.rootScope.$broadcast('push.'+data.event, data.data);
            }
        });

        _this.sct = socket;


        _this.ws =
        {
            on: function(evt, cb)
            {
                socket.on(evt, cb);
            },

            emit: function(ch, pr, cb)
            {
                //this.log2('emit: ' + ch + ': [' + JSON.stringify(pr) + ']');
                if(utilsService.debug){
                    console.log("emit:%o", ch);
                }
                socket.emit(ch, pr, cb);
            },

            //j57y: function(name, cb, isEval)
            //{
            //    var hst = document.location.host;
            //
            //    socket.emit('exec', { eval: isEval, url: hst + '/svrjs/' + name }, function(res)
            //    {
            //        if (cb)
            //        {
            //            cb(res);
            //        }
            //    });
            //},

            j57x: function(params, code, cb)
            {
                // code should be defined as anonimous function. this function will be executed on the server and should call cb(res) itself
                var data = {};
                var wc = '(' + code + ')()';

                data = params;
                data.__code = wc;

                socket.emit('j57xy', data, function(res)
                {
                    if (cb)
                    {
                        cb(res);
                    }
                });
            },

            evalremote: function(name, params, cbk)
            {
                var data = params;


                data.__name = name;
                data.__approot = _this.app_remote_root;

                if(utilsService.debug){
                    console.log("evalremote - "+ data.__name+": %o", data);
                }
                //utilsService.loader.start();
                socket.emit('j57xyz', data, function(res)
                {
                    //utilsService.loader.complete();
                    if(utilsService.debug){
                        console.log("evalremote - "+ data.__name+" results: %o", res);
                    }
                    if (cbk)
                    {
                        cbk(res);
                    }
                });
            },

            evalurl: function(name, params, cbk)
            {
                if (utilsService.isLocal())
                {
                    _this.ws.evalurllocal(name, params, cbk);
                }
                else
                {
                    _this.ws.evalremote(name, params, cbk);
                }
            },

            evalurllocal: function(name, params, cbk)
            {
                var url = 'svr/' + name;

                $http
                    .get(url)
                    .success(function(data, status, headers, config)
                    {
                        var js = '' + data;
                        var code = 'function() {\n' + js + '\n}';
                        var dt = Date.parse(headers()['last-modified']).toString();

                        params.__modified = dt;
                        params.__name = name;
                        params.__module = name + '_' + dt + '.js';

                        _this.ws.j57x(params, code, function(res)
                        {
                            cbk(res);
                        });
                    })
                    .error(function(data, status, headers, config)
                    {
                        console.log(arguments);
                    });
            }
        };



        //console.log('[socketService] has been invoked');



        return this;
    };
})();

