#pragma once

// Finite Impulse Response filter
class FirFilter
{
public:
	FirFilter(int length, float* coeff)
		: _length(length),
		  _coeff(coeff)
	{
		// allocate storage for the current and past input samples.
		_input = new float[_length];

		// initialize input history to 0
		for (unsigned int i = 0; i < _length; i++)
		{
			_input[i] = 0.0f;
		}
	}

	~FirFilter()
	{
		delete _input;
	}

	/* Apply the filter filter on the given input and return the filtered output. */
	float filter(float sample)
	{
		float result = 0.0f;

		_input[0] = sample;		// index 0 is the newest input.


		// This performs multiply and accumulate.It does not reverse the input; it is assumed
		// to be reversed, i.e.index 0 is the most recent sample.

		for (unsigned int i = 0; i < _length; i++)
		{
			result += _input[i] * _coeff[i];
		}

		// Shift values in the given vector to right by 1 position.
		// After the shift, index 0 is considered 'free'.
		for (unsigned int i = _length - 1; i > 0; i--)
		{
			_input[i] = _input[i - 1];
		}

		return result;
	}

	/* Clear filter history. Coefficients are untouched. */
	void clear()
	{
		for (int i = 0; i < _length; i++)
		{
			_input[i] = 0.0f;
		}
	}

private:
	float* _input;		// current and past input samples
	int _length;		// number of filter coefficients
	float* _coeff;		// filter coefficients
};


