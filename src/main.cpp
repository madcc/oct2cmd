/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2013/10/15 15:36:08
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  madcc09 (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include	<stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "circleBuf.h"
#include "commandDefine.h"

circleBuf cmdBuf;
int cmdCnt=0;
FILE *fw;
int fileData;
unsigned char fileDataBuf[512];
void decodeCmd();
bool processExtruderCommandPacket();
uint8_t pop8();
int16_t pop16(); 
int32_t pop32(); 
#define Debug_Data_Cmd

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
	FILE *fp;
	uint8_t hasNextByte=1;
	printf("ok\n");
	circleBufInit(&cmdBuf);
	if(argc>1)
		fp=fopen(argv[1],"r");
	else
		fp=fopen("homeAxes.txt","r");
	if(!fp)
	{
		printf("open error!\n");
		return 0;
	}
	fw=fopen("cmd.txt","w");
	if(!fw)
	{
		printf("open write file error!\n");
		return 0;
	}
	while(1)
	{
		while((circleBufGetRemainCap(&cmdBuf) > 0) && hasNextByte)
		{
			if(!feof(fp))
			{
				fscanf(fp,"%d\n",&fileData);
				if(fileData<0 || fileData >255)
				{
					printf("data out of scope\n");
					fclose(fp);
					fclose(fw);
					exit(0);
				}
				fileDataBuf[0]=(uint8_t)fileData;
				circleBufPush(&cmdBuf,fileDataBuf[0]);
			}
			else
			{
				hasNextByte=0;
			}
		}
		decodeCmd();
		if(cmdBuf.length==0 && !hasNextByte)
			break;
	}
	fclose(fp);
	fclose(fw);
	return 0;
}				/* ----------  end of function main  ---------- */

void decodeCmd()
{

	int pt[4];//toFix
	int32_t a ; 
	int32_t b ; 
	int32_t dda; 
	uint8_t axes ;
	uint8_t i;
	uint8_t currentToolIndex;
	// process next command on the queue.
	cmdCnt++;
	fprintf(fw,"c %6d :\t",cmdCnt);
	if (cmdBuf.length>0)
	{
		uint8_t command = *circleBufGetData(&cmdBuf,0);//command_buffer[0];
		if (command == HOST_CMD_QUEUE_POINT_ABS) 
		{
			// check for completion
			if (cmdBuf.length >= 17) 
			{
				circleBufPop(&cmdBuf); // remove the command code
				//cmdMode = MOVING;
				pt[0] = pop32();
				pt[1] = pop32();
				pt[2] = pop32();
				dda = pop32();
				//motorSetTarget(pt,dda);//steppers::setTarget(Point(x,y,z),dda);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_QUEUE_POINT_ABS\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_QUEUE_POINT_ABS ");
				fprintf(fw,"%d %d %d %d\n",pt[0],pt[1],pt[2],dda);
#endif
			}
		} else if (command == HOST_CMD_QUEUE_POINT_EXT) 
		{
			// check for completion
			if (cmdBuf.length >= 25) 
			{
				circleBufPop(&cmdBuf); // remove the command code
				//cmdMode = MOVING;
				pt[0] = pop32();
				pt[1]= pop32();
				pt[2]= pop32();
				pt[3]= pop32();
				b= pop32();
				dda = pop32();
				//motorSetTarget(pt,dda);//toFix
				//steppers::setTarget(Point(x,y,z,a,b),dda);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_QUEUE_POINT_EXT\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_QUEUE_POINT_EXT ");
				fprintf(fw,"%d %d %d %d %d\n",pt[0],pt[1],pt[2],pt[3],dda);
#endif
			}
		} else if (command == HOST_CMD_QUEUE_POINT_NEW) 
		{
			// check for completion
			if (cmdBuf.length >= 26) 
			{
				uint8_t relative ;
				int32_t us; 
				circleBufPop(&cmdBuf); // remove the command code
				//cmdMode = MOVING;
				pt[0] = pop32();
				pt[1]= pop32();
				pt[2]= pop32();
				//					if(pt[0]>5000)
				//						pt[0]=5000;
				//					if(pt[0]<-5000)
				//						pt[0]=-5000;
				//					if(pt[1]>2000)
				//						pt[1]=2000;
				//					if(pt[1]<-2000)
				//						pt[1]=-2000;
//#define PT2_THRESHOLD 40000
//				if(pt[2]>PT2_THRESHOLD)
//					pt[2]=PT2_THRESHOLD;
//				if(pt[2]<-PT2_THRESHOLD)
//					pt[2]=-PT2_THRESHOLD;
				pt[3]= pop32();
				b= pop32();
				us = pop32();
				relative = pop8();
				//motorSetTargetNew(pt,us,relative);
				//steppers::setTargetNew(Point(x,y,z,a,b),us,relative);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_QUEUE_POINT_NEW ");
				fprintf(fw,"%d %d %d %d %d %d\n",pt[0],pt[1],pt[2],pt[3],us,relative);
#endif
#ifdef Debug_Data_Cmd
//				if(endStopFlagTmpDebug)
//					fprintf(fw,"x\n");
				fprintf(fw,"HOST_CMD_QUEUE_POINT_NEW ");
				fprintf(fw,"%d %d %d %d %d %d %d\n",pt[0],pt[1],pt[2],pt[3],us,relative,b);
#endif
			}
		} else if (command == HOST_CMD_CHANGE_TOOL) 
		{
			if (cmdBuf.length >= 2) 
			{
				circleBufPop(&cmdBuf); // remove the command code
				currentToolIndex = circleBufPop(&cmdBuf);
				//toFix
				//steppers::changeToolIndex(currentToolIndex);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_CHANGE_TOOL\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_CHANGE_TOOL\n");
#endif
			}
		} else if (command == HOST_CMD_ENABLE_AXES) 
		{
			if (cmdBuf.length >= 2) 
			{
				bool enable;
				circleBufPop(&cmdBuf); // remove the command code
				axes = circleBufPop(&cmdBuf);
				enable = (axes & 0x80) != 0;
//				for (i = 0; i < STEPPER_COUNT; i++) 
//				{
//					if ((axes & (1<<i)) != 0) 
//						//steppers::enableAxis(i, enable);
//						axisSetEnablePin(i,enable);
//				}
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_ENABLE_AXES\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_ENABLE_AXES ");
				fprintf(fw,"%d\n",axes);
#endif
			}
		} else if (command == HOST_CMD_SET_POSITION) 
		{
			// check for completion
			if (cmdBuf.length >= 13) 
			{
				circleBufPop(&cmdBuf); // remove the command code
				pt[0] = pop32();
				pt[1]= pop32();
				pt[2] = pop32();//toFix,motorDefinePosition 怎么知道pt有没有第四个参数，会怎么对pt[3]处理？
				//steppers::definePosition(Point(x,y,z));
				//motorDefinePosition(pt);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_SET_POSITION\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_SET_POSITION ");
				fprintf(fw,"%d %d %d\n",pt[0],pt[1],pt[2]);
#endif
			}
		} else if (command == HOST_CMD_SET_POSITION_EXT) 
		{
			// check for completion
			if (cmdBuf.length >= 21) 
			{
				circleBufPop(&cmdBuf); // remove the command code
				pt[0] = pop32();
				pt[1] = pop32();
				pt[2] = pop32();
				pt[3] = pop32();
				//a = pop32();
				b = pop32();
				//motorDefinePosition(pt);//toFix
				//steppers::definePosition(Point(x,y,z,a,b));
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_SET_POSITION_EXT\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_SET_POSITION_EXT ");
				fprintf(fw,"%d %d %d %d\n",pt[0],pt[1],pt[2],pt[3]);
#endif
			}
		} else if (command == HOST_CMD_DELAY) 
		{
			if (cmdBuf.length >= 5) 
			{
				uint32_t microseconds ;
				//cmdMode = DELAY;
				circleBufPop(&cmdBuf); // remove the command code
				// parameter is in milliseconds; timeouts need microseconds
				microseconds = pop32() * 1000;
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_DELAY\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_DELAY\n");
#endif
			}
		} else if (command == HOST_CMD_PAUSE_FOR_BUTTON) 
		{
			if (cmdBuf.length >= 5) 
			{
				uint16_t timeout_seconds;
				uint8_t button_mask;
				uint8_t button_timeout_behavior;
				circleBufPop(&cmdBuf); // remove the command code
				button_mask = circleBufPop(&cmdBuf);
				timeout_seconds = pop16();
				button_timeout_behavior = circleBufPop(&cmdBuf);
				//					if (timeout_seconds != 0) 
				//						button_wait_timeout.start(timeout_seconds * 1000L * 1000L);
				//					else 
				//						button_wait_timeout = Timeout();

				// set button wait via interface board
				//Motherboard::getBoard().interfaceBlink(25,15);
				//					InterfaceBoard& ib = Motherboard::getBoard().getInterfaceBoard();
				//					ib.waitForButton(button_mask);
				//					cmdMode = WAIT_ON_BUTTON;//toFix
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_PAUSE_FOR_BUTTON\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_PAUSE_FOR_BUTTON\n");
#endif
			}
		} else if (command == HOST_CMD_DISPLAY_MESSAGE) 
		{
			//	MessageScreen* scr = Motherboard::getBoard().getMessageScreen();
			if (cmdBuf.length >= 6) 
			{
				uint8_t options ;
				uint8_t xpos ;
				uint8_t ypos ;
				uint8_t timeout_seconds;
				uint8_t idx=0;
				char msg[100];
				circleBufPop(&cmdBuf); // remove the command code
				options = circleBufPop(&cmdBuf);
				xpos = circleBufPop(&cmdBuf);
				ypos = circleBufPop(&cmdBuf);
				timeout_seconds = circleBufPop(&cmdBuf);
				do{
					msg[idx++]=circleBufPop(&cmdBuf);
				}
				while(msg[idx-1]!='\0');
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_DISPLAY_MESSAGE\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_DISPLAY_MESSAGE: %d %d %d %s\n",options,xpos,ypos,msg);
#endif
			}

		} else if (command == HOST_CMD_FIND_AXES_MINIMUM ||
				command == HOST_CMD_FIND_AXES_MAXIMUM) 
		{
			if (cmdBuf.length >= 8) 
			{
				uint8_t flags ;
				uint32_t feedrate ;
				uint16_t timeout_s ;
				bool direction ;
				circleBufPop(&cmdBuf); // remove the command
				flags = pop8();
				feedrate = pop32(); // feedrate in us per step
				timeout_s = pop16();
				direction = command == HOST_CMD_FIND_AXES_MAXIMUM;
				//cmdMode = HOMING;
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_FIND_AXES_MINIMUM \n");
#endif
#ifdef Debug_Data_Cmd
				if(command == HOST_CMD_FIND_AXES_MINIMUM)
					fprintf(fw,"HOST_CMD_FIND_AXES_MINIMUM %d %d %d %d\n",command,flags,feedrate,timeout_s);
				else
					fprintf(fw,"HOST_CMD_FIND_AXES_MAXIMUM %d %d %d %d\n",command,flags,feedrate,timeout_s);
#endif
			}
		} else if (command == HOST_CMD_WAIT_FOR_TOOL) 
		{
			if (cmdBuf.length >= 6) 
			{
				uint16_t toolPingDelay ;
				uint16_t toolTimeout ;
				//cmdMode = WAIT_ON_TOOL;
				circleBufPop(&cmdBuf);
				currentToolIndex = circleBufPop(&cmdBuf);
				toolPingDelay = (uint16_t)pop16();
				toolTimeout = (uint16_t)pop16();
				//tool_wait_timeout.start(toolTimeout*1000000L);//toFix
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_WAIT_FOR_TOOL\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_WAIT_FOR_TOOL\n");
#endif
			}
		} else if (command == HOST_CMD_WAIT_FOR_PLATFORM) 
		{
			// FIXME: Almost equivalent to WAIT_FOR_TOOL
			if (cmdBuf.length >= 6) 
			{
				uint8_t currentToolIndex ;
				uint16_t toolPingDelay ;
				uint16_t toolTimeout ;
				//cmdMode = WAIT_ON_PLATFORM;
				circleBufPop(&cmdBuf);
				currentToolIndex = circleBufPop(&cmdBuf);
				toolPingDelay = (uint16_t)pop16();
				toolTimeout = (uint16_t)pop16();
				//tool_wait_timeout.start(toolTimeout*1000000L);//toFix
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_WAIT_FOR_PLATFORM\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_WAIT_FOR_PLATFORM\n");
#endif
			}
		} else if (command == HOST_CMD_STORE_HOME_POSITION) 
		{

			// check for completion
			if (cmdBuf.length >= 2) 
			{
				uint16_t offset ;
				uint32_t position ;
				circleBufPop(&cmdBuf);
				axes = pop8();
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_STORE_HOME_POSITION\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_STORE_HOME_POSITION\n");
#endif
			}
		} 
		else if (command == HOST_CMD_RECALL_HOME_POSITION) 
		{
			// check for completion
			if (cmdBuf.length >= 2) 
			{
				uint16_t offset ;
				circleBufPop(&cmdBuf);
				axes = pop8();

				//					Point newPoint = steppers::getPosition();
				//
				//					for (i = 0; i < STEPPER_COUNT; i++) {
				//						if ( axes & (1 << i) ) {
				//							offset = eeprom_offsets::AXIS_HOME_POSITIONS + 4*i;
				//							cli();
				//							eeprom_read_block(&(newPoint[i]), (void*) offset, 4);
				//							sei();
				//						}
				//					}
				//
				//					steppers::definePosition(newPoint);
				pt[0]=0;
				pt[1]=3000;
				pt[2]=0;
				pt[3]=0;
				//motorDefinePosition(pt);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_RECALL_HOME_POSITION\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_RECALL_HOME_POSITION\n");
#endif
			}

		}else if (command == HOST_CMD_SET_POT_VALUE)
		{
			if (cmdBuf.length >= 2) 
			{
				uint8_t axis ;
				uint8_t value ;
				circleBufPop(&cmdBuf); // remove the command code
				axis = pop8();
				value = pop8();
				//					steppers::setAxisPotValue(axis, value);
				//					steppers::setAxisPotValue(axis, value);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_SET_POT_VALUE\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_SET_POT_VALUE,%d %d\n",axis,value);
#endif
			}
		}else if (command == HOST_CMD_SET_RGB_LED)
		{
			if (cmdBuf.length >= 2) 
			{
				uint8_t red  ;
				uint8_t green ;
				uint8_t blue ;
				uint8_t blink_rate ;
				uint8_t effect;
				circleBufPop(&cmdBuf); // remove the command code

				red = pop8();
				green = pop8();
				blue = pop8();
				blink_rate = pop8();

				effect = pop8();

				//RGB_LED::setLEDBlink(blink_rate);
				//RGB_LED::setCustomColor(red, green, blue);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_SET_RGB_LED\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_SET_RGB_LED\n");
#endif

			}
		}else if (command == HOST_CMD_SET_BEEP)
		{
			if (cmdBuf.length >= 2)
			{
				uint8_t frequency;
				uint8_t beep_length ;
				uint8_t effect ;
				circleBufPop(&cmdBuf); // remove the command code
				frequency= pop16();
				beep_length = pop16();
				effect = pop8();
				//Piezo::setTone(frequency, beep_length);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_SET_BEEP\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_SET_BEEP\n");
#endif
			}			
		}else if (command == HOST_CMD_TOOL_COMMAND) 
		{
			if (cmdBuf.length >= 4) 
			{ // needs a payload
				uint8_t payload_length =*circleBufGetData(&cmdBuf,3);// command_buffer[3];
				if (cmdBuf.length >= 4+payload_length) 
				{
					circleBufPop(&cmdBuf); // remove the command code
					processExtruderCommandPacket(); 
#ifdef Debug_Cmd
					fprintf(fw,"HOST_CMD_TOOL_COMMAND\n");
#endif
#ifdef Debug_Data_Cmd
					fprintf(fw,"HOST_CMD_TOOL_COMMAND\n");
#endif
				}
			}
		}
		else if (command == HOST_CMD_SET_BUILD_PERCENT)
		{
			if (cmdBuf.length >= 2)
			{
				uint8_t percent ;
				uint8_t ignore ;
				circleBufPop(&cmdBuf); // remove the command code
				percent = pop8();
				ignore = pop8(); // remove the reserved byte
				//interface::setBuildPercentage(percent);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_SET_BUILD_PERCENT\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_SET_BUILD_PERCENT\n");
#endif
			}
		} else if (command == HOST_CMD_QUEUE_SONG ) //queue a song for playing
		{
			/// Error tone is 0,
			/// End tone is 1,
			/// all other tones user-defined (defaults to end-tone)
			if (cmdBuf.length >= 2)
			{
				uint8_t songId;
				circleBufPop(&cmdBuf); // remove the command code
				songId = pop8();
				//					if(songId == 0)
				//						Piezo::errorTone(4);
				//					else if (songId == 1 )
				//						Piezo::doneTone();
				//					else
				//						Piezo::errorTone(2);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_QUEUE_SONG \n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_QUEUE_SONG \n");
#endif
			}

		} else if ( command == HOST_CMD_RESET_TO_FACTORY) 
		{
			/// reset EEPROM settings to the factory value. Reboot bot.
			if (cmdBuf.length >= 1)
			{
				uint8_t options ;
				circleBufPop(&cmdBuf); // remove the command code
				options = pop8();
				//eeprom::factoryResetEEPROM();
				//Motherboard::getBoard().reset(false);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_RESET_TO_FACTORY\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_RESET_TO_FACTORY\n");
#endif
			}
		} else if ( command == HOST_CMD_BUILD_START_NOTIFICATION) 
		{
			if (cmdBuf.length >= 1)
			{
				int buildSteps ;
				int idx=0;
				char newName[50];
				circleBufPop(&cmdBuf); // remove the command code
				buildSteps = pop32();
				//hostHandleBuildStartNotification(&cmdBuf);
				//弹出build Name : RepG Build
				do{
					newName[idx++]=circleBufPop(&cmdBuf);
				}
				while(newName[idx-1]!='\0');
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_BUILD_START_NOTIFICATION\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_BUILD_START_NOTIFICATION: %s\n",newName);
#endif
			}
		} else if ( command == HOST_CMD_BUILD_END_NOTIFICATION) 
		{
			if (cmdBuf.length >= 1)
			{
				uint8_t flags ;
				circleBufPop(&cmdBuf); // remove the command code
				flags = circleBufPop(&cmdBuf);
				//hostHandleBuildStopNotification(flags);
#ifdef Debug_Cmd
				fprintf(fw,"HOST_CMD_BUILD_END_NOTIFICATION\n");
#endif
#ifdef Debug_Data_Cmd
				fprintf(fw,"HOST_CMD_BUILD_END_NOTIFICATION\n");
#endif
			}

		} else 
		{
			//错误指令，或者指令 已经乱了，或未知指令，直接pop？
			fprintf(fw,"error instruct; %d\n",command);
			circleBufPop(&cmdBuf); // remove the command code
		}
	}
}
bool processExtruderCommandPacket()/*{{{*/
	{
        uint8_t	id = circleBufPop(&cmdBuf);
		uint8_t command = circleBufPop(&cmdBuf);
		uint8_t length = circleBufPop(&cmdBuf);
//		uint16_t temp;
		
		int32_t x = 0;
        int32_t y = 0;
        int32_t z = 0;
        int32_t a = 0;
        int32_t b = 0;
        int32_t us = 1000000;
        uint8_t relative = 0x02;
        bool enable = false;
		int32_t value;

		switch (command) 
		{
		case SLAVE_CMD_SET_TEMP:	
			value=pop16();// 注释掉下面的，但是别忘了 pop;
//			board.getExtruderBoard(id).getExtruderHeater().set_target_temperature(pop16());
//			if(board.getPlatformHeater().isHeating()){
//				board.getExtruderBoard(id).getExtruderHeater().Pause(true);}
			fprintf(fw,"SLAVE_CMD_SET_TEMP %d\n",value);
			return true;
		// can be removed in process via host query works OK
 		case SLAVE_CMD_PAUSE_UNPAUSE:
			//pause(!command::isPaused());
			fprintf(fw,"SLAVE_CMD_PAUSE_UNPAUSE\n");
			return true;
		case SLAVE_CMD_TOGGLE_FAN:
			value=circleBufPop(&cmdBuf);
			fprintf(fw,"SLAVE_CMD_TOGGLE_FAN\n");
			//board.getExtruderBoard(id).setFan((pop8() & 0x01) != 0);
			return true;
		case SLAVE_CMD_TOGGLE_VALVE:
			value=circleBufPop(&cmdBuf);
			fprintf(fw,"SLAVE_CMD_TOGGLE_VALVE\n");
			//board.setValve((pop8() & 0x01) != 0);
			return true;
		case SLAVE_CMD_SET_PLATFORM_TEMP:
			value=pop16();
//			board.setUsingPlatform(true);
//			board.getPlatformHeater().set_target_temperature(pop16());
//			// pause extruder heaters if active
//			board.getExtruderBoard(0).getExtruderHeater().Pause(true);
//			board.getExtruderBoard(1).getExtruderHeater().Pause(true);
			fprintf(fw,"SLAVE_CMD_SET_PLATFORM_TEMP %d\n",value);
			return true;
        // not being used with 5D
		case SLAVE_CMD_TOGGLE_MOTOR_1:
			//DEBUG_PIN1.setValue(true);
			enable = circleBufPop(&cmdBuf) & 0x01 ? true:false;
			//cmdMode = MOVING;
			//steppers::enableAxis(4, enable);
			b = 360;
			fprintf(fw,"SLAVE_CMD_TOGGLE_MOTOR_1 %d\n",enable);
			return true;
        // not being used with 5D
		case SLAVE_CMD_TOGGLE_MOTOR_2: 
			//DEBUG_PIN1.setValue(true);
			enable = circleBufPop(&cmdBuf) & 0x01 ? true:false;
			//steppers::enableAxis(3, enable);
			//axisSetEnablePin(3,enable); 错了，这里3,4应该对应到喷嘴了
			a = 160;
			fprintf(fw,"SLAVE_CMD_TOGGLE_MOTOR_2 %d\n",enable);
			return true;
		case SLAVE_CMD_SET_MOTOR_1_PWM:
			value=circleBufPop(&cmdBuf);
			fprintf(fw,"SLAVE_CMD_SET_MOTOR_1_PWM %d\n",value);
			return true;
		case SLAVE_CMD_SET_MOTOR_2_PWM:
			value=circleBufPop(&cmdBuf);
			fprintf(fw,"SLAVE_CMD_SET_MOTOR_2_PWM %d\n",value);
			return true;
		case SLAVE_CMD_SET_MOTOR_1_DIR:
			value=circleBufPop(&cmdBuf);
			fprintf(fw,"SLAVE_CMD_SET_MOTOR_1_DIR %d\n",value);
			return true;
		case SLAVE_CMD_SET_MOTOR_2_DIR:
			value=circleBufPop(&cmdBuf);
			fprintf(fw,"SLAVE_CMD_SET_MOTOR_2_DIR %d\n",value);
			return true;
		case SLAVE_CMD_SET_MOTOR_1_RPM:
			value=pop32();
			fprintf(fw,"SLAVE_CMD_SET_MOTOR_2_RPM %d\n",value);
			return true;
		case SLAVE_CMD_SET_MOTOR_2_RPM:
			value=pop32();
			fprintf(fw,"SLAVE_CMD_SET_MOTOR_2_RPM %d\n",value);
			return true;
		}
	return false;
}/*}}}*/
uint8_t pop8()/*{{{*/
{
	return circleBufPop(&cmdBuf);
}/*}}}*/
int16_t pop16() /*{{{*/
{
	union {
		// AVR is little-endian
		int16_t a;
		struct {
			uint8_t data[2];
		} b;
	} shared;
	shared.b.data[0] = circleBufPop(&cmdBuf);
	shared.b.data[1] = circleBufPop(&cmdBuf);
	return shared.a;
}/*}}}*/

int32_t pop32() /*{{{*/
{
	union {
		// AVR is little-endian
		int32_t a;
		struct {
			uint8_t data[4];
		} b;
	} shared;
	shared.b.data[0] = circleBufPop(&cmdBuf);
	shared.b.data[1] = circleBufPop(&cmdBuf);
	shared.b.data[2] = circleBufPop(&cmdBuf);
	shared.b.data[3] = circleBufPop(&cmdBuf);
	return shared.a;
}/*}}}*/
