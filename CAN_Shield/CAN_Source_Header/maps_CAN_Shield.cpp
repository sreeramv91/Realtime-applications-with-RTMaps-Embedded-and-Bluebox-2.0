////////////////////////////////
// RTMaps SDK Component
////////////////////////////////

////////////////////////////////
// Purpose of this module :
////////////////////////////////

#include "maps_CAN_Shield.h"	// Includes the header of this component

// Use the macros to declare the inputs
MAPS_BEGIN_INPUTS_DEFINITION(MAPSCAN_Filter)
	MAPS_INPUT("iCAN", MAPS::FilterCANFrame, MAPS::FifoReader)
MAPS_END_INPUTS_DEFINITION

// Use the macros to declare the outputs
MAPS_BEGIN_OUTPUTS_DEFINITION(MAPSCAN_Filter)
	MAPS_OUTPUT("oCAN1", MAPS::CANFrame, NULL, NULL, 1)
MAPS_END_OUTPUTS_DEFINITION

// Use the macros to declare the properties
MAPS_BEGIN_PROPERTIES_DEFINITION(MAPSCAN_Filter)
MAPS_END_PROPERTIES_DEFINITION

// Use the macros to declare the actions
MAPS_BEGIN_ACTIONS_DEFINITION(MAPSCAN_Filter)
MAPS_END_ACTIONS_DEFINITION

// Use the macros to declare this component (CAN_Filter) behaviour
MAPS_COMPONENT_DEFINITION(MAPSCAN_Filter,"CAN_Shield","1.0",128,
			  MAPS::Threaded,MAPS::Threaded,
			  -1, // Nb of inputs. Leave -1 to use the number of declared input definitions
			   1, // Nb of outputs. Leave -1 to use the number of declared output definitions
			   0, // Nb of properties. Leave -1 to use the number of declared property definitions
			   0) // Nb of actions. Leave -1 to use the number of declared action definitions

//Initialization: Birth() will be called once at diagram execution startup.			  
void MAPSCAN_Filter::Birth()
{
}


void MAPSCAN_Filter::Core() 
{
   
	MAPSIOElt *iIOEltCAN = StartReading(Input("iCAN"));
	// Always check a pointer returned by RTMaps
	if (NULL == iIOEltCAN)
		return;
	// It's Ok, the MAPSIOElt returned is valid

	// Get the array of frames encapsulated in the MAPSIOElt
	MAPSCANFrame *receivedFrame = &(iIOEltCAN->CANFrame());
	// Declare a MAPSStreamedString data to display the frames content in the Studio Console
	MAPSStreamedString str;
	// Display the number of frames included in the MAPSIOElt and the common timestamp of these frames
	str << iIOEltCAN->VectorSize() << " frame(s) received at timestamp : " << (int)(iIOEltCAN->Timestamp() / 1000) << " ms";
	// For all frames
	for (int i = 0; i<iIOEltCAN->VectorSize(); i++)
	{
		str << '\n';
		str << "Id : ";
		// Display the identifier (Extended or Standard)
		if (receivedFrame[i].arbitrationId&MAPSCANFrame::ExtendedId)
		{
			str << "Ignoring as it is Extended CAN";
			break;
		}
		else
		{
			MAPSIOElt *ioEltOutput1;
			ioEltOutput1 = StartWriting(Output("oCAN1"));
			MAPSCANFrame &frame1 = ioEltOutput1->CANFrame();
			frame1 = *receivedFrame;
			StopWriting(ioEltOutput1);
		}
	}
	// Send the string to the console as an information
	ReportInfo(str);
}

//De-initialization: Death() will be called once at diagram execution shutdown.
void MAPSCAN_Filter::Death()
{
  
}
