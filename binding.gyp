{
  "targets": [
    {
      "target_name": "pcsc",
      "sources": [ 
      	"./src/node-pcsc.cc",
      	"./src/pcsc.cc",
      	"./src/pcscwrap.cc"
      ],
      "conditions" : [
      	['OS=="win"', {
          'libraries': ['-lWinSCard']
        }]
      ]
    }
  ]
}
