/**
 * Created by coolbong on 2014-07-01.
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
    //console.log('[USERCARD: Command]');
    //console.log(buff.toString('hex').toUpperCase());
    //console.log('[Response]');


    var resp = pcsc.transmit(buff);

    if(resp.sw1 == 0x6c) {
        var sw2 = new Buffer( ("0" + (Number(resp.sw2).toString(16))).slice(-2).toUpperCase(),'hex');
        var buffer = Buffer.concat([buff.slice(0, 4), sw2]);
        resp = pcsc.transmit(buffer);
    }

    if(resp.sw1 == 0x61) {
        var sw2 = new Buffer( ("0" + (Number(resp.sw2).toString(16))).slice(-2).toUpperCase(),'hex');
        var buffer = Buffer.concat([new Buffer('00C00000', 'hex'), sw2]);
        resp = pcsc.transmit(buffer);
    }

    resp.sw = resp.sw.toString(16);
    //console.log(resp.resp.toString('hex').toUpperCase());
    //console.log('(' + resp.sw + ')');
    return resp;
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