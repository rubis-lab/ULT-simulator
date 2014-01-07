#pragma once

namespace KF
{
	enum mode
	{
		P = 0,
		PV,
		PVA,
	};
}


class KFArgument
{
public:
	KFArgument()
	{
		mode = KF::PV;
		timeSlot = 50;
		KFMeasError = 0.001;
		KFSystemError = 0.0008;
	}

	KF::mode mode;
	unsigned long timeSlot;		// in ms
	double KFMeasError;
	double KFSystemError;
};
