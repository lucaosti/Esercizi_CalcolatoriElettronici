/* vim: set syntax=cpp :*/
namespace std {
	template<typename T1, typename T2>
	struct is_same {
		static constexpr bool value = false;
	};
	template <typename T>
	struct is_same<T,T> {
		static constexpr bool value = true;
	};
	template<typename T1, typename T2>
	static bool const is_same_v = is_same<T1,T2>::value;

	template<bool T>
	struct enable_if {
	};

	template<>
	struct enable_if<true> {
		using type = void;
	};

	template<bool b>
	using enable_if_t = typename enable_if<b>::type;
}
