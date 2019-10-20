#pragma once

#include <jsapi.h>
#include "JSEnvBuilder.h"
#include <vector>
#include <map>
#include <iostream>

namespace JSEngine
{
	namespace Extern
	{
		std::map<std::string, std::pair<JSContext*, JSFunction*>> WebServerCallMap; // First string = path, second string = event type
	}

	namespace WebServer
	{
		static JSClass WebServer =
		{
			"WebServer"
		};

		struct WebContentData
		{
			std::string webPath;
		};

		void WebContentFinalize(JSFreeOp* fop, JSObject* obj)
		{
			WebContentData* wcd = reinterpret_cast<WebContentData*>(JS_GetPrivate(obj));
			delete wcd;
		}

		static JSClassOps WebContentOps =
		{
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			WebContentFinalize
		};

		static JSClass WebContent =
		{
			"WebContent",
			JSCLASS_HAS_PRIVATE,
			&WebContentOps
		};

		struct WebEventData
		{

		};

		void WebEventFinalize(JSFreeOp* fop, JSObject* obj)
		{
			WebEventData* wed = reinterpret_cast<WebEventData*>(JS_GetPrivate(obj));
			delete wed;
		}

		static JSClassOps WebEventOps =
		{
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			WebEventFinalize
		};

		static JSClass WebEvent =
		{
			"WebEvent",
			JSCLASS_HAS_PRIVATE,
			&WebEventOps
		};

		bool WebServerConstruct(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, JS_NewObjectForConstructor(cx, &WebServer, args));
			args.rval().setObject(*obj);
			return false;
		}

		bool WebServerRegisterContent(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			if (args.length() != 1) return false;
			if (!args[0].isString()) return false;
			JS::RootedObject obj(cx, &args.thisv().toObject());
			JS::RootedValue constructor_val(cx);
			JS::RootedObject global(cx, JS_GetGlobalForObject(cx, obj));
			if (!JS_GetProperty(cx, global, "WebContent", &constructor_val))
				return false;
			if (!constructor_val.isObject())
			{
				JS_ReportErrorASCII(cx, "WebContent not found. Something went wrong with initialisation. Please contact a developer");
				return false;
			}
			JS::RootedObject constructor(cx, &constructor_val.toObject());
			JS::RootedObject contentObj(cx, JS_New(cx, constructor, JS::HandleValueArray::empty()));
			if (!contentObj) return false;
			WebContentData* wcd = new WebContentData;
			wcd->webPath = JS_EncodeString(cx, args[0].toString());
			JS_SetPrivate(contentObj, wcd);
			args.rval().setObject(*contentObj);
			return true;
		}

		bool WebContentConstruct(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, JS_NewObjectForConstructor(cx, &WebContent, args));
			args.rval().setObject(*obj);
			return true;
		}

		bool WebContentAddEventListener(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			WebContentData* wcd = reinterpret_cast<WebContentData*>(JS_GetPrivate(obj));
			if (args.length() != 2) return false;
			if (!args[0].isString()) return false;
			if (!args[1].isObject()) return false;
			if (!JS_ObjectIsFunction(cx, &args[1].toObject())) return false;
			std::string str(std::move(JS_EncodeString(cx, args[0].toString())));
			if (!Extern::WebServerCallMap.count(wcd->webPath))
			{
				std::cout << "Registered Web Javascript Interface at '" << wcd->webPath << "'." << std::endl;
				Extern::WebServerCallMap[wcd->webPath] = { cx, JS_ValueToFunction(cx, args[1]) };
				args.rval().setBoolean(true);
			}
			else args.rval().setBoolean(false);
			return true;
		}

		bool WebEventConstruct(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			WebEventData* wed = new WebEventData;
			JS::RootedObject obj(cx, JS_NewObjectForConstructor(cx, &WebEvent, args));
			JS_SetPrivate(obj, wed);
			args.rval().setObject(*obj);
			return true;
		}

		bool Init(Environment::EnvironmentInformation* ei)
		{
			static JSFunctionSpec sfsp[] =
			{
				{"registerContent", &WebServerRegisterContent, 0, 0}
			};

			static JSFunctionSpec wcfs[] =
			{
				{"addEventListener", &WebContentAddEventListener, 0, 0}
			};

			JS::RootedObject webContent(ei->cx, JS_InitClass(ei->cx, ei->global, nullptr, &WebContent, &WebContentConstruct, 0, nullptr, wcfs, nullptr, nullptr));
			JS::RootedObject webServer(ei->cx, JS_InitClass(ei->cx, ei->global, nullptr, &WebServer, &WebServerConstruct, 0, nullptr, nullptr, nullptr, sfsp));
			return webContent != nullptr && webServer != nullptr;
		}
	}
}