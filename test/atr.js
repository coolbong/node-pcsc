/**
 * Created by coobong on 2014-07-02.
 */


var assert = require('assert');
var pcsc = require('../index');

exports.atr = {
    'atr': function() {
        pcsc.init();
        var atr = pcsc.connect();
        assert(Buffer.isBuffer(atr));
    }
};
