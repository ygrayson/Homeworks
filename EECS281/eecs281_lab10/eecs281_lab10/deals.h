#ifndef DEALS_H
#define DEALS_H

// EECS 281 LAB 10: DYNAMIC PROGRAMMING

// Your favorite restaurant has a customer loyalty program. It works like this: whenever you buy a
// meal, you can add one holepunch to your punchcard. Once you have 5 punches, you can turn in the
// card for a free meal (and a new, unmarked punchcard).

// For example, if your meals cost [3, 3, 3, 3, 3, 3, 3, 120] then you should earn hole punches
// from the first 5 meals ($15), pay normally for the next two, and then turn in the punchcard
// so that the $120 meal is free! The total cost would be $19 (or with coupons [see below] $17).

// However, you ALSO have a lot of coupons to this restaurant. In fact, you have enough coupons
// that you can apply one to any and every meal! If you apply a coupon, you get a 25% discount
// on that meal. HOWEVER, you don't get to add a holepunch to your card if you use a coupon!

// For example, if your meals cost [2, 2, 2, 2, 1000, 100], then if you use the first 5 meals to
// earn holepunches, you'll need to spend $1008. Then you'll get $100 free. It would be much better
// to just apply the 25% discount to each item, so that you pay a total of $829.

// There are, however, many cases where it makes sense to use a mixture of punchcard discounts and
// discounting coupons. This is where your program comes in!

// You will be given a list of meal prices. For the given meal prices, find the LEAST POSSIBLE
// cost needed to pay for all the meals using the holepunch loyalty program and coupons.

// Notes/Clarifications/Hints:
// * use the "discounted" function below to compute discount
//    (so that you round the same way as the instructor solution).
// * you should always EITHER use a coupon, or apply the punchcard
// * you have an unlimited number of coupons
// * your program should be able to run in linear time.
// * greedy solutions will not work
// * use dynamic programming

#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>


// This is a type synonym.
using cost = long long;

// How much you pay for a discounted (25% off) meal.
cost discounted(cost full_price) {
    return full_price * 3 / 4;
}


cost best_price(const std::vector<cost>& prices) {
    // DONE: put your code here
	std::vector<std::vector<cost>> cost_table(prices.size(), std::vector<cost>(6, std::numeric_limits<cost>::max()));

	cost_table[0][0] = discounted(prices[0]);
	cost_table[0][1] = prices[0];

	for (std::size_t i = 1; i < cost_table.size(); ++i) {
		cost price = prices[i];
		//before having enough punches
		if (i < 5) {
			cost_table[i][0] = cost_table[i - 1][0] + discounted(price);
			for (std::size_t j = 1; j <= i; ++j) {
				cost_table[i][j] = std::min(cost_table[i - 1][j - 1] + price, 
											cost_table[i - 1][j] + discounted(price));
			}
			cost_table[i][i + 1] = cost_table[i - 1][i] + price;
		}
		//after collecting enough punches
		else {
			cost_table[i][0] = std::min(cost_table[i - 1][0] + discounted(price), cost_table[i - 1][5]);
			for (std::size_t j = 1; j < 6; ++j) {
				cost_table[i][j] = std::min(cost_table[i - 1][j - 1] + price, 
											cost_table[i - 1][j] + discounted(price));
			}
		}
	}

	return *std::min_element(cost_table.back().begin(), cost_table.back().end());


}

#endif
