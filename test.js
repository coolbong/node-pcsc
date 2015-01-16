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
console.log('atr : ' + atr.toString('hex').toUpperCase());
input = new Buffer('00A4040008A000000151000000', 'hex');

result = pcsc.sendapdu(input);
console.log(result);
