//#ifndef BUILDING_NODE_EXTENSION
//#define BUILDING_NODE_EXTENSION
//#endif
#include <node.h>
#include "pcsc.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
  PCSC::Init(exports);
}

NODE_MODULE(pcsc, InitAll)
