#pragma once

#include <string>
#include "yaml-cpp/yaml.h"

using namespace std;

class config_t {
public:
	int16_t n;
	int64_t repeats;
	string output_path;
	string log_path;
};

namespace YAML {
	template<>
	struct convert<config_t> {
		static Node encode(const config_t &rhs) 
		{
			Node node;

			node["n"          ] = rhs.n;
			node["repeats"    ] = rhs.repeats;
			node["output_path"] = rhs.output_path;
			node["log_path"   ] = rhs.log_path;

			return node;
		}

		static bool decode(const Node &node, config_t &rhs) 
		{
			if (!node.IsMap() || node.size() != 4) 
			{
				return false;
			}

			rhs.n           = node["n"          ].as<int16_t>();
			rhs.repeats     = node["repeats"    ].as<int64_t>();
			rhs.output_path = node["output_path"].as<string>();
			rhs.log_path    = node["log_path"   ].as<string>();

			if (rhs.n < 1) {
				return false;
			}

			if (rhs.repeats < 1) {
				return false;
			}

			if (rhs.output_path.length() < 1) {
				return false;
			}

			if (rhs.log_path.length() < 1) {
				return false;
			}

			return true;
		}
	};
}

/*
Example

// YAML testing
YAML::Node node;
node = YAML::LoadFile("test.json");
config_t test = node.as<config_t>();
*/