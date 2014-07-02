/**
 * Created by coobong on 2014-07-01.
 */



//var bindings = require('bindings')('pcsc');
var pcsc = require('../build/Release/pcsc').PCSC;

var pcsc = new pcsc();

var parse_readers_string = function(readers_str) {
    var pos;
    var readers = [];
    var ini = 0;
    while ((pos = readers_str.slice(ini).indexOf('\0')) > 0) {
        readers.push(readers_str.slice(ini, ini + pos).toString());
        ini += pos + 1;
    }

    return readers;
};

function init() {
    pcsc.init();
    get_reader_list();
}

function get_reader_list() {
    return parse_readers_string(pcsc.getReadersName().toString());
}

function select_reader(reader_name) {
    pcsc.setReader(reader_name);
}

function connect(reader) {
    if(reader !== undefined && (typeof reader === 'string')) {
        pcsc.setReader(reader);
    }

    return pcsc.connect();
}

function sendapdu(buff) {
    return pcsc.transmit(buff);
}

function disconnet() {
    return pcsc.disconnect();
}

module.exports = {
    init:init,
    get_reader_list: get_reader_list,
    connect: connect,
    sendapdu: sendapdu,
    select_reader: select_reader,
    disconnet:disconnet
};