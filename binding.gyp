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
        }],
        ['OS=="linux"', {
          'include_dirs': [ '/usr/include/PCSC' ],
          'link_settings': {
            'libraries': [ '-lpcsclite' ],
            'library_dirs': [ '/usr/lib' ]
          }
        }],
         ['OS=="mac"', {
            'libraries': ['-framework', 'PCSC']
        }]
      ]
    }
  ]
}
