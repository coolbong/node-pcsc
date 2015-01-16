# node-pcsc

pc/sc c++ addon


## Example

	var pcsc = require('node-pcsc');

	pcsc.init();

	var atr = pcsc.connect();
	console.log('atr : ' + atr.toString('hex').toUpperCase());
	input = new Buffer('00A4040008A000000151000000', 'hex');

	result = pcsc.sendapdu(input);
	console.log(result);


## Installation

You can install the latest tag via npm:

	npm install git://github.com/coolbong/node-pcsc.git
