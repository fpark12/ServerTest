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

Config& GConfigRef = Config::Instance();
Config& Config::Instance()
{
	static Config instance;
	return instance;
}

#include "ini.h"

ConfigFile::ConfigFile()
{
}

ConfigFile::~ConfigFile()
{
	// Clean up the field sets
	std::map<std::string, std::set<std::string>*>::iterator fieldSetsIt;
	for (fieldSetsIt = Fields.begin(); fieldSetsIt != Fields.end(); ++fieldSetsIt)
	{
		delete fieldSetsIt->second;
	}
}

int ConfigFile::LoadFile(std::string Filename)
{
	this->Filename = Filename;
	Error = ini_parse(Filename.c_str(), ValueHandler, this);
	return Error;
}

bool ConfigFile::GetString(std::string Section, std::string Name, std::string& Output) const
{
	const std::string key = MakeKey(Section, Name);
	if (Values.count(key))
	{
		auto it = Values.find(key);
		if (it != Values.end())
		{
			Output = (*it).second;
		}
		return 0;
	}
	
	return 1;
}

bool ConfigFile::GetLong(std::string section, std::string name, long& Output) const
{
	std::string Value = "";
	GetString(section, name, Value);
	char* end;
	// This parses "1234" (decimal) and also "0x4D2" (hex)
	long Result = strtol(Value.c_str(), &end, 0);
	if (end > Value.c_str())
	{
		Output = Result;
		return 0;
	}

	return 1;
}

bool ConfigFile::GetDouble(std::string section, std::string name, double& Output) const
{
	std::string Value = "";
	GetString(section, name, Value);
	char* end;
	double Result = strtod(Value.c_str(), &end);
	if (end > Value.c_str())
	{
		Output = Result;
		return 0;
	}

	return 1;
}

bool ConfigFile::GetBoolean(std::string Section, std::string Name, bool& Output) const
{
	std::string Value = "";
	GetString(Section, Name, Value);
	std::transform(Value.begin(), Value.end(), Value.begin(), ::tolower);
	if (Value == "true" || Value == "yes" || Value == "on" || Value == "1")
	{
		Output = true;
		return 0;
	}
	else if (Value == "false" || Value == "no" || Value == "off" || Value == "0")
	{
		Output = false;
		return 0;
	}

	return 1;
}

std::set<std::string> ConfigFile::GetFields(std::string Section) const
{
	std::string sectionKey = Section;
	std::transform(sectionKey.begin(), sectionKey.end(), sectionKey.begin(), ::tolower);
	std::map<std::string, std::set<std::string>*>::const_iterator fieldSetIt = Fields.find(sectionKey);
	if (fieldSetIt == Fields.end())
	{
		return std::set<std::string>();
	}

	return *(fieldSetIt->second);
}

std::string ConfigFile::MakeKey(std::string section, std::string name)
{
	std::string key = section + "=" + name;
	std::transform(key.begin(), key.end(), key.begin(), ::tolower);
	return key;
}

int ConfigFile::ValueHandler(void* user, const char* section, const char* name, const char* value)
{
	ConfigFile* reader = (ConfigFile*)user;

	// Add the value to the lookup map
	std::string key = MakeKey(section, name);
	if (reader->Values[key].size() > 0)
		reader->Values[key] += "\n";
	reader->Values[key] += value;

	// Insert the section in the sections set
	reader->Sections.insert(section);

	// Add the value to the values set
	std::string sectionKey = section;
	std::transform(sectionKey.begin(), sectionKey.end(), sectionKey.begin(), ::tolower);

	std::set<std::string>* fieldsSet;
	std::map<std::string, std::set<std::string>*>::iterator fieldSetIt = reader->Fields.find(sectionKey);
	if (fieldSetIt == reader->Fields.end())
	{
		fieldsSet = new std::set<std::string>();
		reader->Fields.insert(std::pair<std::string, std::set<std::string>*>(sectionKey, fieldsSet));
	}
	else {
		fieldsSet = fieldSetIt->second;
	}
	fieldsSet->insert(name);

	return 1;
}


bool Config::Load(std::string const& Filename)
{
	std::lock_guard<std::mutex> lock(Lock);

	if (Find(Filename))
	{
		return true;
	}

	ConfigFiles.emplace_back();
	int res = ConfigFiles.back().LoadFile(Filename);

	if (res == 0)
	{
		ConfigFiles.back().GetFilename() = Filename;
		return true;
	}
	else
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
		if (i->GetFilename() == Filename)
		{
			i = ConfigFiles.erase(i);
		}
		else
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
		if (i.GetFilename() == Filename)
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
		for (auto& i : ConfigFiles)
		{
			Filenames.push_back(i.GetFilename());
		}
	}
	return Filenames;
}

//deprecated
std::set<std::string> Config::GetKeys(std::string const& Filename) const
{
	std::set<std::string> Keys;
	{
		std::lock_guard<std::mutex> lock(Lock);

		const ConfigFile* File = Find(Filename);
		if (!File)
		{
			return std::set<std::string>();
		}

		return File->GetFields("");
	}

	return Keys;
}

// deprecated
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

		File->GetFields("");
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

	return ConfigFile->GetString(Section.c_str(), Key.c_str(), Value);
}

bool Config::GetBool(std::string const& Section, std::string const& Key, bool& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return false;
	}
	return ConfigFile->GetBoolean(Section.c_str(), Key.c_str(), Value);
}

bool Config::GetLong(std::string const& Section, std::string const& Key, long& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return false;
	}
	return ConfigFile->GetLong(Section.c_str(), Key.c_str(), Value);
}

bool Config::GetInt(std::string const& Section, std::string const& Key, int& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return false;
	}
	long Long;
	bool bResult = ConfigFile->GetLong(Section.c_str(), Key.c_str(), Long);
	if (bResult == 0)
	{
		Value = (int)Long;
	}

	return bResult;
}

bool Config::GetDouble(std::string const& Section, std::string const& Key, double& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return false;
	}
	return ConfigFile->GetDouble(Section.c_str(), Key.c_str(), Value);
}

bool Config::GetFloat(std::string const& Section, std::string const& Key, float& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return false;
	}
	double Double;
	bool bResult = ConfigFile->GetDouble(Section.c_str(), Key.c_str(), Double);
	if (bResult == 0)
	{
		Value = (float)Double;
	}

	return bResult;
}
