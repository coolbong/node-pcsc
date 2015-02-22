#ifndef __PCSC_H__
#define __PCSC_H__
#include <node.h>
#include "pcsc_type.h"


class PCSC : public node::ObjectWrap {
 public:
  static void Init(v8::Handle<v8::Object> target);
  

 private:
	PCSC();
	~PCSC();

	static v8::Handle<v8::Value> New(const v8::Arguments& args);
	
	static v8::Handle<v8::Value> Init(const v8::Arguments& args);
	static v8::Handle<v8::Value> GetList(const v8::Arguments& args);
	static v8::Handle<v8::Value> GetReadersName(const v8::Arguments& args);
	static v8::Handle<v8::Value> SetReader(const v8::Arguments& args);
	static v8::Handle<v8::Value> Connect(const v8::Arguments& args);
	static v8::Handle<v8::Value> Transmit(const v8::Arguments& args);
	static v8::Handle<v8::Value> Disconnect(const v8::Arguments& args);
	


#if 0
	//for example
	static v8::Handle<v8::Value> GetCounter(const v8::Arguments& args);
	static v8::Handle<v8::Value> GetBytes(const v8::Arguments& args);

	
	double counter_;
#endif

};

#endif /* #ifndef __PCSC_H__ */
