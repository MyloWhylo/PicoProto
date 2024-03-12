#pragma once
#include <stdlib.h>

template <size_t memory, class type>
MinFilter<memory, type>::MinFilter() {
	// Don't really have a lot to do here
	for (int ii = 0; ii < memory; ii++) {
		this->values[ii] = 0;
	}
}

template <size_t memory, class type>
type MinFilter<memory, type>::Filter(type sample) {
	this->values[this->writeIndex++] = sample;  // write the next sample
	this->writeIndex %= memory;                 // but make sure to loop around

	type minVal = values[0];                  // Starting minval is the first sample
	for (size_t ii = 1; ii < memory; ii++) {  // Loop through the values to find lower ones
		if (values[ii] < minVal) {
			minVal = values[ii];
		}
	}

	return minVal;  // Return that
}