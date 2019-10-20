#pragma once

#include <map>
#include <string>

namespace RankSystem {
	std::map<std::string, uint64_t> rankDict;
	std::map<std::string, uint64_t> permissionParseDict;

	constexpr uint64_t BSHIFT(unsigned char bytes) {
		return static_cast<uint64_t>(1) << bytes;
	}

	constexpr uint64_t PERMISSION_ALL = 0xFFFFFFFFFFFFFFFF;
	constexpr uint64_t PERMISSION_MANAGE_ACCOUNTS = BSHIFT(1);
	constexpr uint64_t PERMISSION_POST_BLOG = BSHIFT(2);
	constexpr uint64_t PERMISSION_MANAGE_SERVER_PERMISSIONS = BSHIFT(3);
	constexpr uint64_t PERMISSION_NONE = 0;

	void addRank(const char* name, uint64_t permission) {
		rankDict[name] = permission;
	}

	bool addRankPermission(const char* name, uint64_t permission) {
		if (rankDict.count(name) > 0) {
			rankDict[name] = rankDict[name] | permission;
			return true;
		}
		else return false;
	}

	bool removeRankPermission(const char* name, uint64_t permission) {
		if (rankDict.count(name) > 0) {
			rankDict[name] = rankDict[name] & ~permission;
			return true;
		}
		else return false;
	}

	bool removeRank(const char* name) {
		if (rankDict.count(name) > 0) {
			rankDict.erase(name);
			return true;
		}
		else return false;
	}

	void init() {
		std::cout << "Rank Initiation started." << std::endl;
		permissionParseDict["all"] = PERMISSION_ALL;
		permissionParseDict["post_blog"] = PERMISSION_POST_BLOG;
		permissionParseDict["manage_accounts"] = PERMISSION_MANAGE_ACCOUNTS;
		permissionParseDict["manage_server_permissions"] = PERMISSION_MANAGE_SERVER_PERMISSIONS;
		permissionParseDict["none"] = PERMISSION_NONE;
	}

	std::string parse(uint64_t permission) {
		std::string output = "";
		if (permission == PERMISSION_ALL) {
			output += "  all";
		}
		else {
			for (std::pair<std::string, uint64_t> pair : permissionParseDict) {
				if (permission & pair.second && pair.first != "all") {
					output += ", " + pair.first;
				}
			}
		}
		if (output.size() > 0) return output.substr(2);
		output += "none";
		return output;
	}

	uint64_t parse(const char* permission) {
		if (permissionParseDict.count(permission) > 0) {
			return permissionParseDict[permission];
		}
		return 0;
	}

	uint64_t getRankPermissions(const char* name) {
		if (rankDict.count(name) > 0) {
			return rankDict[name];
		}
		else return 0;
	}

	bool rankExists(const char* name) {
		return rankDict.count(name) > 0;
	}
}