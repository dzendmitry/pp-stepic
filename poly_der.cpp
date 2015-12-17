#include <iostream>
#include <string>
#include <map>

std::string derivative(std::string polynomial) {
	std::map<size_t, int> der_map;
	size_t pos = 0;
	while (pos != std::string::npos) {
		std::string unit;
		size_t found_pos = polynomial.find_first_of("+-", pos);
		if (found_pos == std::string::npos) {
			if (!pos) {
				unit = polynomial.substr(pos);	
			} else {
				unit = polynomial.substr(pos - 1);
			}
			pos = found_pos;
		} else {
			if (!pos) {
				unit = polynomial.substr(pos, found_pos - pos);
			} else {
				unit = polynomial.substr(pos - 1, found_pos - pos + 1);
			}
			pos = found_pos + 1;
		}
		//std::cout << unit << std::endl;

		/* derivative of const is 0 */
		if (unit.find('x') == std::string::npos) {
			continue;
		}

		/* find pow */
		size_t pow = 1;
		size_t pow_pos = 0;
		if ((pow_pos = unit.find('^')) != std::string::npos) {
			pow = std::stoul(unit.substr(pow_pos+1));
		}

		/* find coeficient */
		int coef = 1;
		if (unit[0] == '-') {
			coef = -1;
		}
		if (unit.find('*') != std::string::npos) {
			coef = std::stoi(unit);
		}

		auto it = der_map.find(pow);
		if (it != der_map.end()) {
			it->second += coef;
			continue;
		}
		der_map.insert(std::pair<size_t, int>(pow, coef));
	}
	std::string res_poly;
	for (auto it = der_map.rbegin(); it != der_map.rend(); it++) {
		//std::cout << it->first << " -> " << it->second << std::endl;
		if (!it->second) {
			continue;
		}

		/* coeficient */
		int coef = it->first * it->second;
		if (!res_poly.length()) {
			res_poly += std::to_string(coef);
		} else {
			if (coef > 0) {
				res_poly += "+";
			}
			res_poly += std::to_string(coef);
		}

		/* pow */
		size_t pow = it->first - 1;
		if (!pow) {
			continue;
		} else if (pow == 1) {
			res_poly += "*x";
		} else {
			res_poly += "*x^";
			res_poly += std::to_string(pow);
		}
	}
    return res_poly;
}

int main(int argc, char* argv[]) {
	std::string polynomial;
	std::cin >> polynomial;
	std::cout << derivative(polynomial) << std::endl;
	return 0;
}