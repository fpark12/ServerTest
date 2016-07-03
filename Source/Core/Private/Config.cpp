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


 //#include "Errors.h"
 //#include "Log.h"
#include "Config.h"

std::mutex Config::Lock;
std::vector<ConfigFile> Config::ConfigFiles;

bool Config::Load(std::string const& Filename)
{
	std::lock_guard<std::mutex> lock(Lock);

	if (Find(Filename))
	{
		return true;
	}

	ConfigFiles.emplace_back(ConfigFile());
	ConfigFiles.back().ConfigFileImpl.SetUnicode();
	SI_Error res = ConfigFiles.back().ConfigFileImpl.LoadFile(Filename.c_str());

	if (res == SI_OK)
	{
		ConfigFiles.back().Filename = Filename;
		return true;
	} else
	{
		ConfigFiles.pop_back();
		return false;
	}
}

bool Config::Unload(std::string const& Filename)
{
	auto i = std::begin(ConfigFiles);

	while (i != std::end(ConfigFiles))
	{
		if (i->Filename == Filename)
		{
			i = ConfigFiles.erase(i);
		} else
		{
			++i;
		}
	}

	return true;
}

bool Config::Reload(std::string const& Filename)
{
	if (Unload(Filename))
	{
		return Load(Filename);
	}

	return false;
}

const ConfigFile* Config::Find(std::string const& Filename) const
{
	for (const auto& i : ConfigFiles)
	{
		if (i.Filename == Filename)
		{
			return &i;
		}
	}
	return nullptr;
}

std::list<std::string> Config::GetFilenames() const
{
	std::list<std::string> Filenames;
	{
		std::lock_guard<std::mutex> lock(Lock);
		for (auto i : ConfigFiles)
		{
			Filenames.push_back(i.Filename);
		}
	}
	return Filenames;
}

std::list<std::string> Config::GetKeys(std::string const& Filename) const
{
	std::list<std::string> Keys;
	{
		std::lock_guard<std::mutex> lock(Lock);

		const ConfigFile* File = Find(Filename);
		if (!File)
		{
			return Keys;
		}

		CSimpleIniA::TNamesDepend KeysImpl;
		File->ConfigFileImpl.GetAllKeys("", KeysImpl);
		for (auto i : KeysImpl)
		{
			Keys.push_back(i.pItem);
		}
	}

	return Keys;
}

std::list<std::string> Config::GetKeysByString(std::string const& Key, std::string const& Filename) const
{
	std::list<std::string> Keys;
	{
		std::lock_guard<std::mutex> lock(Lock);

		const ConfigFile* File = Find(Filename);
		if (!File)
		{
			return Keys;
		}

		CSimpleIniA::TNamesDepend KeysImpl;
		File->ConfigFileImpl.GetAllKeys("", KeysImpl);
		for (auto i : KeysImpl)
		{
			if (std::string(i.pItem).substr(0, Key.length()) == Key)
			{
				Keys.push_back(i.pItem);
			}
		}
	}

	return Keys;
}

bool Config::GetString(std::string const& Section, std::string const& Key, std::string& Value, std::string const& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return "";
	}

	return ConfigFile->ConfigFileImpl.GetValue(Section.c_str(), Key.c_str(), Value, false);
}

bool Config::GetBool(std::string const& Section, std::string const& Key, bool& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return false;
	}
	return ConfigFile->ConfigFileImpl.GetBoolValue(Section.c_str(), Key.c_str(), Value, false);
}

bool Config::GetInt(std::string const& Section, std::string const& Key, int& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return false;
	}
	return ConfigFile->ConfigFileImpl.GetLongValue(Section.c_str(), Key.c_str(), (long&)Value, false);
}

bool Config::GetFloat(std::string const& Section, std::string const& Key, float& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return false;
	}
	double Double;
	bool bResult = ConfigFile->ConfigFileImpl.GetDoubleValue(Section.c_str(), Key.c_str(), Double, false);
	if (bResult)
	{
		Value = (float)Double;
	}

	return bResult;
}
