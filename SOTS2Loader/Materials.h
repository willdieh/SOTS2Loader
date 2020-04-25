#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct Material
{
	std::string materialName;
	std::string materialType;
	std::unordered_map<std::string, std::string> settings;
};


struct  MaterialDictionary
{
	bool Load(std::string& filename, std::string& baseInstallDir);
	void Print();

	// Kerberos specific header stuff (seems useless)
	std::string						kerbLeader;			// "SHYL"
	int								kerbSerialNumber;	// INT? UINT? Two CHARs?
	std::string						kerbType;			// "MaterialDictionary", etc
		
	std::vector<Material>			materials;
};

