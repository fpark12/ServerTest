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
#ifndef Core_API
#error "not defined"
#endif

class Core_API ConfigFile
{
public:
	// Construct INIReader and parse given filename. See ini.h for more info
	// about the parsing.
	ConfigFile();
	~ConfigFile();

	// Return the result of ini_parse(), i.e., 0 on success, line number of
	// first error on parse error, or -1 on file open error.
	int LoadFile(std::string Filename);

	bool GetString(std::string Key, std::string& Output) const;
	bool GetLong(std::string Key, long& Output) const;
	bool GetDouble(std::string Key, double& Output) const;
	bool GetBoolean(std::string Key, bool& Output) const;

	// Get a string value from INI file, returning default_value if not found.
	bool GetString(std::string section, std::string name, std::string& OutString) const;

	// Get an integer (long) value from INI file, returning default_value if
	// not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
	bool GetLong(std::string section, std::string name, long& Output) const;

	// Get a real (floating point double) value from INI file, returning
	// default_value if not found or not a valid floating point value
	// according to strtod().
	bool GetDouble(std::string section, std::string name, double& Output) const;

	// Get a boolean value from INI file, returning default_value if not found or if
	// not a valid true/false value. Valid true values are "true", "yes", "on", "1",
	// and valid false values are "false", "no", "off", "0" (not case sensitive).
	bool GetBoolean(std::string section, std::string name, bool& Output) const;

	// Returns all the section names from the INI file, in alphabetical order, but in the
	// original casing
	std::set<std::string> GetSections() const;

	// Returns all the field names from a section in the INI file, in alphabetical order,
	// but in the original casing. Returns an empty set if the field name is unknown
	std::set<std::string> GetFields(std::string section) const;

	std::string GetFilename() const
	{
		return Filename;
	}

private:
	std::string Filename;
	int Error;
	std::map<std::string, std::string> Values;
	std::set<std::string> Sections;
	// Because we want to retain the original casing in _fields, but
	// want lookups to be case-insensitive, we need both _fields and _values
	std::map<std::string, std::set<std::string>*> Fields;
	static std::string MakeKey(std::string section, std::string name);
	static int ValueHandler(void* user, const char* section, const char* name, const char* value);
};

class Core_API Config
{
	DECLARE_SINGLETON(Config);
private:

	std::list<ConfigFile> ConfigFilesList;
	static std::mutex ConfigLock;
	const ConfigFile* Find(std::string const& Filename) const;

public:
	// Method used only for loading main configuration files
	bool Load(std::string const& Filename);
	bool Unload(std::string const& Filename);
	bool Reload(std::string const& Filename);

	const ConfigFile* GetFile(std::string const& Filename);
	std::list<std::string> GetFilenames() const;
	std::list<std::string> GetKeysByString(std::string const& Key, std::string const& Filename) const;
	std::set<std::string> GetKeys(std::string const& Filename) const;

	bool GetString(const std::string& Key, std::string& Value) const;
	bool GetBool(const std::string& Key, bool& Value) const;
	bool GetLong(const std::string& Key, long& Value) const;
	bool GetInt(const std::string& Key, int& Value) const;
	bool GetDouble(const std::string& Key, double& Value) const;
	bool GetFloat(const std::string& Key, float& Value) const;

	bool GetString(std::string const& section, std::string const& Key, std::string& Value, std::string const& Filename) const;
	bool GetBool(std::string const& section, std::string const& Key, bool& Value, std::string const& Filename) const;
	bool GetLong(std::string const& section, std::string const& Key, long& Value, std::string const& Filename) const;
	bool GetInt(std::string const& section, std::string const& Key, int& Value, std::string const& Filename) const;
	bool GetDouble(std::string const& section, std::string const& Key, double& Value, std::string const& Filename) const;
	bool GetFloat(std::string const& section, std::string const& Key, float& Value, std::string const& Filename) const;
};
