#pragma once

#include <sqlpp11/table.h>
#include <sqlpp11/char_sequence.h>
#include <sqlpp11/column_types.h>

// Requires boost preprocessor

//*
#include <sqlpp11/ppgen.h>
namespace SQLTable
{
	SQLPP_DECLARE_TABLE(
		(tab_person)
		,
		(id     , bigint      , SQLPP_AUTO_INCREMENT)
		(name   , varchar(255), SQLPP_NOT_NULL      )
		(feature, int         , SQLPP_NOT_NULL      )
	);

	SQLPP_DECLARE_TABLE(
		(Users)
		,
		(ID          , bigint      , SQLPP_AUTO_INCREMENT)
		(AccountName , varchar(255), SQLPP_NOT_NULL      )
		(Password    , varchar(255), SQLPP_NOT_NULL      )
		(EmailAddress, varchar(255), SQLPP_NOT_NULL      )
	);


}
//*/

namespace TabFoo_
{
	struct Omega
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "omega";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T omega;
				T& operator()()
				{
					return omega;
				}
				const T& operator()() const
				{
					return omega;
				}
			};
		};
		using _traits = ::sqlpp::make_traits<::sqlpp::bigint>;
	};
}

struct TabFoo : sqlpp::table_t<TabFoo, TabFoo_::Omega>
{
	using _value_type = sqlpp::no_value_t;
	struct _alias_t
	{
		static constexpr const char _literal[] = "tab_foo";
		using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
		template <typename T>
		struct _member_t
		{
			T tabFoo;
			T& operator()()
			{
				return tabFoo;
			}
			const T& operator()() const
			{
				return tabFoo;
			}
		};
	};
};

namespace TabBar_
{
	struct Alpha
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "alpha";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T alpha;
				T& operator()()
				{
					return alpha;
				}
				const T& operator()() const
				{
					return alpha;
				}
			};
		};
		using _traits = sqlpp::
			make_traits<sqlpp::bigint, sqlpp::tag::must_not_insert, sqlpp::tag::must_not_update, sqlpp::tag::can_be_null>;
	};

	struct Beta
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "beta";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T beta;
				T& operator()()
				{
					return beta;
				}
				const T& operator()() const
				{
					return beta;
				}
			};
		};
		using _traits = sqlpp::make_traits<sqlpp::varchar, sqlpp::tag::can_be_null>;
	};

	struct Gamma
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "gamma";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T gamma;
				T& operator()()
				{
					return gamma;
				}
				const T& operator()() const
				{
					return gamma;
				}
			};
		};
		using _traits = sqlpp::make_traits<sqlpp::boolean, sqlpp::tag::require_insert>;
	};

	struct Delta
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "delta";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T delta;
				T& operator()()
				{
					return delta;
				}
				const T& operator()() const
				{
					return delta;
				}
			};
		};
		using _traits = sqlpp::make_traits<sqlpp::integer, sqlpp::tag::can_be_null>;
	};

}

struct TabBar : sqlpp::table_t<TabBar, TabBar_::Alpha, TabBar_::Beta, TabBar_::Gamma, TabBar_::Delta>
{
	struct _alias_t
	{
		static constexpr const char _literal[] = "tab_bar";
		using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
		template <typename T>
		struct _member_t
		{
			T tabBar;
			T& operator()()
			{
				return tabBar;
			}
			const T& operator()() const
			{
				return tabBar;
			}
		};
	};
};


namespace TabSample_
{
	struct Alpha
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "alpha";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T alpha;
				T& operator()()
				{
					return alpha;
				}
				const T& operator()() const
				{
					return alpha;
				}
			};
		};
		using _traits = ::sqlpp::make_traits<::sqlpp::bigint, ::sqlpp::tag::must_not_insert, ::sqlpp::tag::must_not_update>;
	};

	struct Beta
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "beta";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T beta;
				T& operator()()
				{
					return beta;
				}
				const T& operator()() const
				{
					return beta;
				}
			};
		};
		using _traits = ::sqlpp::make_traits<::sqlpp::varchar, ::sqlpp::tag::must_not_update, ::sqlpp::tag::can_be_null>;
	};

	struct Gamma
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "gamma";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T gamma;
				T& operator()()
				{
					return gamma;
				}
				const T& operator()() const
				{
					return gamma;
				}
			};
		};
		using _traits = ::sqlpp::make_traits<::sqlpp::boolean>;
	};
}

struct SampleTable : sqlpp::table_t<SampleTable, TabSample_::Alpha, TabSample_::Beta, TabSample_::Gamma>
{
	using _value_type = sqlpp::no_value_t;
	struct _alias_t
	{
		static constexpr const char _literal[] = "tab_sample";
		using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
		template <typename T>
		struct _member_t
		{
			T tabSample;
			T& operator()()
			{
				return tabSample;
			}
			const T& operator()() const
			{
				return tabSample;
			}
		};
	};
};

namespace TabDateTime_
{
	struct ColDayPoint
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "col_day_point";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T colDayPoint;
				T& operator()()
				{
					return colDayPoint;
				}
				const T& operator()() const
				{
					return colDayPoint;
				}
			};
		};
		using _traits = sqlpp::make_traits<sqlpp::day_point, sqlpp::tag::can_be_null>;
	};
	struct ColTimePoint
	{
		struct _alias_t
		{
			static constexpr const char _literal[] = "col_time_point";
			using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
			template <typename T>
			struct _member_t
			{
				T colTimePoint;
				T& operator()()
				{
					return colTimePoint;
				}
				const T& operator()() const
				{
					return colTimePoint;
				}
			};
		};
		using _traits = sqlpp::make_traits<sqlpp::time_point, sqlpp::tag::can_be_null>;
	};
}

struct TabDateTime : sqlpp::table_t<TabDateTime, TabDateTime_::ColDayPoint, TabDateTime_::ColTimePoint>
{
	struct _alias_t
	{
		static constexpr const char _literal[] = "tab_date_time";
		using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
		template <typename T>
		struct _member_t
		{
			T tabDateTime;
			T& operator()()
			{
				return tabDateTime;
			}
			const T& operator()() const
			{
				return tabDateTime;
			}
		};
	};
};

