#pragma once

#include <iostream>
#include <fstream>

#include <string>
#include <vector>

#include <algorithm>

#include <cassert>

using s8 = int8_t;
using u8 = uint8_t;
using s16 = int16_t;
using u16 = uint16_t;
using s32 = int32_t;
using u32 = uint32_t;
using s64 = int64_t;
using u64 = uint64_t;

using point_t = double;

using text_t = const char*;

template<typename T>
T clamp(const T& min, const T& val, const T& max) {
	if (val < min) {
		return min;
	}
	else if (val > max) {
		return max;
	}
	else {
		return val;
	}
}

template<typename T>
bool is_between(const T& min, const T& val, const T& max) {
	return min <= val && val <= max;
}

// кастует число в строку
std::string cast(s64 num) {
	std::string str;
	if (num == 0) {
		str = "0";
	}
	else {
		bool isNeg = false;
		if (num < 0) {
			isNeg = true;
			num *= -1;
		}
		while (num) {
			str += num % 10 + '0';
			num /= 10;
		}
		if (isNeg) {
			str += '-';
		}
		std::reverse(str.begin(), str.end());
	}
	return str;
}

// округляет число до степени 2
u64 round_two(u64 n) {
	n--;
	n |= n >> 1;
	n |= n >> 2;
	n |= n >> 4;
	n |= n >> 8;
	n |= n >> 16;
	n |= n >> 32;
	return n + 1;
}
