////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_RandomCANFrameGenerator.h"	// Includes the header of this component
#include <stdlib.h>							// for rand()
// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSRandomCANFrameGenerator)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSRandomCANFrameGenerator)
	MAPS_OUTPUT("oCAN1", MAPS::CANFrame, NULL, NULL, 1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSRandomCANFrameGenerator)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSRandomCANFrameGenerator)
	MAPS_ACTION("aSpeedUp", MAPSRandomCANFrameGenerator::SpeedUp)
	MAPS_ACTION("aSpeedDown", MAPSRandomCANFrameGenerator::SpeedDown)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (RandomCANFrameGenerator) behaviour
MAPS_COMPONENT_DEFINITION(MAPSRandomCANFrameGenerator,"RandomCANFrameGenerator","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			   0, // Nb of inputs. Leave -1 to use the number of declared input definitions
			   1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			   0, // Nb of properties. Leave -1 to use the number of declared property definitions
			   2) // Nb of actions. Leave -1 to use the number of declared action definitions

void MAPSRandomCANFrameGenerator::SpeedUp(MAPSModule *module, int /*actionNb*/)
{
				   // The function is static. The 'module' pointer provided as an argument
				   // enables to call non-static functions or access non-static member variables
				   // of the component the action was triggered from.

				   if (MAPS::IsRunning())
				   {
					   // This action is only supported in run mode.
					   MAPSRandomCANFrameGenerator &comp = *(MAPSRandomCANFrameGenerator *)module;
					   // Decrease the period
					   comp.SetPeriod(comp.GetPeriod() / 2);
				   }
}

void MAPSRandomCANFrameGenerator::SpeedDown(MAPSModule *module, int /*actionNb*/)
{
	// The function is static. The 'module' pointer provided as an argument
	// enables to call non-static functions or access non-static member variables
	// of the component the action was triggered from.

	if (MAPS::IsRunning())
	{
		// This action is only supported in run mode.
		MAPSRandomCANFrameGenerator &comp = *(MAPSRandomCANFrameGenerator *)module;
		// Increase the period
		comp.SetPeriod(comp.GetPeriod() * 2);
	}
}

MAPSDelay MAPSRandomCANFrameGenerator::GetPeriod()
{
	// GetPeriod wand SetPeriod will be called by concurrent
	// threads so we have to protect them with a mutex.
	MAPSMutexGuard	g(m_periodMtx);

	return m_period;
}

void MAPSRandomCANFrameGenerator::SetPeriod(MAPSDelay newPeriod)
{
	if (newPeriod>0)
	{
		// GetPeriod wand SetPeriod will be called by concurrent
		// threads so we have to protect them with a mutex.
		MAPSMutexGuard	g(m_periodMtx);

		m_period = newPeriod;
		MAPSStreamedString sx;
		sx << "Period is now " << m_period << " microseconds.";
		ReportInfo(sx);
	}
	else
		ReportWarning("Period change failed");
}

void MAPSRandomCANFrameGenerator::Birth()
{
	SetPeriod(1000000);
}

//ATTENTION: 
//	Make sure there is ONE and ONLY ONE blocking function inside this Core method.
//	Consider that Core() will be called inside an infinite loop while the diagram is executing.
//	Something similar to: 
//		while (componentIsRunning) {Core();}
//
//	Usually, the one and only blocking function is one of the following:
//		* StartReading(MAPSInput& input); //Data request on a single BLOCKING input. A "blocking input" is an input declared as FifoReader, LastOrNextReader, Wait4NextReader or NeverskippingReader (declaration happens in MAPS_INPUT: see the beginning of this file). A SamplingReader input is non-blocking: StartReading will not block with a SamplingReader input.
//		* StartReading(int nCount, MAPSInput* inputs[], int* inputThatAnswered, int nCountEvents = 0, MAPSEvent* events[] = NULL); //Data request on several BLOCKING inputs.
//		* SynchroStartReading(int nb, MAPSInput** inputs, MAPSIOElt** IOElts, MAPSInt64 synchroTolerance = 0, MAPSEvent* abortEvent = NULL); // Synchronized reading - waiting for samples with same or nearly same timestamps on several BLOCKING inputs.
//		* Wait(MAPSTimestamp t); or Rest(MAPSDelay d); or MAPS::Sleep(MAPSDelay d); //Pauses the current thread for some time. Can be used for instance in conjunction with StartReading on a SamplingReader input (in which case StartReading is not blocking).
//		* Any blocking grabbing function or other data reception function from another API (device driver,etc.). In such case, make sure this function cannot block forever otherwise it could freeze RTMaps when shutting down diagram.
//**************************************************************************/
//	In case of no blocking function inside the Core, your component will consume 100% of a CPU.
//  Remember that the StartReading function used with an input declared as a SamplingReader is not blocking.
//	In case of two or more blocking functions inside the Core, this is likely to induce synchronization issues and data loss. (Ex: don't call two successive StartReading on FifoReader inputs.)
/***************************************************************************/
void MAPSRandomCANFrameGenerator::Core()
{
	
	Rest(GetPeriod());
	ReportInfo("tic");
	MAPSInt64 pId1 = rand() % 100 + 500;	// random value for identifier

	bool pExt1 = FALSE;
	if (pId1%2)
		pExt1 = FALSE;
	else
		pExt1 = TRUE;
	
	// First sample of CAN frame writing : write a single CAN frame to output oCAN1
	// Declare a new MAPSIOElt
	MAPSIOElt *ioEltOutput1;
	// Start writing a new data on oCAN1 output, get the corresponding MAPSIOElt
	ioEltOutput1 = StartWriting(Output("oCAN1"));

	// We get the frame where to write the CAN data
	MAPSCANFrame &frame1 = ioEltOutput1->CANFrame();
	// Set the frame identifier (adress) on the CAN bus
	frame1.arbitrationId = (long)(pId1 | (pExt1 ? MAPSCANFrame::ExtendedId : MAPSCANFrame::StandardId));

	// This is not a remote frame
	frame1.isRemote = false;
	// Set the frame data length
	frame1.dataLength = 1;
	// Set the data content
	frame1.data[0] = 254;
	// Set the vector size to 1 (number of CAN frames contained in this MAPSIOElt). This is optionnal here
	// because the output has been defined with a maximum vector size of 1
	ioEltOutput1->VectorSize() = 1;
	// Send the corresponding CAN frame now !
	StopWriting(ioEltOutput1);
	Rest(GetPeriod());
	ReportInfo("toc");
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSRandomCANFrameGenerator::Death()
{
   
}
