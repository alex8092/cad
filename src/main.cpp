#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "Checker.hpp"

namespace user
{
	struct CheckWord {
		static bool			check(const std::string& to_check, size_t& size) noexcept {
			size_t s = 0;
			while (std::isalpha(to_check[size + s]) || std::isdigit(to_check[size + s]) || to_check[size + s] == '_' || to_check[size + s] == '.' || to_check[size + s] == '/')
				++s;
			if (s > 0) {
				size += s;
				return (true);
			}
			return (false);
		}
	};

	struct CheckPath {
		static bool			check(const std::string& to_check, size_t& size) noexcept {
			size_t save = size;
			while (CheckWord::check(to_check, size) || to_check[size] == '+')
				++size;
			return (save != size);
		}
	};

	namespace preproc
	{
		static constexpr const char keyword_define[] = "define";
		static constexpr const char keyword_include[] = "include";
	}

	typedef cad::CheckAnd	<
							cad::CheckCharacter<'#'>,
							cad::CheckStar<cad::CheckSpace>,
							cad::CheckOr<
										cad::CheckAnd	<
														cad::CheckString<preproc::keyword_include, sizeof(preproc::keyword_include) - 1>,
														cad::CheckPlus<cad::CheckSpace>,
														cad::CheckOr<
																	cad::CheckAnd	<
																					cad::CheckCharacter<'<'>,
																					cad::CheckVar<0, CheckWord>,
																					cad::CheckCharacter<'>'>
																					>,
																	cad::CheckAnd	<
																					cad::CheckCharacter<'"'>,
																					cad::CheckVar<0, CheckWord>,
																					cad::CheckCharacter<'"'>
																					>
																	>
														>
										>

							>
							checkerPreprocessor;

	typedef cad::CheckAnd 	<
							cad::CheckCharacter<'#'>,
							cad::CheckRepeat<cad::CheckSpace,0, -1>,
							cad::CheckString<preproc::keyword_define, sizeof(preproc::keyword_define) - 1>,
							cad::CheckRepeat<cad::CheckSpace, 1, -1>,
							cad::CheckVar<0, cad::CheckRepeat<cad::CheckOr<cad::CheckCharacter<'_'>, cad::CheckAlphaNum>, 1, -1> >,
							cad::CheckRepeat<cad::CheckSpace, 1, -1>,
							cad::CheckVar<1, cad::CheckRepeat<cad::CheckAny, 0, -1> >
							>
							checkerPreprocDefine;

	typedef cad::CheckAnd	<
							cad::CheckCharacter<'-'>,
							cad::CheckCharacter<'I'>,
							cad::CheckVar<0, CheckPath>
							>
							checkerPathInclude;
}

static std::vector<std::string>		paths;
static std::map<std::string, bool>	_file_parsed;

void	parse_file(const std::string& filename) {

	std::vector<std::string>	lines;
	std::string 				line;
	std::ifstream				ifs(filename);
	if (!ifs.is_open()) {
		bool open = false;
		for (auto it : paths) {
			if (_file_parsed.find(it + "/" + filename) != _file_parsed.end())
				return ;
			ifs.open(it + "/" + filename);
			if (ifs.is_open()) {
				_file_parsed[it + "/" + filename] = true;
				open = true;
				break ;
			}
		}
		if (!open) {
			std::cerr << "Cannot open file: " << filename << std::endl;
			return ;
		}
	} else {
		if (_file_parsed.find(filename) != _file_parsed.end())
			return ;
		_file_parsed[filename] = true;
	}
	if (ifs.is_open()) {
		// std::cout << "open and parse: " << filename << std::endl;
		while (std::getline(ifs, line)) {
			size_t size = 0;
			if (user::checkerPreprocessor::check(line, size)) {
				// std::cout << "preprocessor include : [" << cad::Var<0>::value << "]" << std::endl;
				parse_file(cad::Var<0>::value);
			} else if (user::checkerPreprocDefine::check(line, size)) {
				std::cout << "preprocessor define [" << cad::Var<0>::value << "] = [" << cad::Var<1>::value << "]" << std::endl;
			}
			lines.push_back(line);
		}
		ifs.close();
	}
}

int	main(int ac, char **av)
{
	if (ac >= 2) {
		paths.push_back("./");
		int i = 1;
		size_t size = 0;
		for (; user::checkerPathInclude::check(av[i], size); ++i) {
			paths.push_back(cad::Var<0>::value);
			size = 0;
		}
		for (; i < ac; ++i) {
			parse_file(av[i]);
		}
		for (auto it : paths) {
			std::cout << "path to find [" << it << "]" << std::endl;
		}
		for (auto it : _file_parsed) {
			// std::cout << "parsed file [" << it.first << "]" << std::endl;
		}
	} else {
		std::cerr << "usage: " << av[0] << "[-Ipath ...] <file ...>" << std::endl;
		return (1);
	}
	return (0);
}