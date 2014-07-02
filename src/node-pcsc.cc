#define BUILDING_NODE_EXTENSION
#include <node.h>
#include "pcsc.h"

using namespace v8;

void InitAll(Handle<Object> exports) {
  PCSC::Init(exports);
}

NODE_MODULE(pcsc, InitAll)
