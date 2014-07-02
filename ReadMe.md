# node-pcsc
      
pc/sc c++ addon


## Example

	var pcsc = require('node-pcsc');

	pcsc.init();

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



## Installation

You can install the latest tag via npm:

	npm install git://github.com/coolbong/node-pcsc.git

