////////////////////////////////
// RTMaps SDK Component header
////////////////////////////////

#ifndef _Maps_RandomCANFrameGenerator_H
#define _Maps_RandomCANFrameGenerator_H

// Includes maps sdk library header
#include "maps.hpp"

// Declares a new MAPSComponent child class
class MAPSRandomCANFrameGenerator : public MAPSComponent 
{
	// Use standard header definition macro
	MAPS_COMPONENT_STANDARD_HEADER_CODE(MAPSRandomCANFrameGenerator)

	static void SpeedUp(MAPSModule *module, int actionNb);
	static void SpeedDown(MAPSModule *module, int actionNb);

	MAPSDelay	GetPeriod();
	void		SetPeriod(MAPSDelay newPeriod);

private :
	// Place here your specific methods and attributes

	MAPSMutex	m_periodMtx;
	MAPSDelay	m_period;
};

#endif
