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
#include "Public/Detail/Config.h"
#include "ini.h"


std::mutex Config::ConfigLock;

//Config& GConfigRef = Config::Instance();


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

	this->Filename = Filename.substr(0, Filename.find_first_of("."));
	Error = ini_parse(Filename.c_str(), ValueHandler, this);
	return Error;
}

Status ConfigFile::GetString(std::string Key, std::string& Output) const
{
	if (Values.count(Key))
	{
		auto it = Values.find(Key);
		if (it != Values.end())
		{
			Output = (*it).second;
		}
		return Status::OK;
	}

	return Status::FAILED;
}

Status ConfigFile::GetLong(std::string Key, long& Output) const
{
	std::string Value = "";
	GetString(Key, Value);
	char* end;
	// This parses "1234" (decimal) and also "0x4D2" (hex)
	long Result = strtol(Value.c_str(), &end, 0);
	if (end > Value.c_str())
	{
		Output = Result;
		return Status::OK;
	}

	return Status::FAILED;
}
Status ConfigFile::GetDouble(std::string Key, double& Output) const
{
	std::string Value = "";
	GetString(Key, Value);
	char* end;
	double Result = strtod(Value.c_str(), &end);
	if (end > Value.c_str())
	{
		Output = Result;
		return Status::OK;
	}

	return Status::FAILED;
}

Status ConfigFile::GetBoolean(std::string Key, bool& Output) const
{
	std::string Value = "";
	GetString(Key, Value);
	std::transform(Value.begin(), Value.end(), Value.begin(), ::tolower);
	if (Value == "true" || Value == "yes" || Value == "on" || Value == "1")
	{
		Output = true;
		return Status::OK;
	}
	else if (Value == "false" || Value == "no" || Value == "off" || Value == "0")
	{
		Output = false;
		return Status::OK;
	}

	return Status::FAILED;
}

Status ConfigFile::GetString(std::string Section, std::string Name, std::string& Output) const
{
	const std::string key = MakeKey(Section, Name);
	if (Values.count(key))
	{
		auto it = Values.find(key);
		if (it != Values.end())
		{
			Output = (*it).second;
		}
		return Status::OK;
	}

	return Status::FAILED;
}

Status ConfigFile::GetLong(std::string section, std::string name, long& Output) const
{
	std::string Value = "";
	GetString(section, name, Value);
	char* end;
	// This parses "1234" (decimal) and also "0x4D2" (hex)
	long Result = strtol(Value.c_str(), &end, 0);
	if (end > Value.c_str())
	{
		Output = Result;
		return Status::OK;
	}

	return Status::FAILED;
}

Status ConfigFile::GetDouble(std::string section, std::string name, double& Output) const
{
	std::string Value = "";
	GetString(section, name, Value);
	char* end;
	double Result = strtod(Value.c_str(), &end);
	if (end > Value.c_str())
	{
		Output = Result;
		return Status::OK;
	}

	return Status::FAILED;
}

Status ConfigFile::GetBoolean(std::string Section, std::string Name, bool& Output) const
{
	std::string Value = "";
	GetString(Section, Name, Value);
	std::transform(Value.begin(), Value.end(), Value.begin(), ::tolower);
	if (Value == "true" || Value == "yes" || Value == "on" || Value == "1")
	{
		Output = true;
		return Status::OK;
	}
	else if (Value == "false" || Value == "no" || Value == "off" || Value == "0")
	{
		Output = false;
		return Status::OK;
	}

	return Status::FAILED;
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
	std::string key = section + "." + name;
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


Status Config::Load(std::string const& Filename)
{
	std::lock_guard<std::mutex> lock(ConfigLock);

	if (Find(Filename))
	{
		return SC::OK;
	}

	ConfigFilesList.emplace_back();
	int res = ConfigFilesList.back().LoadFile(Filename);

	if (res == 0)
	{
		ConfigFilesList.back().GetFilename() = Filename;
		return SC::OK;
	}
	else
	{
		ConfigFilesList.pop_back();
		return SC::FAILED;
	}
}

Status Config::Unload(std::string const& Filename)
{
	auto i = std::begin(ConfigFilesList);

	while (i != std::end(ConfigFilesList))
	{
		if (i->GetFilename() == Filename)
		{
			i = ConfigFilesList.erase(i);
		}
		else
		{
			++i;
		}
	}

	return SC::OK;
}

Status Config::Reload(std::string const& Filename)
{
	if (SC::OK == Unload(Filename))
	{
		return Load(Filename);
	}

	return SC::FAILED;
}

const ConfigFile* Config::GetFile(std::string const& Filename)
{
	for (const auto& i : ConfigFilesList)
	{
		if (i.GetFilename() == Filename)
		{
			return &i;
		}
	}

	return nullptr;
}

const ConfigFile* Config::Find(std::string const& Filename) const
{
	for (const auto& i : ConfigFilesList)
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
		std::lock_guard<std::mutex> lock(ConfigLock);
		for (auto& i : ConfigFilesList)
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
		std::lock_guard<std::mutex> lock(ConfigLock);

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
		std::lock_guard<std::mutex> lock(ConfigLock);

		const ConfigFile* File = Find(Filename);
		if (!File)
		{
			return Keys;
		}

		File->GetFields("");
	}

	return Keys;
}

Status Config::GetString(const std::string& Key, std::string& Value) const
{
	size_t FilenameEnd = Key.find_first_of(".", 0);
	std::string Filename = Key.substr(0, FilenameEnd);
	std::string Subkey = Key.substr(FilenameEnd + 1);
	std::transform(Subkey.begin(), Subkey.end(), Subkey.begin(), ::tolower);

	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}

	return ConfigFile->GetString(Subkey, Value);
}

Status Config::GetBool(const std::string& Key, bool& Value) const
{
	size_t FilenameEnd = Key.find_first_of(".", 0);
	std::string Filename = Key.substr(0, FilenameEnd);
	std::string Subkey = Key.substr(FilenameEnd + 1);
	std::transform(Subkey.begin(), Subkey.end(), Subkey.begin(), ::tolower);

	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	return ConfigFile->GetBoolean(Subkey, Value);
}

Status Config::GetLong(const std::string& Key, long& Value) const
{
	size_t FilenameEnd = Key.find_first_of(".", 0);
	std::string Filename = Key.substr(0, FilenameEnd);
	std::string Subkey = Key.substr(FilenameEnd + 1);
	std::transform(Subkey.begin(), Subkey.end(), Subkey.begin(), ::tolower);

	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	return ConfigFile->GetLong(Subkey, Value);
}

Status Config::GetUInt(const std::string& Key, uint32& Value) const
{
	size_t FilenameEnd = Key.find_first_of(".", 0);
	std::string Filename = Key.substr(0, FilenameEnd);
	std::string Subkey = Key.substr(FilenameEnd + 1);
	std::transform(Subkey.begin(), Subkey.end(), Subkey.begin(), ::tolower);

	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	long Long;
	Status bResult = ConfigFile->GetLong(Subkey, Long);
	if (bResult == Status::OK)
	{
		Value = (uint32)Long;
	}

	return bResult;
}

Status Config::GetInt(const std::string& Key, int& Value) const
{
	size_t FilenameEnd = Key.find_first_of(".", 0);
	std::string Filename = Key.substr(0, FilenameEnd);
	std::string Subkey = Key.substr(FilenameEnd + 1);
	std::transform(Subkey.begin(), Subkey.end(), Subkey.begin(), ::tolower);

	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	long Long;
	Status bResult = ConfigFile->GetLong(Subkey, Long);
	if (bResult == Status::OK)
	{
		Value = (int)Long;
	}

	return bResult;
}

Status Config::GetDouble(const std::string& Key, double& Value) const
{
	size_t FilenameEnd = Key.find_first_of(".", 0);
	std::string Filename = Key.substr(0, FilenameEnd);
	std::string Subkey = Key.substr(FilenameEnd + 1);
	std::transform(Subkey.begin(), Subkey.end(), Subkey.begin(), ::tolower);

	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	return ConfigFile->GetDouble(Subkey, Value);
}

Status Config::GetFloat(const std::string& Key, float& Value) const
{
	size_t FilenameEnd = Key.find_first_of(".", 0);
	std::string Filename = Key.substr(0, FilenameEnd);
	std::string Subkey = Key.substr(FilenameEnd + 1);
	std::transform(Subkey.begin(), Subkey.end(), Subkey.begin(), ::tolower);

	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	double Double;
	Status bResult = ConfigFile->GetDouble(Subkey, Double);
	if (bResult == Status::OK)
	{
		Value = (float)Double;
	}

	return bResult;
}

// deprecated
Status Config::GetString(std::string const& Section, std::string const& Key, std::string& Value, std::string const& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}

	return ConfigFile->GetString(Section.c_str(), Key.c_str(), Value);
}

Status Config::GetBool(std::string const& Section, std::string const& Key, bool& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	return ConfigFile->GetBoolean(Section.c_str(), Key.c_str(), Value);
}

Status Config::GetLong(std::string const& Section, std::string const& Key, long& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	return ConfigFile->GetLong(Section.c_str(), Key.c_str(), Value);
}

Status Config::GetUInt(std::string const& section, std::string const& Key, uint32& Value, std::string const& Filename) const
{
	return Status::FAILED;
}

Status Config::GetInt(std::string const& Section, std::string const& Key, int& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	long Long;
	Status bResult = ConfigFile->GetLong(Section.c_str(), Key.c_str(), Long);
	if (bResult == Status::OK)
	{
		Value = (int)Long;
	}

	return bResult;
}

Status Config::GetDouble(std::string const& Section, std::string const& Key, double& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	return ConfigFile->GetDouble(Section.c_str(), Key.c_str(), Value);
}

Status Config::GetFloat(std::string const& Section, std::string const& Key, float& Value, const std::string& Filename) const
{
	const ConfigFile* ConfigFile = this->Find(Filename);
	if (!ConfigFile)
	{
		return Status::FAILED;
	}
	double Double;
	Status bResult = ConfigFile->GetDouble(Section.c_str(), Key.c_str(), Double);
	if (bResult == Status::OK)
	{
		Value = (float)Double;
	}

	return bResult;
}
