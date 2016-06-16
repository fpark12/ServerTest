/*
 * Copyright (C) 2008-2016 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

 //#include "Define.h"

#include <string>
#include <list>
#include <vector>
#include <mutex>
#include "SimpleIni.h"

#define GConfig Config::instance()
#define Run

struct ConfigFile
{
	std::string Filename;
	CSimpleIniA ConfigFileImpl;
};

class Config
{
private:
	Config() = default;
	~Config() = default;
	Config(Config const&) = delete;
	Config& operator=(Config const&) = delete;

	std::vector<ConfigFile> ConfigFiles;
	ConfigFile* Find(std::string const& Filename) const;

public:
	static Config* Config::instance();

	/// Method used only for loading main configuration files
	bool Load(std::string const& Filename);
	bool Reload(std::string const& Filename);

	bool GetString(std::string const& section, std::string const& Key, std::string& Value, std::string const& Filename) const;
	bool GetBool(std::string const& section, std::string const& Key, bool& Value, std::string const& Filename) const;
	bool GetInt(std::string const& section, std::string const& Key, int& Value, std::string const& Filename) const;
	bool GetFloat(std::string const& section, std::string const& Key, float& Value, std::string const& Filename) const;

	std::list<std::string> const GetFilenames();
	std::list<std::string> const GetKeysByString(std::string const& Key, std::string const& Filename);
	std::list<std::string> const GetKeys(std::string const& Filename);

private:
	//std::lock_guard<std::mutex> Lock();
	std::mutex Config::Lock;

	bool Unload(std::string const& Filename);
};

