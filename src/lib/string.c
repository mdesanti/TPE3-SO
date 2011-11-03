#include "../../include/string.h"

int startsWith(char * source, int length, char * comp) {
	int i = 0;
	while (i < length && source[i] == comp[i]) {
		i++;
	}
	if (i == length)
		return 1;
	return 0;
}

int strcmp(char *s, char *t) {
	int i;
	for (i = 0; s[i] == t[i]; i++) {
		if (s[i] == '\0')
			return 1;
	}
	return 0;
}

char *strcpy(char *from, char *to) {
	int i = 0;
	for (; from[i] != '\0'; ++i) {
		if (from[i] != '\n') {
			to[i] = from[i];
		} else {
			to[i] = '\0';
		}
	}
	to[i] = '\0';
	return to;
}

float toFloat(char* array, int arraySize) {
	int i = 0;
	int negative = 0;
	if (arraySize > 0 && array[i] == '-')
		negative = 1;
	while (i < arraySize && array[i] != '.')
		i++;
	int integer = toInteger(array, i);
	if (integer < 0)
		integer *= -1;
	char decimalArray[INT_MAX_VALUE] = { '\0' };
	int j = 0;
	i++;
	while (j < INT_MAX_VALUE && i < arraySize && array[i] != '\0') {
		decimalArray[j++] = array[i++];
	}
	int decimal = toInteger(decimalArray, j - 1);
	float resInt = integer + 0.0;
	float pot = 1.0;
	while ((--j) > 0) {
		pot *= 10;
	}

	float resDec = decimal / pot;
	if (negative == 1)
		return (resInt + resDec) * -1;
	return resInt + resDec;
}
int toInteger(char* array, int arraySize) {
	int number = 0;
	int pot = 1;
	int i = 0;
	if (array[0] == '-') {
		arraySize--;
		i++;
	}
	int arraySizeAux = arraySize;
	while ((--arraySizeAux) > 0)
		pot *= 10;
	while ((arraySize--) > 0) {
		number += (array[i++] - '0') * pot;
		pot /= 10;
	}
	if (array[0] == '-')
		number *= -1;
	return number;
}

int strlen(char *s) {
	int x = 0;
	while (*s != '\0') {
		x++;
		s++;
	}
	return (x);
}

