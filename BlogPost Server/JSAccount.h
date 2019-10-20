#pragma once

#include <jsapi.h>
#include "JSEnvBuilder.h"
#include "Logger.h"
#include <algorithm>
#include <Encryptor.h>
#include <Configurable.h>

namespace JSEngine
{
	namespace Account
	{
		struct AccountData
		{
			std::string name;
		};

		void AccountFinalize(JSFreeOp* fop, JSObject* obj)
		{
			AccountData* p = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			delete p;
		}

		static JSClassOps AccountOps =
		{
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			AccountFinalize
		};

		static JSClass Account =
		{
			"Account",
			JSCLASS_HAS_PRIVATE,
			&AccountOps
		};

		bool AccountConstructor(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, JS_NewObjectForConstructor(cx, &Account, args));

			if (!obj) return false;

			AccountData* ad = new AccountData;
			JS_SetPrivate(obj, ad);

			if (args.length() == 1 && args[0].isString())
			{
				std::string pointer(std::move(JS_EncodeString(cx, args[0].toString())));
				ad->name = pointer;
				std::transform(ad->name.begin(), ad->name.end(), ad->name.begin(), std::tolower);
			}

			args.rval().setObject(*obj);
			return true;
		}

		bool AccountSetName(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			if (args.length() != 1) return false;
			JS::RootedObject obj(cx, &args.thisv().toObject());
			AccountData* ad = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			if (ad == nullptr) return false;
			if (!args[0].isString()) return false;
			std::string text(std::move(JS_EncodeString(cx, args[0].toString())));
			ad->name = text;
			std::transform(ad->name.begin(), ad->name.end(), ad->name.begin(), std::tolower);
			return true;
		}

		bool AccountGetName(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			AccountData* ad = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			if (ad == nullptr) return false;
			args.rval().setString(JS_NewStringCopyN(cx, ad->name.c_str(), ad->name.size()));
			return true;
		}

		bool AccountGetDisplayName(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			AccountData* ad = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			if (ad == nullptr) return false;
			std::string displayName = Logger::getUserDisplayName(ad->name);
			args.rval().setString(JS_NewStringCopyN(cx,	displayName.c_str(), displayName.size()));
			return true;
		}

		bool AccountGetValid(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			AccountData* ad = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			args.rval().setBoolean(Logger::userExists(ad->name.c_str()));
			return true;
		}

		bool AccountNoSet(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			return true; // Nothing to set
		}

		bool AccountGetOnline(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			AccountData* ad = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			args.rval().setBoolean(Logger::isLogged(ad->name));
			return true;
		}

		bool AccountPasswordValid(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			if (argc != 1) return false;
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			AccountData* ad = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			if (!args[0].isString()) return false;
			std::string password(std::move(JS_EncodeString(cx, args[0].toString())));
			if (Logger::LoggDict.get().count(ad->name))
			{
				std::string hash = Logger::LoggDict[ad->name.c_str()]["hash"].getValue();
				args.rval().setBoolean(Encryptor::hashCompare(password, hash)); // Password needs to be encrypted.
			}
			else
			{
				args.rval().setBoolean(false);
			}
			return true;
		}

		bool AccountGetAccounts(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject arr(cx, JS_NewArrayObject(cx, Logger::LoggDict.get().size()));
			JS::RootedValue constructorVal(cx);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			JS::RootedObject global(cx, JS_GetGlobalForObject(cx, obj));
			if (!JS_GetProperty(cx, global, "Account", &constructorVal)) return false;
			if (!constructorVal.isObject()) return false;
			JS::RootedObject constructor(cx, &constructorVal.toObject());
			JS::AutoValueArray<1> cargs(cx); // Not needed
			cargs[0].setBoolean(true);
			int count = 0;
			for (std::pair<std::string, ConfigObject> cfgObj : Logger::LoggDict.get())
			{
				JS::RootedObject account(cx, JS_New(cx, constructor, cargs));
				if (!account) return false;
				JS::RootedValue nameVal(cx);
				nameVal.setString(JS_NewStringCopyN(cx, cfgObj.first.c_str(), cfgObj.first.size()));
				JS_SetProperty(cx, account, "name", nameVal);
				JS_SetElement(cx, arr, count, account);
				++count;
			}
			args.rval().setObject(*arr);
			return true;
		}

		bool AccountGetAccountsOnline(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject arr(cx, JS_NewArrayObject(cx, Logger::LoggDict.get().size()));
			JS::RootedValue constructorVal(cx);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			JS::RootedObject global(cx, JS_GetGlobalForObject(cx, obj));
			if (!JS_GetProperty(cx, global, "Account", &constructorVal)) return false;
			if (!constructorVal.isObject()) return false;
			JS::RootedObject constructor(cx, &constructorVal.toObject());
			JS::AutoValueArray<1> cargs(cx); // Not needed
			cargs[0].setBoolean(true);
			int count = 0;
			for (std::pair<SOCKET, std::string> cfgObj : Logger::loggedSockets)
			{
				JS::RootedObject account(cx, JS_New(cx, constructor, cargs));
				if (!account) return false;
				JS::RootedValue nameVal(cx);
				nameVal.setString(JS_NewStringCopyN(cx, cfgObj.second.c_str(), cfgObj.second.size()));
				JS_SetProperty(cx, account, "name", nameVal);
				JS_SetElement(cx, arr, count, account);
				++count;
			}
			args.rval().setObject(*arr);
			return true;
		}

		bool AccountDelete(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			AccountData* ad = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			args.rval().setBoolean(Logger::eraseUser(ad->name.c_str()));
			return true;
		}

		bool AccountCreate(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			if (args.length() != 2) return false;
			if (!args[0].isString()) return false;
			if (!args[1].isString()) return false;
			JS::RootedValue constructorVal(cx);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			JS::RootedObject global(cx, JS_GetGlobalForObject(cx, obj));
			JS_GetProperty(cx, global, "Account", &constructorVal);
			if (!constructorVal.isObject()) return false;
			JS::RootedObject constructor(cx, &constructorVal.toObject());
			JS::AutoValueArray<1> cargs(cx); // Not needed
			cargs[0].setBoolean(true);
			JS::RootedObject account(cx, JS_New(cx, constructor, cargs));
			std::string username(std::move(JS_EncodeString(cx, args[0].toString())));
			std::transform(username.begin(), username.end(), username.begin(), std::tolower);
			std::string password(std::move(JS_EncodeString(cx, args[1].toString())));
			std::string hash(Encryptor::hashEncrypt(password));
			if (Logger::registerUser(username.c_str(), hash.c_str())) // TODO Fix Hash
			{
				JS::RootedValue val(cx);
				val.setString(args[0].toString());
				JS_SetProperty(cx, account, "name", val);
				args.rval().setObject(*account);
				return true;
			}
			else
			{
				args.rval().setBoolean(false);
				return true;
			}
		}

		bool AccountSendMessage(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			JS::RootedObject obj(cx, &args.thisv().toObject());
			AccountData* ad = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			SOCKET socket = Logger::getSocket(ad->name);
			if (socket != INVALID_SOCKET)
			{
				ConfigObject packet;
				ConfigObject messageParam;
				std::string output;
				for (unsigned int i = 0; i < args.length(); i++)
				{
					std::string msg(std::move(JS_EncodeString(cx, args[i].toString())));
					if (args[i].isString())
						output += msg;
				}
				Packet::CreateMessagePacket(messageParam);
				Packet::AddMessageToPacket(messageParam, output);
				PacketList::add(packet, messageParam);
				SocketExchange exch(socket);
				Configurable::netSend(packet, exch);
				args.rval().setBoolean(true);
				return true;
			}
			else
			{
				args.rval().setBoolean(false);
				return true;
			}
		}

		bool AccountSetNameOfficial(JSContext* cx, unsigned argc, JS::Value* vp)
		{
			JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
			if (args.length() != 1) return false;
			if (!args[0].isString()) return true;
			std::string nameVal(std::move(JS_EncodeString(cx, args[0].toString())));
			JS::RootedObject obj(cx, &args.thisv().toObject());
			AccountData* ad = reinterpret_cast<AccountData*>(JS_GetPrivate(obj));
			if (!Logger::userExists(nameVal.c_str()))
			{
				Logger::LoggDict[nameVal.c_str()] = Logger::LoggDict[ad->name.c_str()];
				if(Logger::userRankDict.count(ad->name.c_str()) > 0)
					Logger::userRankDict[nameVal.c_str()] = Logger::userRankDict[ad->name.c_str()];
				SOCKET socket = Logger::getSocket(ad->name.c_str());
				Logger::eraseUser(ad->name.c_str());
				if (socket != INVALID_SOCKET)
				{
					Logger::loggedSockets[socket] = nameVal.c_str();
				}

				ad->name = nameVal;
				args.rval().setBoolean(true);
				return true;
			}
			args.rval().setBoolean(false);
			return true;
		}

		bool Init(Environment::EnvironmentInformation* ei)
		{
			static JSFunctionSpec acfn[] =
			{
				{"setPointer", AccountSetName, 0, 0},
				{"getPointer", AccountGetName, 0, 0},
				{"isOnline", AccountGetOnline, 0, 0},
				{"isValid", AccountGetValid, 0, 0},
				{"isPassword", AccountPasswordValid, 0, 0},
				{"delete", AccountDelete, 0, 0},
				{"getDisplayName", AccountGetDisplayName, 0, 0},
				{"sendMessage", AccountSendMessage, 0, 0},
				{"setName", AccountSetNameOfficial, 0, 0},
				{"getName", AccountGetName, 0, 0},
				JS_FS_END
			};

			static JSFunctionSpec acsfn[] =
			{
				{"getAccounts", AccountGetAccounts, 0, 0},
				{"getAccountsOnline", AccountGetAccountsOnline, 0, 0},
				{"create", AccountCreate, 0, 0},
				JS_FS_END
			};

			static JSPropertySpec acps[] =
			{
				JS_PSGS("pointer", AccountGetName, AccountSetName, JSPROP_ENUMERATE),
				JS_PSGS("displayName", AccountGetDisplayName, AccountNoSet, JSPROP_ENUMERATE),
				JS_PSGS("valid", AccountGetValid, AccountNoSet, JSPROP_ENUMERATE),
				JS_PSGS("online", AccountGetOnline, AccountNoSet, JSPROP_ENUMERATE),
				JS_PSGS("name", AccountGetName, AccountNoSet, JSPROP_ENUMERATE),
				JS_PS_END
			};

			static JSPropertySpec acsps[] =
			{
				JS_PSGS("accounts", AccountGetAccounts, AccountNoSet, JSPROP_ENUMERATE),
				JS_PSGS("accountsOnline", AccountGetAccountsOnline, AccountNoSet, JSPROP_ENUMERATE),
				JS_PS_END
			};

			JS::RootedObject AccountBuild(ei->cx, JS_InitClass(ei->cx, ei->global, nullptr, &Account,
										&AccountConstructor, 0, acps, acfn, acsps, acsfn));
			return AccountBuild != nullptr;
		}
	}
}