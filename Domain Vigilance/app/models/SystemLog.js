
/**
 * Created by Iman Nassirian on 12/16/15.
 */

    var log=function($) {
        this.$ = $;
        this.db = $.db.db;
        this.Obj = this.db.model('systemLog', $.db.ObjectsSchema);
    };

    log.prototype.create = function (userId, action, message, data) {
        try {
            var logInfo = {
                log_type: 'systemLog'
            };
            if (action) logInfo.action = action;
            if (data.func) logInfo.func = data.func;
            if (data.__name) logInfo.fileName = data.__name;
            //need to convert to string and strinigift and parse it again bCuz of the esb data format to make sure we can
            //store the data without error
            if (data) logInfo.params = JSON.parse(JSON.stringify(data.toString()));
            if (userId)  {
                logInfo.user_id = userId;
                logInfo.systemLog=false;
            } else {
                logInfo.systemLog=true;
            }
            if (message) logInfo.message = message;
            logInfo.created_at = new Date();
            var crs = this.Obj.collection.save(logInfo, function (err, res) {
                if (err) {
                    console.log('failed to create sysLog ');
                    console.log(err);
                }
            });
        } catch (eee) {
            console.log('failed to create sysLog ');
            console.log(eee);
        }
    };

    module.exports=log;
