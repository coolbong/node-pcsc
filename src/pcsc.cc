#define BUILDING_NODE_EXTENSION
#include <node.h>
#include <node_buffer.h>
#include "pcsc_type.h"
#include "pcsc.h"
#include "pcscwrap.h"


using namespace v8;
using namespace node;

PCSC::PCSC() {};
PCSC::~PCSC() {
    pcsc_disconnect();
};

byte receive_buf[256] = {0};

void PCSC::Init(Handle<Object> target) 
{
	// Prepare constructor template
	Local<FunctionTemplate> tpl = FunctionTemplate::New(New);
	tpl->SetClassName(String::NewSymbol("PCSC"));
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	tpl->PrototypeTemplate()->Set(String::NewSymbol("init"), FunctionTemplate::New(Init)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("getList"), FunctionTemplate::New(GetList)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("getReadersName"), FunctionTemplate::New(GetReadersName)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("setReader"), FunctionTemplate::New(SetReader)->GetFunction());


	tpl->PrototypeTemplate()->Set(String::NewSymbol("connect"), FunctionTemplate::New(Connect)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("transmit"), FunctionTemplate::New(Transmit)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("disconnect"), FunctionTemplate::New(Disconnect)->GetFunction());
	

	tpl->PrototypeTemplate()->Set(String::NewSymbol("getCounter"), FunctionTemplate::New(GetCounter)->GetFunction());
	tpl->PrototypeTemplate()->Set(String::NewSymbol("getBytes"), FunctionTemplate::New(GetBytes)->GetFunction());
	//tpl->PrototypeTemplate()->Set(String::NewSymbol("init"), FunctionTemplate::New(Init)->GetFunction());
	

	Persistent<Function> constructor = Persistent<Function>::New(tpl->GetFunction());
	target->Set(String::NewSymbol("PCSC"), constructor);
}

Handle<Value> PCSC::New(const Arguments& args) 
{
	HandleScope scope;

	PCSC* obj = new PCSC();
	obj->counter_ = args[0]->IsUndefined() ? 0 : args[0]->NumberValue();
	obj->Wrap(args.This());

	return args.This();
}

Handle<Value> PCSC::GetCounter(const Arguments& args)
{
	HandleScope scope;

	PCSC* obj = ObjectWrap::Unwrap<PCSC>(args.This());

	return scope.Close(Number::New(obj->counter_));
}

Handle<Value> PCSC::Init(const Arguments& args)
{
	HandleScope scope;	
	LONG ret;

	ret = pcsc_init();
	if(ret != SCARD_S_SUCCESS) {
		return ThrowException(Exception::TypeError(String::New("PCSC SCardEstablishContext error")));
	}
	else {
		//printf("SCardEstablishContext success!! %d\n", ret);
	}

	return scope.Close(Undefined());
}

Handle<Value> PCSC::GetList(const Arguments& args)
{
	HandleScope scope;	
	char *list;

	list = pcsc_get_reader_list();

	return scope.Close(String::New(list));
}

Handle<Value> PCSC::GetReadersName(const Arguments& args)
{
	HandleScope scope;
    LONG ret;


	uint64 len = pcsc_get_readers_name_length();
	if(len == 0) {
	    return ThrowException(Exception::TypeError(String::New("PCSC SCardListReaders error")));
	}
	//else {
	//    printf("readers_name_length: %d\n", len);
	//}

	char *readers_name = new char[len];

	ret = pcsc_get_readers_name(readers_name, len);
    //printf("readers_name: %s\n", readers_name);

	if(ret != SCARD_S_SUCCESS) {
	    delete readers_name;
	    return ThrowException(Exception::TypeError(String::New("PCSC SCardListReaders error")));
	}

    node::Buffer *slowBuffer = node::Buffer::New(len);
	memcpy(node::Buffer::Data(slowBuffer), readers_name, len);

    delete readers_name;

	Local<Object> globalObj = Context::GetCurrent()->Global();
	Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
	Handle<Value> constructorArgs[3] = { slowBuffer->handle_, v8::Integer::New(len), v8::Integer::New(0) };
	Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);
	return scope.Close(actualBuffer);
}
Handle<Value> PCSC::SetReader(const Arguments& args)
{
	HandleScope scope;


    //if (args.Length() < 1) return TYPE_ERROR("reader name required");
    //if (!args[0]->IsString()) return TYPE_ERROR("reader name must be a string");

    if (args.Length() == 0 || !args[0]->IsString()) {
          return ThrowException(Exception::Error(String::New("Must give reader name string as argument")));
    }

    String::Utf8Value reader_name(args[0]);

	pcsc_set_reader_name(*reader_name);
	return scope.Close(Undefined());

}


Handle<Value> PCSC::Connect(const Arguments& args)
{
	HandleScope scope;
	LONG ret;

	byte atr[64];
	DWORD len;

	ret = pcsc_connect(atr, &len);

	if(ret != SCARD_S_SUCCESS) {
		return ThrowException(Exception::TypeError(String::New("PCSC Connect error")));
	}


	node::Buffer *slowBuffer = node::Buffer::New(len);
	memcpy(node::Buffer::Data(slowBuffer), atr, len);

	Local<Object> globalObj = Context::GetCurrent()->Global();
	Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
	Handle<Value> constructorArgs[3] = { slowBuffer->handle_, v8::Integer::New(len), v8::Integer::New(0) };
	Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);
	return scope.Close(actualBuffer);

	//return scope.Close(String::New("Hello World"));
}

Handle<Value> PCSC::Transmit(const Arguments& args)
{
	HandleScope scope;

	if (!node::Buffer::HasInstance(args[0])) {
		return ThrowException(Exception::TypeError(String::New("First argument must be a Buffer")));
	}

	Local<Object> buffer_data = args[0]->ToObject();

	//https://github.com/santigimeno/node-pcsclite/blob/master/src/pcsclite.cpp

	byte* input = (byte *)node::Buffer::Data(buffer_data);
	DWORD input_len = node::Buffer::Length(buffer_data);
	DWORD output_len;
	int32 len;
	LONG ret;

    //printf("input_len: %d\n", input_len);
	ret = pcsc_transmit(input, input_len, receive_buf, &output_len);

	if(ret != SCARD_S_SUCCESS) {
		return ThrowException(Exception::TypeError(String::New("PCSC reset error")));
	}


	len = output_len - 2;
	//printf("output %d\n", output_len);
	//printf("len %d\n", len);

    int sw1 = receive_buf[output_len - 2];
    int sw2 = receive_buf[output_len - 1];
    int sw = (sw1 << 8) | sw2;


	node::Buffer *slowBuffer = node::Buffer::New(len);
	memcpy(node::Buffer::Data(slowBuffer), receive_buf, len);

	Local<Object> globalObj = Context::GetCurrent()->Global();
	Local<Function> bufferConstructor = Local<Function>::Cast(globalObj->Get(String::New("Buffer")));
	Handle<Value> constructorArgs[3] = { slowBuffer->handle_, v8::Integer::New(len), v8::Integer::New(0) };
	Local<Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);

	Local<Object> response = Object::New();
    response->Set(String::New("resp"), actualBuffer);

    response->Set(String::New("sw"), Number::New(sw));
    response->Set(String::New("sw1"), Number::New(sw1));
    response->Set(String::New("sw2"), Number::New(sw2));

	return scope.Close(response);
}



Handle<Value> PCSC::Disconnect(const Arguments& args)
{
	HandleScope scope;
	LONG ret;

	ret = pcsc_disconnect();;

	return scope.Close(Undefined());
}


Handle<Value> PCSC::GetBytes(const Arguments& args)
{
	HandleScope scope;
	const char *data = "Hello world!";
	int length = strlen(data);

	// This is Buffer that actually makes heap-allocated raw binary available
	// to userland code.
	node::Buffer *slowBuffer = node::Buffer::New(length);

	// Buffer:Data gives us a yummy void* pointer to play with to our hearts
	// content.
	memcpy(node::Buffer::Data(slowBuffer), data, length);

	// Now we need to create the JS version of the Buffer I was telling you about.
	// To do that we need to actually pull it from the execution context.
	// First step is to get a handle to the global object.
	v8::Local<v8::Object> globalObj = v8::Context::GetCurrent()->Global();

	// Now we need to grab the Buffer constructor function.
	v8::Local<v8::Function> bufferConstructor = v8::Local<v8::Function>::Cast(globalObj->Get(v8::String::New("Buffer")));

	// Great. We can use this constructor function to allocate new Buffers.
	// Let's do that now. First we need to provide the correct arguments.
	// First argument is the JS object Handle for the SlowBuffer.
	// Second arg is the length of the SlowBuffer.
	// Third arg is the offset in the SlowBuffer we want the .. "Fast"Buffer to start at.
	v8::Handle<v8::Value> constructorArgs[3] = { slowBuffer->handle_, v8::Integer::New(length), v8::Integer::New(0) };

	// Now we have our constructor, and our constructor args. Let's create the 
	// damn Buffer already!
	v8::Local<v8::Object> actualBuffer = bufferConstructor->NewInstance(3, constructorArgs);

	// This Buffer can now be provided to the calling JS code as easy as this:
	return scope.Close(actualBuffer);
}

void printHexString(CHAR* sPrefix, LPBYTE baData, DWORD dataLen)
{
	DWORD i;

	printf("%s", sPrefix);

	for(i = 0; i < dataLen;i++)
	{
		printf("%02X ",baData[i]);
	}

	printf("\n");
}