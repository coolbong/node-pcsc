/**
 * Created by coobong on 2014-07-01.
 */


var assert = require('assert');
var pcsc = require('./index');


pcsc.init();

var readers = pcsc.get_reader_list()
console.log(readers);

pcsc.select_reader(readers[0]);

var atr = pcsc.connect();
console.log(atr.toString('hex').toUpperCase());

input = new Buffer('00A4040008A000000003000000', 'hex');
result = pcsc.sendapdu(input);
console.log(result.sw.toString(16).toUpperCase());


var str = '00C00000' + result.sw2.toString(16);
input = new Buffer(str, 'hex');
result = pcsc.sendapdu(input);
assert(result.sw == 0x9000);
console.log(result.resp.toString('hex').toUpperCase());

