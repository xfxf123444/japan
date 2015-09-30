#ifndef MISC_H_
#define MISC_H_


#define RESIZE_PARTITION    1
#define CONVERT_PARTITION   2

//  Type : TRUE
//
//  Pri ---> Logical
//
//  SubType :
//
    typedef struct  tagPri2Log_1
	{
	    DWORD   Pri_Start;
	    DWORD   Pri_Size;
	    DWORD   First_Log_Start;
	    DWORD   First_Log_Size;
	} PRI2LOG_1,*PPRI2LOG_1;

//		1.
//
//		      |_______ Extended _______|
//     _______________|________________________|_____
//	   |	 |    | 	|	       |
//	   | Pri |    | Logical |	       |
//     ____|_____|____|_________|______________|_____
//
//
//		     |________ Extended _______|
//     ______________|_________________________|____________
//	   |	 |   |	     |	       |	      |
//	   | Pri |   | NULL  | Logical |	      |
//     ____|_____|___|_______|_________|______________|_____
//
//
//		 |___________ Extended ________|
//     __________|_____________________________|_____
//	   |	 |    | 	|	       |
//	   | Pri |    | Logical |	       |
//     ____|_____|____|_________|______________|_____
//
//
//		 |________ Extended ______|
//     __________|________________________|_____
//	   |	 |	   |		  |
//	   | Pri | Logical |		  |
//     ____|_____|_________|______________|_____
//
//
    typedef struct  tagPri2Log_2
	{
	    DWORD   Pri_Start;
	    DWORD   Pri_Size;
	} PRI2LOG_2,*PPRI2LOG_2;

//
//		2.
//
//
//		 |________ Extended ________|
//     __________|__________________________|_______
//	   |	 |			    |
//	   | Pri | (No logical partition)   |
//     ____|_____|__________________________|________
//
//
//
//		     |________ Extended ________|
//     ______________|__________________________|_______
//	   |	 |   |				|
//	   | Pri |   | (No logical partition)	|
//     ____|_____|___|__________________________|________
//
//

    typedef struct  tagPri2Log_3
	{
	    DWORD   Pri_Start;
	    DWORD   Pri_Size;
	    DWORD   Ext_Start;
	} PRI2LOG_3,*PPRI2LOG_3;

//
//		3.
//
//	   |________ Extended ____|
//	___|______________________|____________________
//	   |			  |	|     |
//	   |(No logical partition)|	| Pri |
//	___|______________________|_____|_____|________
//
//
//	   |________ Extended ____|
//	___|______________________|______________
//	   |			  |	|
//	   |(No logical partition)| Pri |
//	___|______________________|_____|________
//

    typedef struct  tagPri2Log_4
	{
	    DWORD   Pri_Start;
	    DWORD   Pri_Size;
	    DWORD   Ext_Start;
	    DWORD   Last_Log_Start;
	} PRI2LOG_4,*PPRI2LOG_4;

//
//		4.
//
//	   |________ Extended _______|
//	___|_________________________|_______________
//	   |		     |	     |	   |   |
//	   |		     |Logical|	   |Pri|
//	___|_________________|_______|_____|___|______
//
//
//	   |________ Extended __________|
//	___|____________________________|____________
//	   |		     |	     |	|   |	|
//	   |		     |Logical|	|   |Pri|
//	___|_________________|_______|__|___|___|______
//
//
//	   |________ Extended _______|
//	___|_________________________|_________
//	   |		     |	     |	 |
//	   |		     |Logical|Pri|
//	___|_________________|_______|___|______
//
//
//	   |________ Extended __________|
//	___|____________________________|________
//	   |		     |	     |	|   |
//	   |		     |Logical|	|Pri|
//	___|_________________|_______|__|___|______
//

    typedef struct  tagPri2Log_5
	{
	    DWORD   Pri_Start;
	    DWORD   Pri_Size;
	} PRI2LOG_5,*PPRI2LOG_5;

//
//		5.
//		No extended partiton
//
//	_____________________________________
//	   |	 |
//	   | Pri |
//	___|_____|_____________________________
//
//
//
//  Type : FALSE
//
//	Logical ---> Pri
//
//	SubType :

    typedef struct  tagLog2Pri_1
	{
	    DWORD   Ext_Start;
	    DWORD   Ext_End;
	    DWORD   Log_Start;
	} LOG2PRI_1,*PLOG2PRI_1;

//		    1.
//
//	    |___________ Extended ______________|
//	____|___________________________________|_________________
//	    |	    |				|
//	    |Logical|				|
//	____|_______|___________________________|_________________
//
//
//	    |_________ Extended _______|
//	____|__________________________|__________________________
//	    |		|	|      |
//	    |  NULL	|Logical| NULL |
//	____|___________|_______|______|____________________________
//
//
    typedef struct  tagLog2Pri_2
	{
	    DWORD   Log_Start;
	    DWORD   Log_Size;
	    DWORD   Next_Log_Size;
	    DWORD   Ext_End;
	} LOG2PRI_2,*PLOG2PRI_2;

//		    2.	( convert Logical 1 to pri )
//
//	    |______________ Extended ________________|
//	____|________________________________________|______________
//	    |	      | 	|		     |
//	    |Logical 1|Logical 2|		     |
//	____|_________|_________|____________________|________________
//

    typedef struct  tagLog2Pri_3
	{
	    DWORD   Ext_Start;
	    DWORD   Log_Start;
	    DWORD   Log_Size;
	} LOG2PRI_3,*PLOG2PRI_3;

//		    3.
//
//	    |______________ Extended ________________|
//	____|________________________________________|________________
//	    |	      |      |	       |	     |
//	    |Logical 1| NULL |Logical 2|	     |
//	____|_________|______|_________|_____________|___________________
//
//
//		( convert Logical 2 to pri )
//
//	    |______________ Extended ________________|
//	____|________________________________________|______________
//	    |	      | 	|		     |
//	    |Logical 1|Logical 2|		     |
//	____|_________|_________|____________________|________________
//
//
    typedef struct  tagLog2Pri_4
	{
	    DWORD   Prev_Log_Start;
	    DWORD   Log_Start;
	    DWORD   Log_Size;
	} LOG2PRI_4,*PLOG2PRI_4;

//		    4.
//
//	    |_____________ Extended ____|
//	____|___________________________|______________________________
//	    |	    |		|	|
//	    |	    | ......	|Logical|
//	____|_______|___________|_______|________________________________
//
//
//	    |____________ Extended _________|
//	____|_______________________________|____________________________
//	    |	    |		|	|   |
//	    |	    | ......	|Logical|NUL|
//	____|_______|___________|_______|___|_______________________________
//
//

typedef struct	tagPri2LogInfo
    {
	DWORD	Type	:   1;	//  TRUE --- Pri ---> Logical
				//  FALSE --- Log ---> Pri
	DWORD	SubType :   5;
	DWORD	SystemFlag  :	8;
	DWORD	Pri_Index   :	2;
	DWORD	Ext_Index   :	2;
	union
	    {
		PRI2LOG_1   P2L_1;
		PRI2LOG_2   P2L_2;
		PRI2LOG_3   P2L_3;
		PRI2LOG_4   P2L_4;
		PRI2LOG_5   P2L_5;

		LOG2PRI_1   L2P_1;
		LOG2PRI_2   L2P_2;
		LOG2PRI_3   L2P_3;
		LOG2PRI_4   L2P_4;
	    } Info;
    } PRI2LOG_INFO,*PPRI2LOG_INFO;


#endif	//  MISC_H_
