#include <gtest/gtest.h>

#include <libkmap/libkmap.hpp>

using namespace libkmap;

TEST(libkmap, Empty)
{
	KMap<4, SolutionType::SUM_OF_PRODUCTS> kmap;
	auto solution = kmap.optimal_solution();

	ASSERT_EQ("0", solution.to_string());
}

TEST(libkmap, Full)
{
	KMap<4, SolutionType::SUM_OF_PRODUCTS> kmap;

	kmap.fill(CellValue::HIGH);

	auto solution = kmap.optimal_solution();

	ASSERT_EQ("1", solution.to_string());
}

TEST(libkmap, DiagonalLeftRight)
{
	KMap<4, SolutionType::SUM_OF_PRODUCTS> kmap = {
		CellValue::HIGH, CellValue::LOW, CellValue::LOW, CellValue::LOW,
		CellValue::LOW, CellValue::HIGH, CellValue::LOW, CellValue::LOW,
		CellValue::LOW, CellValue::LOW, CellValue::HIGH, CellValue::LOW,
		CellValue::LOW, CellValue::LOW, CellValue::LOW, CellValue::HIGH,
	};

	auto solution = kmap.optimal_solution();

	ASSERT_EQ("(A'B'C'D') + (A'BC'D) + (ABCD) + (AB'CD')", solution.to_string());
}

TEST(libkmap, DiagonalRightLeft)
{
	KMap<4, SolutionType::SUM_OF_PRODUCTS> kmap = {
		CellValue::LOW, CellValue::LOW, CellValue::LOW, CellValue::HIGH,
		CellValue::LOW, CellValue::LOW, CellValue::HIGH, CellValue::LOW,
		CellValue::LOW, CellValue::HIGH, CellValue::LOW, CellValue::LOW,
		CellValue::HIGH, CellValue::LOW, CellValue::LOW, CellValue::LOW,
	};

	auto solution = kmap.optimal_solution();

	ASSERT_EQ("(A'B'CD') + (A'BCD) + (ABC'D) + (AB'C'D')", solution.to_string());
}

TEST(libkmap, WedgeLeftRight)
{
	KMap<4, SolutionType::SUM_OF_PRODUCTS> kmap = {
		CellValue::HIGH, CellValue::LOW, CellValue::LOW, CellValue::LOW,
		CellValue::HIGH, CellValue::HIGH, CellValue::LOW, CellValue::LOW,
		CellValue::HIGH, CellValue::HIGH, CellValue::HIGH, CellValue::LOW,
		CellValue::HIGH, CellValue::HIGH, CellValue::HIGH, CellValue::HIGH,
	};

	auto solution = kmap.optimal_solution();

	ASSERT_EQ("(C'D') + (BC') + (AD) + (AB')", solution.to_string());
}

TEST(libkmap, WedgeRightLeft)
{
	KMap<4, SolutionType::SUM_OF_PRODUCTS> kmap = {
		CellValue::LOW, CellValue::LOW, CellValue::LOW, CellValue::HIGH,
		CellValue::LOW, CellValue::LOW, CellValue::HIGH, CellValue::HIGH,
		CellValue::LOW, CellValue::HIGH, CellValue::HIGH, CellValue::HIGH,
		CellValue::HIGH, CellValue::HIGH, CellValue::HIGH, CellValue::HIGH,
	};

	auto solution = kmap.optimal_solution();

	ASSERT_EQ("(CD') + (BC) + (AD) + (AB')", solution.to_string());
}
