#ifndef CAD_CHECKER_HPP
# define CAD_CHECKER_HPP

namespace cad
{
	template <int id>
	struct Var {
		static std::string	value;
	};

	template <int id>
	std::string	Var<id>::value;

	struct NoCheck {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			(void)to_check;
			(void)size;
			return (true);
		}
	};

	template <class Checker, class ... Next>
	struct CheckAnd
	{
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			size_t save = size;
			if (Checker::check(to_check, size) && CheckAnd<Next...>::check(to_check, size)) {
				return (true);
			}
			size = save;
			return (false);
		}
	};

	template <class Checker>
	struct CheckAnd<Checker>
	{
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			return (Checker::check(to_check, size));
		}
	};

	template <class Checker, class ... Next>
	struct CheckOr {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			return (Checker::check(to_check, size) || CheckOr<Next...>::check(to_check, size));
		}
	};

	template <class Checker>
	struct CheckOr<Checker> {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			return (Checker::check(to_check, size));
		}
	};

	template <class Checker, class Next>
	struct CheckNor {
		static bool			check(const std::string& to_check, size_t& size) noexcept {
			size_t sizet1 = 0;
			size_t sizet2 = 0;
			bool res1 = Checker::check(to_check, sizet1);
			bool res2 = Next::check(to_check, sizet2);
			if (res1 && !res2) {
				size += sizet1;
				return (true);
			} else if (!res1 && res2) {
				size += sizet2;
				return (true);
			}
			return (false);
		}
	};

	template <class Checker, int min, int max>
	struct CheckRepeat {
		static inline bool	check(const std::string& to_check, size_t& size) {
			size_t save = size;
			int n = 0;
			while ((max != -1 && n < max) || true) {
				if (!Checker::check(to_check, size))
					break ;
				++n;
			}
			if (n >= min && ((max != -1 && n < max) || true)) {
				return (true);
			}
			size = save;
			return (false);
		}
	};

	template <class Checker>
	struct CheckStar : public CheckRepeat<Checker, 0, -1> {};

	template <class Checker>
	struct CheckPlus : public CheckRepeat<Checker, 1, -1> {};

	template <char c, char ... Next>
	struct CheckCharacter {
		typedef CheckCharacter<c, Next...>	type;
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			size_t save = size;
			if (to_check[size] == c) {
				++size;
				if (!CheckCharacter<Next...>::check(to_check, size)) {
					size = save;
					return (false);
				}
				return (true);
			}
			return (false);
		}
	};

	template <char c>
	struct CheckCharacter<c> {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			if (to_check[size] == c) {
				++size;
				return (true);
			}
			return (false);
		}
	};

	template <char c, char ... Next>
	struct CheckNoneOf {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			if (to_check[size] != c) {
				if (!CheckNoneOf<Next...>::check(to_check, size)) {
					return (false);
				}
				++size;
				return (true);
			}
			return (false);
		}
	};

	template <char c>
	struct CheckNoneOf<c> {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			if (to_check[size] != c)
			{
				++size;
				return (true);
			}
			return (false);
		}
	};

	template <const char (str)[], size_t size_str>
	struct CheckString
	{
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			size_t i;
			for (i = 0; i < size_str && to_check[size + i] == str[i]; ++i);
			if (i == size_str) {
				size += i;
				return (true);
			}
			return (false);
		}
	};

	struct CheckSpace {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			if (std::isspace(to_check[size])) {
				++size;
				return (true);
			}
			return (false);
		}
	};

	struct CheckAny {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			if (to_check[size]) {
				++size;
				return (true);
			}
			return (false);
		}
	};

	struct CheckAlpha {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			if (std::isalpha(to_check[size])) {
				++size;
				return (true);
			}
			return (false);
		}
	};

	struct CheckAlphaNum {
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			if (std::isalpha(to_check[size]) || std::isdigit(to_check[size])) {
				++size;
				return (true);
			}
			return (false);
		}
	};

	template <int id, class Checker>
	struct CheckVar {
		static std::string	value;
		static inline bool	check(const std::string& to_check, size_t& size) noexcept {
			size_t start = size;
			if (Checker::check(to_check, size)) {
				size_t end = size;
				Var<id>::value = to_check.substr(start, end - start);
				return (true);
			}
			return (false);
		}
	};
}

#endif
