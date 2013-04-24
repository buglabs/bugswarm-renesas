/********************************************************************
Copyright 2012 BNS Solutions.
The computer program contained herein is the property of BNS Solutions and
may not be copied in whole or in part without prior written
authorization from BNS Solutions.

Filename:        	eink_sample_display.c
Creation Date: 	July 4, 2012
Author:			O. Raut

Description:	This file contains information about the display screens
                which are supported by default in the demo board without
                the need to download information

REVISION HISTORY:
          07/08/2012: Code ported over from R5F100MJ to R5F100MF
          07/20/2012: Renamed CUSTOM_SCREEN_DEF to EINK_SCREEN_DEF
*********************************************************************/

#include "r_cg_macrodriver.h"
#include "r_cg_userdefine.h"
#include "eink_rdk_display.h"
#include "eink_driver.h"
#include <string.h>

//  k l a
//  j   b
//  i m c      
//  h   d
//  g f e
   
struct EINK_SCREEN_DEF rdkScreen;
extern uint8_t dpLocation;

const uint8_t pin2SegConnectionRdk[40] = {
    2,       
    3,
    4,
    5,
    6,
    7,
};

const uint8_t displayZoneInfoRdk[2][6] = {
    1, 1,
    1, 1,
    1, 1,
    1, 1,
    1, 1,
    1, 1
};

const uint8_t ascii2SegMappingInfoRdk[4] = {
    8, 255,
    16, 127
};

const uint8_t alternateAsciiMapRdk[] = {
    'S',	//0		null
    0x00,	//1		start"
    0x00,	//2		start"
    0x00,	//3		end"
    0x00,	//4		end"
    0x00,	//5		enquiry"
    0x00,	//6		acknowledge"
    0x00,	//7		bell"
    0x00,	//8		backspace"
    0x00,	//9		horizontal"
    0x00,	//10	new"
    0x00,	//11	vertical"
    0x00,	//12	new"
    0x00,	//13	carriage"
    0x00,	//14	shift"
    0x00,	//15	shift"
    0x00,	//16	data"
    0x00,	//17	device"
    0x00,	//18	device"
    0x00,	//19	device"
    0x00,	//20	device"
    0x00,	//21	negative"
    0x00,	//22	synchronous"
    0x00,	//23	end"
    0x00,	//24	cancel"
    0x00,	//25	end"
    0x00,	//26	substitute"
    0x00,	//27	escape"
    0x00,	//28	file"
    0x00,	//29	group"
    0x00,	//30	record"
    0x00,	//31	unit"
    0x00,	//32	space"
    0x00,	//33	!
    0x00,	//34	"""
    0x00,	//35	#"
    0x00,	//36	$"
    0x00,	//37	%"
    0x00,	//38	&"
    0x00,	//39	'"
    0x00,	//40	("
    0x00,	//41	)"
    0x00,	//42	*"
    0x00,	//43	+"
    0x00,	//44	"
    0x00,	//45	-"
    0x00,	//46	."
    0x00,	//47	/"
    0x00,	//48	0"
    0x00,	//49	1"
    0x00,	//50	2"
    0x00,	//51	3"
    0x00,	//52	4"
    0x00,	//53	5"
    0x00,	//54	6"
    0x00,	//55	7"
    0x00,	//0xFE
    0x00,	//0xF6
    0x00,	//58	:"
    0x00,	//59	;"
    0x00,	//60	<"
    0x00,	//61	="
    0x00,	//62	>"
    0x00,	//63	?"
    0x00,	//64	@"
    0x00,	//65	A"
    0x00,	//66	B"
    0x00,	//67	C"
    0x00,	//68	D"
    0x00,	//69	E"
    0x00,	//70	F"
    0x00,	//71	G"
    0x00,	//72	H"
    0x00,	//73	I"
    0x00,	//74	J"
    0x00,	//75	K"
    0x00,	//76	L"
    0x00,	//77	M"
    0x00,	//78	N"
    0x00,	//79	O"
    0x00,	//80	P"
    0x00,	//81	Q"
    0x00,	//82	R"
    0x00,	//83	S"
    0x00,	//84	T"
    0x00,	//85	U"
    0x00,	//86	V"
    0x00,	//87	W"
    0x00,	//88	X"
    0x00,	//89	Y"
    0x00,	//90	Z"
    0x00,	//91	["
    0x00,	//92	'\'"
    0x00,	//93	]"
    0x00,	//94	^"
    0x00,	//95	_"
    0xF0,	//96	`"  // 0b1111 1111 1111 0000
    0xFF,	//97	a"
    0x00,	//98	b"  // 0b0111 1100 0000 0000
    0x7C,	//99	c"
    0xD8,	//100	d"  // 0b1111 0111 1101 1000
    0xF7,	//101	e"
    0x58,	//102	f"  // 0b1111 1111 0101 1000
    0xFF,	//103	g"
    0x78,	//104	h"  // 0b0111 1100 0111 1000
    0x7C,	//105	i"
    0x78,	//106	j"  // 0b1101 1111 0111 1000
    0xDF,	//107	k"
    0xF8,	//108	l"  // 0b1101 1111 1111 1000
    0xDF,	//109	m"
    0x10,	//110	n"  // 0b1111 1100 0001 0000
    0xFC,	//111	o"
    0xF8,	//0x00,     // 0b1111 1111 1111 1000
    0xFF,	//0x00,
    0x78,	//114	r"  // 0b1111 1111 0111 1000
    0xFF,	//115	s"
    0x00,	//116	t"
    0x00,	//117	u"
    0x00,	//118	v"
    0x00,	//119	w"
    0x00,	//120	x"
    0x00,	//121	y"
    0x00,	//122	z"
    0x00,	//123	{"
    0x00,	//124	|"
    0x00,	//125	}"
    0x00,	//126	~"
    0x00,	//127	DEL"
    0x00,	//128	//ICON"
    0x00,	//129	"
    0x00,	//130	"
    0x00,	//131	"
    0x00,	//132	"
    0x00,	//133	"
    0x00,	//134	"
    0x00,	//135	"
    0x00,	//136	"
    0x00,	//137	"
    0x00,	//138	"
    0x00,	//139	"   //0b1000 1111 1000 0000
    0x00,	//140	"
    0x00,	//141	"
    0x00,	//142	"
    0x00,	//143	"
    0x00,	//144	"
    0x00,	//145	"
    0x00,	//146	"
    0x00,	//147	"
    0x00,	//148	"
    0x00,	//149	"
    0x80,	//150	"
    0x57,	//151	"   //0b0101 0111 1000 0000
    0x00,	//152	"
    0x00,	//153	"
    0x00,	//154	"
    0x00,	//155	"
    0x00,	//156	"
    0x00,	//157	"
    0x00,	//158	"
    0x00,	//159	"
    0x00,	//160	"
    0x00,	//161	"
    0x00,	//162	"
    0x00,	//163	"
    0x00,	//164	"
    0x00,	//165	"
    0x00,	//166	"
    0x00,	//167	"
    0x00,	//168	"
    0x00,	//169	"
    0x00,	//170	"
    0x00,	//171	"
    0x00,	//172	"
    0x00,	//173	"
    0x00,	//174	"
    0x00,	//175	"
    0x00,	//176	"
    0x00,	//177	"
    0x00,	//178	"
    0x00,	//179	"
    0x00,	//180	"
    0x00,	//181	"
    0x00,	//182	"
    0x00,	//183	"
    0x00,	//184	"
    0x00,	//185	"
    0x00,	//186	"
    0x00,	//187	"
    0x00,	//188	"
    0x00,	//189	"
    0x00,	//190	"
    0x00,	//191	"
    0x00,	//192	"
    0x00,	//193	"
    0x00,	//194	"
    0x00,	//195	"
    0x00,	//196	"
    0x00,	//197	"
    0x00,	//198	"
    0x00,	//199	"
    0x00,	//200	"
    0x00,	//201	"
    0x00,	//202	"
    0x00,	//203	"
    0x00,	//204	"
    0x00,	//205	"
    0x00,	//206	"
    0x00,	//207	"
    0x00,	//208	"
    0x00,	//209	"
    0x00,	//210	"
    0x00,	//211	"
    0x00,	//212	"
    0x00,	//213	"
    0x00,	//214	"
    0x00,	//215	"
    0x00,	//216	"
    0x00,	//217	"
    0x00,	//218	"
    0x00,	//219	"
    0x00,	//220	"
    0x00,	//221	" // 0b0001 0110 1000
    0x00,	//222	"
    0x00,	//223	"
    0x00,	//224	"
    0x00,	//225	"
    0x00,	//226	"
    0x00,	//227	"
    0x00,	//228	"
    0x00,	//229	"
    0x00,	//230	"
    0x00,	//231	"
    0x00,	//232	"
    0x00,	//233	"
    0x00,	//234	"
    0x00,	//235	"
    0x00,	//236	"
    0x00,	//237	"
    0x00,	//238	"
    0x00,	//239	"
    0x00,	//240	"
    0x00,	//241	"
    0x00,	//242	"
    0x00,	//243	"
    0x00,	//244	"
    0x00,	//245	"
    0x00,	//246	"
    0x00,	//247	"
    0x00,	//248	"
    0x00,	//249	"
    0x00,	//250	"
    0x00,	//251	"
    0x00,	//252	"
    0x00,	//253	"
    0x00,	//254	"
    0x00

};

const uint8_t defaultAsciiMapRdk[] = {
    'E',	//0		null
    0x00,	//1		start of heading
    0x00,	//2		start of text
    0x00,	//3		end of text
    0x00,	//4		end of transmission
    0x00,	//5		enquiry
    0x00,	//6		acknowledge
    0x00,	//7		bell
    0x00,	//8		backspace
    0x00,	//9		horizontal tab
    0x00,	//10	new line
    0x00,	//11	vertical tab
    0x00,	//12	new page
    0x00,	//13	carriage return
    0x00,	//14	shift out
    0x00,	//15	shift in
    0x00,	//16	data link escape
    0x00,	//17	device control 1
    0x00,	//18	device control 2
    0x00,	//19	device control 3
    0x00,	//20	device control 4
    0x00,	//21	negative acknowledge
    0x00,	//22	synchronous idle
    0x00,	//23	end of trans. block
    0x00,	//24	cancel
    0x00,	//25	end of medium
    0x00,	//26	substitute
    0x00,	//27	escape
    0x00,	//28	file separator
    0x00,	//29	group separator
    0x00,	//30	record separator
    0x00,	//31	unit separator
    0x00,	//32	space
    0x00,	//33	!
    0x00,	//34	"
    0x00,	//35	#
    0x00,	//36	$
    0x00,	//37	%
    0x00,	//38	&
    0x00,	//39	'
    0x00,	//40	(
    0x00,	//41	)
    0x00,	//42	*
    0x00,	//43	+
    0x00,	//44	,
    0x00,	//45	-
    0xFF,	//46	.
    0x00,	//47	/
    0x00,	//48	0
    0x00,	//49	1
    0x00,	//50	2
    0x00,	//51	3
    0x00,	//52	4
    0x00,	//53	5
    0x00,	//54	6
    0x00,	//55	7
    0xFF,	//56	8
    0x00,	//57	9
    0x00,	//58	:
    0x00,	//59	;
    0x00,	//60	<
    0x00,	//61	=
    0x00,	//62	>
    0x00,	//63	?
    0x00,	//64	@
    0x00,	//65	A
    0x00,	//66	B
    0x00,	//67	C
    0x00,	//68	D
    0x00,	//69	E
    0x00,	//70	F
    0x00,	//71	G
    0x00,	//72	H
    0x00,	//73	I
    0x00,	//74	J
    0x00,	//75	K
    0x00,	//76	L
    0x00,	//77	M
    0x00,	//78	N
    0x00,	//79	O
    0x00,	//80	P
    0x00,	//81	Q
    0x00,	//82	R
    0x00,	//83	S
    0x00,	//84	T
    0x00,	//85	U
    0x00,	//86	V
    0x00,	//87	W
    0x00,	//88	X
    0x00,	//89	Y
    0x00,	//90	Z
    0x00,	//91	[
    0x00,	//92	'\'
    0x00,	//93	]
    0x00,	//94	^
    0x00,	//95	_
    0x00,	//96	`           // 0      0b1111 1111 0000 0000
    0x00,	//97	a
    0x00,	//98	b           // 1      0b0111 0000 0000 0000
    0x00,	//99	c
    0x00,	//100	d           // 2      0b1110 1110 1000 0000
    0x00,	//101	e
    0x00,	//102	f           // 3      0b1111 1000 1000 0000
    0x00,	//103	g
    0x00,	//104	h           // 4      0b0111 0011 1000 0000
    0x00,	//105	i
    0x00,	//106	j           // 5      0b1011 1011 1000 0000
    0x00,	//107	k
    0x00,	//108	l           // 6      0b1011 1111 1000 0000
    0x00,	//109	m
    0x00,	//110	n           // 7      0b1111 0000 0000 0000
    0x00,	//111	o
    0x00,       //112	p           // 8      0b1111 1111 1000 0000
    0x00,       //113	q
    0x00,	//114	r           // 9      0b1111 1011 1000 0000
    0x00,	//115	s
    0x00,	//116	t
    0x00,	//117	u
    0x00,	//118	v
    0x00,	//119	w
    0x00,	//120	x
    0x00,	//121	y
    0x00,	//122	z
    0x00,	//123	{
    0x00,	//124	|
    0x00,	//125	}
    0x00,	//126	~
    0x00,	//127	DEL
    0x00,	//128	//ICON LISTINGS CAN START HERE.
    0x00,	//129
    0x00,	//130
    0x00,	//131
    0x00,	//132
    0x00,	//133
    0x00,	//134
    0x00,	//135
    0x00,	//136
    0x00,	//137
    0x00,	//138
    0x00,	//139
    0x00,	//140
    0x00,	//141
    0x00,	//142
    0x00,	//143
    0x00,	//144
    0x00,	//145
    0x00,	//146
    0x00,	//147
    0x00,	//148
    0x00,	//149
    0x00,	//150
    0x00,	//151
    0x00,	//152
    0x00,	//153
    0x00,	//154
    0x00,	//155
    0x00,	//156
    0x00,	//157
    0x00,	//158
    0x00,	//159
    0x00,	//160
    0x00,	//161
    0x00,	//162
    0x00,	//163
    0x00,	//164
    0x00,	//165
    0x00,	//166
    0x00,	//167
    0x00,	//168
    0x00,	//169
    0x00,	//170
    0x00,	//171
    0x00,	//172
    0x00,	//173
    0x00,	//174
    0x00,	//175
    0x00,	//176
    0x00,	//177
    0x00,	//178
    0x00,	//179
    0x00,	//180
    0x00,	//181
    0x00,	//182
    0x00,	//183
    0x00,	//184
    0x00,	//185
    0x00,	//186
    0x00,	//187
    0x00,	//188
    0x00,	//189
    0x00,	//190
    0x00,	//191
    0x00,	//192
    0x00,	//193
    0x00,	//194
    0x00,	//195
    0x00,	//196
    0x00,	//197
    0x00,	//198
    0x00,	//199
    0x00,	//200
    0x00,	//201
    0x00,	//202
    0x00,	//203
    0x00,	//204
    0x00,	//205
    0x00,	//206
    0x00,	//207
    0x00,	//208
    0x00,	//209
    0x00,	//210
    0x00,	//211
    0x00,	//212
    0x00,	//213
    0x00,	//214
    0x00,	//215
    0x00,	//216
    0x00,	//217
    0x00,	//218
    0x00,	//219
    0x00,	//220
    0x00,	//221
    0x00,	//222
    0x00,	//223
    0x00,	//224
    0x00,	//225
    0x00,	//226
    0x00,	//227
    0x00,	//228
    0x00,	//229
    0x00,	//230
    0x00,	//231
    0x00,	//232
    0x00,	//233
    0x00,	//234
    0x00,	//235
    0x00,	//236
    0x00,	//237
    0x00,	//238
    0x00,	//239
    0x00,	//240
    0x00,	//241
    0x00,	//242
    0x00,	//243
    0x00,	//244
    0x00,	//245
    0x00,	//246
    0x00,	//247
    0x00,	//248
    0x00,	//249
    0x00,	//250
    0x00,	//251
    0x00,	//252
    0x00,	//253
    0x00,	//254
    0x00	//255
};

void SetupRdkScreen(void)
{
  memcpy(rdkScreen.SCR_NAME,"RDK01",6);
  rdkScreen.numDisplayZones = 6;
  rdkScreen.displayZoneInformation = (uint8_t*)displayZoneInfoRdk;
  rdkScreen.numPinConnections = 6;
  rdkScreen.pinToSegmentConnectionInformation = (uint8_t*)pin2SegConnectionRdk;
  rdkScreen.numValidAsciiToSegmentMappings = 2;
  rdkScreen.asciiToSegmentMappingInformation = (uint8_t*)ascii2SegMappingInfoRdk;
  rdkScreen.currMapSegs = 8;
  rdkScreen.alternateAsciiToSegmentMap = (uint8_t*)alternateAsciiMapRdk;
  rdkScreen.defaultAsciiToSegmentMap = (uint8_t*)defaultAsciiMapRdk;
  dpLocation = 1;
}

