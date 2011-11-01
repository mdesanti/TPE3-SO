/*
 * lib.c
 *
 *  Created on: Sep 19, 2011
 *      Author: mdesanti90
 */

int pow(int base, int exp) {
	int i = 1;
	int ans = base;
	if(exp == 0)
		return 1;
	while(i != exp) {
		ans *= base;
		i++;
	}
	return ans;
}
