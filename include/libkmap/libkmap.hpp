#ifndef LIBKMAP_HPP
#define LIBKMAP_HPP

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <initializer_list>
#include <iterator>
#include <list>
#include <sstream>
#include <string>
#include <vector>

namespace libkmap
{
	typedef unsigned long long index_t;

	enum class CellValue
	{
		DONT_CARE = -1,
		LOW = 0,
		HIGH = 1,
	};

	enum class SolutionType
	{
		SUM_OF_PRODUCTS,
		PRODUCT_OF_SUMS,
	};

	template <index_t variable_count, SolutionType solution_type>
	class KMap
	{
		static_assert(variable_count >= 2, "KMap must have 2 or more variables.");
	private:
		template <index_t size>
		class WrappedIndexIterator
		{
		public:
			WrappedIndexIterator(index_t i) : m_index(i) {}
			WrappedIndexIterator(const WrappedIndexIterator& wrapped_index_iterator) : m_index(wrapped_index_iterator.m_index) {}

			WrappedIndexIterator& operator=(index_t i)
			{
				m_index = i;
				return *this;
			}
			
			WrappedIndexIterator& operator=(int i)
			{
				m_index = i;
				return *this;
			}

			WrappedIndexIterator& operator=(const WrappedIndexIterator& wrapped_index_iterator)
			{
				m_index = wrapped_index_iterator.m_index;
				return *this;
			}

			operator unsigned long long() const
			{
				return m_index % size;
			}

			const WrappedIndexIterator& operator+(const WrappedIndexIterator& wrapped_index_iterator)
			{
				m_index = m_index + wrapped_index_iterator.m_index;
				return *this;
			}

			const WrappedIndexIterator& operator+(index_t i)
			{
				m_index = m_index + i;
				return *this;
			}

			const WrappedIndexIterator& operator+(int i)
			{
				m_index = m_index + i;
				return *this;
			}

			const WrappedIndexIterator& operator-(const WrappedIndexIterator& wrapped_index_iterator)
			{
				m_index = m_index - wrapped_index_iterator.m_index;
				return *this;
			}

			const WrappedIndexIterator& operator-(index_t i)
			{
				m_index = m_index - i;
				return *this;
			}

			const WrappedIndexIterator& operator-(int i)
			{
				m_index = m_index - i;
				return *this;
			}

			WrappedIndexIterator& operator++()
			{
				m_index = m_index + 1;
				return *this;
			}

			WrappedIndexIterator operator++(int)
			{
				WrappedIndexIterator tmp = *this;
				m_index = m_index + 1;
				return tmp;
			}

			WrappedIndexIterator& operator--()
			{
				m_index = m_index - 1;
				return i;
			}

			WrappedIndexIterator operator--(int)
			{
				WrappedIndexIterator tmp = *this;
				m_index = m_index - 1;
				return tmp;
			}

			bool operator==(const WrappedIndexIterator& wrapped_index_iterator) const
			{
				return m_index == wrapped_index_iterator.m_index;
			}

			bool operator!=(const WrappedIndexIterator& wrapped_index_iterator) const
			{
				return m_index != wrapped_index_iterator.m_index;
			}

			bool operator<(const WrappedIndexIterator& wrapped_index_iterator) const
			{
				return m_index < wrapped_index_iterator.m_index;
			}

			bool operator>(const WrappedIndexIterator& wrapped_index_iterator) const
			{
				return m_index > wrapped_index_iterator.m_index;
			}

			bool operator<=(const WrappedIndexIterator& wrapped_index_iterator) const
			{
				return m_index <= wrapped_index_iterator.m_index;
			}

			bool operator>=(const WrappedIndexIterator& wrapped_index_iterator) const
			{
				return m_index >= wrapped_index_iterator.m_index;
			}
		private:
			index_t m_index;
		};

		static constexpr index_t s_row_variable_count = variable_count / 2;
		static constexpr index_t s_column_variable_count = variable_count / 2 + variable_count % 2;
		static constexpr index_t s_row_count = 1 << s_row_variable_count;
		static constexpr index_t s_column_count = 1 << s_column_variable_count;

		typedef WrappedIndexIterator<s_row_count> RowIndexIterator;
		typedef WrappedIndexIterator<s_column_count> ColumnIndexIterator;

		CellValue m_data[s_row_count][s_column_count] = {};

		std::string m_variable_names;

		static constexpr char s_default_variable_names[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

	public:
		KMap()
		{
			static_assert(variable_count < sizeof(s_default_variable_names), "Variable count is too large for the default variable names list; please use the value constructor.");
			for (index_t i = 0; i < variable_count; ++i)
			{
				m_variable_names.push_back(s_default_variable_names[i]);
			}
		}

		KMap(const std::initializer_list<CellValue>& init_list)
		{
			static_assert(variable_count < sizeof(s_default_variable_names), "Variable count is too large for the default variable names list; please use the value constructor.");
			assert(init_list.size() == s_row_count * s_column_count && "Number of values in initializer list does not match KMap dimensions.");

			for (index_t i = 0; i < variable_count; ++i)
			{
				m_variable_names.push_back(s_default_variable_names[i]);
			}

			std::copy(init_list.begin(), init_list.end(), reinterpret_cast<CellValue*>(&m_data[0][0]));
		}

		KMap(const std::string& variable_names)
			: m_variable_names(variable_names)
		{
			static_assert(variable_count == variable_names.length(), "Variable count must match number of variable names");
		}

		KMap(const std::string& variable_names, const std::initializer_list<CellValue>& init_list)
			: m_variable_names(variable_names)
		{
			static_assert(variable_count == variable_names.length(), "Variable count must match number of variable names");
			assert(init_list.size() == s_row_count * s_column_count && "Number of values in initializer list does not match KMap dimensions.");

			std::copy(init_list.begin(), init_list.end(), reinterpret_cast<CellValue*>(&m_data[0][0]));
		}

		KMap(const KMap& kmap)
			: m_variable_names(kmap.m_variable_names)
		{
			std::memcpy(m_data, kmap.m_data, sizeof(m_data));
		}

		class Point
		{
		public:
			Point(index_t i, index_t j) : m_i(i), m_j(j) {}

			index_t i() const { return m_i; }
			index_t j() const { return m_j; }

			bool operator==(const Point& point) const
			{
				return m_i == point.m_i && m_j == point.m_j;
			}
			
			bool operator!=(const Point& point) const
			{
				return m_i != point.m_i || m_j != point.m_j;
			}
		private:
			index_t m_i;
			index_t m_j;
		};

		class Term
		{
		public:
			Term(const Point& top_left, const Point& bottom_right)
				: m_top_left(top_left), m_bottom_right(bottom_right)
			{
				m_width = m_bottom_right.j() - m_top_left.j() + 1;
				m_height = m_bottom_right.i() - m_top_left.i() + 1;
			}

			const Point& top_left() const { return m_top_left; }
			const Point& bottom_right() const { return m_bottom_right; }

			index_t width() const { return m_width; }
			index_t height() const { return m_height; }
			index_t area() const { return width() * height(); }

			index_t gates_required() const
			{
				index_t row_gates = s_row_variable_count - static_cast<index_t>(std::log2l(static_cast<long double>(height())));
				index_t column_gates = s_column_variable_count - static_cast<index_t>(std::log2l(static_cast<long double>(width())));

				return row_gates + column_gates;
			}

			RowIndexIterator row_begin() const
			{
				return m_top_left.i();
			}

			RowIndexIterator row_end() const
			{
				return row_begin() + height();
			}

			RowIndexIterator column_begin() const
			{
				return m_top_left.j();
			}

			RowIndexIterator column_end() const
			{
				return column_begin() + width();
			}

			Point begin() const
			{
				return Point(row_begin(), column_begin());
			}

			Point end() const
			{
				return Point(row_end(), column_end());
			}

			bool contains(const Point& point) const
			{
				if (m_bottom_right.i() >= m_top_left.i())
				{
					if (point.i() < m_top_left.i() || point.i() > m_bottom_right.i())
					{
						return false;
					}
				}
				else
				{
					if (point.i() < m_top_left.i() && point.i() > m_bottom_right.i())
					{
						return false;
					}
				}

				if (m_bottom_right.j() >= m_top_left.j())
				{
					if (point.j() < m_top_left.j() || point.j() > m_bottom_right.j())
					{
						return false;
					}
				}
				else
				{
					if (point.j() < m_top_left.j() && point.j() > m_bottom_right.j())
					{
						return false;
					}
				}

				return true;
			}

			bool contains(const Term& term) const
			{
				return contains(term.top_left()) && contains(term.bottom_right());
			}
		private:
			Point m_top_left, m_bottom_right;
			index_t m_width, m_height;
		};

		class Solution
		{
		public:
			Solution(const std::vector<Term>& terms, const std::string& variable_names) : m_terms(terms), m_variable_names(variable_names) {}

			const std::vector<Term>& terms() const { return m_terms; }

			index_t gate_count() const
			{
				index_t gates_for_terms = m_terms.size() - 1;

				for (const Term& term : m_terms)
				{
					gates_for_terms += term.gates_required();
				}

				return gates_for_terms;
			}

			std::string to_string() const
			{
				if (m_terms.empty()) return "0";
				if (m_terms.size() == 1 && m_terms.front().height() == s_row_count && m_terms.front().width() == s_column_count) return "1";

				std::ostringstream str;

				for (const Term& term : m_terms)
				{
					str << "(";

					index_t rows = (1 << s_row_variable_count) - 1;
					index_t cols = (1 << s_column_variable_count) - 1;
					index_t rowsNeg = rows;
					index_t colsNeg = cols;

					for (RowIndexIterator i = term.row_begin(); i != term.row_end(); ++i)
					{
						for (ColumnIndexIterator j = term.column_begin(); j != term.column_end(); ++j)
						{
							index_t grey_code_i = binary_to_grey_code(i);
							index_t grey_code_j = binary_to_grey_code(j);

							rows &= grey_code_i;
							cols &= grey_code_j;
							rowsNeg &= ~grey_code_i;
							colsNeg &= ~grey_code_j;
						}
					}

					for (index_t i = 0; i < s_row_variable_count; ++i)
					{
						if (rows & (1ULL << (s_row_variable_count - i - 1)))
						{
							str << m_variable_names[i];
						}
						else if (rowsNeg & (1ULL << (s_row_variable_count - i - 1)))
						{
							str << m_variable_names[i] << "'";
						}
					}

					for (index_t i = 0; i < s_column_variable_count; ++i)
					{
						if (cols & (1ULL << (s_column_variable_count - i - 1)))
						{
							str << m_variable_names[i + s_row_variable_count];
						}
						else if (colsNeg & (1ULL << (s_column_variable_count - i - 1)))
						{
							str << m_variable_names[i + s_row_variable_count] << "'";
						}
					}

					str << ") + ";
				}

				return str.str().substr(0, str.str().size() - 3);
			}
		private:
			std::vector<Term> m_terms;
			std::string m_variable_names;

			index_t binary_to_grey_code(index_t binary) const
			{
				index_t grey_code = 0;
				index_t i = 0;

				while (binary != 0)
				{
					bool a = binary & 1ULL;
					binary >>= 1;
					bool b = binary & 1ULL;

					if (a ^ b)
					{
						grey_code = grey_code | (1ULL << i);
					}

					++i;
				}

				return grey_code;
			}
		};

		CellValue& data(index_t row, index_t column)
		{
			return m_data[row][column];
		}

		RowIndexIterator row_begin() const
		{
			return 0;
		}

		RowIndexIterator row_end() const
		{
			return s_row_count;
		}

		RowIndexIterator column_begin() const
		{
			return 0;
		}

		RowIndexIterator column_end() const
		{
			return s_column_count;
		}

		template <class UnaryFunction>
		UnaryFunction for_each(UnaryFunction f)
		{
			for (auto i = row_begin(); i != row_end(); ++i)
			{
				for (auto j = column_begin(); j != column_end(); ++j)
				{
					f(i, j, data(i, j));
				}
			}
			return f;
		}

		template <class UnaryFunction>
		UnaryFunction for_each(RowIndexIterator row_begin, RowIndexIterator row_end, ColumnIndexIterator column_begin, ColumnIndexIterator column_end, UnaryFunction f)
		{
			for (auto i = row_begin; i != row_end; ++i)
			{
				for (auto j = column_begin; j != column_end; ++j)
				{
					f(i, j, data(i, j));
				}
			}
			return f;
		}

		void fill(RowIndexIterator row_begin, RowIndexIterator row_end, ColumnIndexIterator column_begin, ColumnIndexIterator column_end, CellValue cell_value)
		{
			for_each(row_begin, row_end, column_begin, column_end, [&](const RowIndexIterator& i, const ColumnIndexIterator& j, CellValue& value)
				{
					value = cell_value;
				});
		}

		void fill(CellValue cell_value)
		{
			for_each([&](const RowIndexIterator& i, const ColumnIndexIterator& j, CellValue& value)
			{
				value = cell_value;
			});
		}

		Point find(RowIndexIterator row_begin, RowIndexIterator row_end, ColumnIndexIterator column_begin, ColumnIndexIterator column_end, CellValue cell_value)
		{
			for (auto i = row_begin; i != row_end; ++i)
			{
				for (auto j = column_begin; j != column_end; ++j)
				{
					if (data(i, j) == cell_value) return Point(i, j);
				}
			}

			return Point(row_end, column_end);
		}

		bool is_term_valid(const Term& term)
		{
			if ((term.height() == 0) || (term.width() == 0) ||
				(term.height() & (term.height() - 1)) || (term.width() & (term.width() - 1)) || // enforce is a power of 2
				(term.height() == s_row_count && term.top_left().i() != 0) || (term.width() == s_column_count && term.top_left().j() != 0)) // enforce ends dont touch
			{
				return false;
			}

			// enforce no low values in term
			if (find(term.row_begin(), term.row_end(), term.column_begin(), term.column_end(), CellValue::LOW) != term.end())
			{
				return false;
			}

			return true;
		}

		index_t high_count() const
		{
			index_t high_count = 0;

			for_each([&](const RowIndexIterator& i, const ColumnIndexIterator& j, CellValue& value)
			{
				if (value == CellValue::HIGH)
				{
					++high_count;
				}
			});

			return high_count;
		}

		bool is_solution_valid(const std::vector<Term>& solution)
		{
			std::vector<Point> points = high_points();

			bool found = false;

			for (const Point& point : points)
			{
				for (const Term& term : solution)
				{
					if (term.contains(point))
					{
						found = true;
						break;
					}
				}

				if (found)
				{
					found = false;
				}
				else
				{
					return false;
				}
			}

			return true;
		}

		index_t gates_required(const std::vector<Term>& solution)
		{
			index_t gates_for_terms = solution.size() - 1;

			for (const Term& term : solution)
			{
				gates_for_terms += term.gates_required();
			}

			return gates_for_terms;
		}

		std::vector<Point> high_points()
		{
			std::vector<Point> points;

			for_each([&](const RowIndexIterator& i, const ColumnIndexIterator& j, CellValue& value)
			{
				if (value == CellValue::HIGH)
				{
					points.push_back(Point(i, j));
				}
			});

			return points;
		}

		std::vector<Point> valid_points()
		{
			std::vector<Point> points;

			for_each([&](const RowIndexIterator& i, const ColumnIndexIterator& j, CellValue& value)
			{
				if (value == CellValue::HIGH || value == CellValue::DONT_CARE)
				{
					points.push_back(Point(i, j));
				}
			});

			return points;
		}

		std::vector<Term> valid_terms()
		{
			std::vector<Point> points = valid_points();

			std::vector<Term> valid_terms;

			for (const Point& pointA : points)
			{
				for (const Point& pointB : points)
				{
					Term term(pointA, pointB);
					if (is_term_valid(term))
					{
						valid_terms.push_back(term);
					}
				}
			}

			return valid_terms;
		}

		std::vector<Term> cull_redundant_terms(const std::vector<Term>& terms)
		{
			std::list<Term> terms_list(terms.begin(), terms.end());

			for (auto itA = terms_list.begin(); itA != terms_list.end(); ++itA)
			{
				for (auto itB = terms_list.begin(); itB != terms_list.end();)
				{
					if (itA != itB && (*itA).contains(*itB))
					{
						terms_list.erase(itB++);
					}
					else
					{
						++itB;
					}
				}
			}

			return std::vector<Term>(terms_list.begin(), terms_list.end());
		}

		Solution optimal_solution()
		{
			std::vector<Term> useful_terms = cull_redundant_terms(valid_terms());
			
			std::vector<Term> minimal;
			index_t minimal_gates = 0;

			index_t set_size = useful_terms.size();
			index_t power_set_size = 1ULL << set_size;

			std::vector<Term> solution;

			for (index_t counter = 1; counter < power_set_size; ++counter)
			{
				for (index_t j = 0; j < set_size; ++j)
				{
					if (counter & (1ULL << j))
					{
						solution.push_back(useful_terms[j]);
					}
				}

				if (is_solution_valid(solution))
				{
					index_t solution_gates = gates_required(solution);

					if (solution_gates < minimal_gates || minimal.empty())
					{
						minimal = solution;
						minimal_gates = solution_gates;
					}
				}

				solution.clear();
			}

			return Solution(minimal, m_variable_names);
		}
	};

	template <index_t variable_count, SolutionType solution_type>
	class System
	{
	private:
		using KMap = KMap<variable_count, solution_type>;

		std::vector<KMap> m_kmaps;
	public:
		System() {}
		System(const std::vector<KMap>& kmaps) : m_kmaps(kmaps) {}

		void add(const KMap& kmap)
		{
			m_kmaps.push_back(kmap);
		}

		class Solution
		{
		public:
			Solution(const std::vector<typename KMap::Solution>& solutions) : m_solutions(solutions) {}

			const std::vector<typename KMap::Solution>& solutions() const { return m_solutions; }
		private:
			std::vector<typename KMap::Solution> m_solutions;
		};

		Solution optimal_solution() const
		{

		}
	};
}

#endif // !LIBKMAP_HPP
