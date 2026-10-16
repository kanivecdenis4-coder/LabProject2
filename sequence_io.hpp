#pragma once

#include <string>
#include <vector>

namespace seqio {

	
	std::vector<int> generate(std::size_t n, unsigned seed);


	void save(const std::string& path, const std::vector<int>& data);
	std::vector<int> load(const std::string& path);

}  