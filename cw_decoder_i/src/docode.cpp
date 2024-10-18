#include <stdint.h>
#include <string.h>

typedef struct _code_data {
    char    *code;
    uint8_t  data;
} code_t;

#if 1
code_t us_code[] = {
    {".-", 	    65},
	{"-...",    66},
	{"-.-.",    67},
	{"-..", 	68},
	{".", 	    69},
	{"..-.",    70},
	{"--.", 	71},
	{"....",    72},
	{"..", 	    73},
	{".---",    74},
	{"-.-", 	75},
	{".-..",    76},
	{"--", 	    77},
	{"-.", 	    78},
	{"---", 	79},
	{".--.",    80},
	{"--.-",    81},
	{".-.", 	82},
	{"...", 	83},
	{"-", 	    84},
	{"..-", 	85},
	{"...-",    86},
	{".--", 	87},
	{"-..-",    88},
	{"-.--",    89},
	{"--..",    90},

	{".----",   49},
	{"..---",   50},
	{"...--",   51},
	{"....-",   52},
	{".....",   53},
	{"-....",   54},
	{"--...",   55},
	{"---..",   56},
	{"----.",   57},
	{"-----",   48},

	{"..--..",  63},
	{".-.-.-",  46},
	{"--..--",  44},
	{"-.-.--",  33},
	{".--.-.",  64},
	{"---...",  58},
	{"-....-",  45},
	{"-..-.",   47},

	{"-.--.", 	40},
	{"-.--.-", 	41},
	{".-...", 	95},
	{"...-..-", 36},
	{"...-.-", 	62},
	{".-.-.", 	60},
	{"...-.", 	126},
	//////////////////
	// The specials //
	//////////////////
//	{".-.-", 	3},
//	{"---.", 	4},
//	{".--.-", 	6},
};


////////////////////////////////
// translate cw code to ascii //
////////////////////////////////
int16_t docode(char *code) 
{
    for (int i = 0; i < sizeof(us_code) / sizeof(code_t); i++) {
        if (strcmp(code, us_code[i].code) == 0) {
            return us_code[i].data;
        };
    }
    return 0;
}
#else
////////////////////////////////
// translate cw code to ascii //
////////////////////////////////
int16_t docode(char *code)
{
    if (strcmp(code,".-") 	== 0) return(65);
	if (strcmp(code,"-...") == 0) return(66);
	if (strcmp(code,"-.-.") == 0) return(67);
	if (strcmp(code,"-..") 	== 0) return(68);
	if (strcmp(code,".") 	== 0) return(69);
	if (strcmp(code,"..-.") == 0) return(70);
	if (strcmp(code,"--.") 	== 0) return(71);
	if (strcmp(code,"....") == 0) return(72);
	if (strcmp(code,"..") 	== 0) return(73);
	if (strcmp(code,".---") == 0) return(74);
	if (strcmp(code,"-.-") 	== 0) return(75);
	if (strcmp(code,".-..") == 0) return(76);
	if (strcmp(code,"--") 	== 0) return(77);
	if (strcmp(code,"-.") 	== 0) return(78);
	if (strcmp(code,"---") 	== 0) return(79);
	if (strcmp(code,".--.") == 0) return(80);
	if (strcmp(code,"--.-") == 0) return(81);
	if (strcmp(code,".-.") 	== 0) return(82);
	if (strcmp(code,"...") 	== 0) return(83);
	if (strcmp(code,"-") 	== 0) return(84);
	if (strcmp(code,"..-") 	== 0) return(85);
	if (strcmp(code,"...-") == 0) return(86);
	if (strcmp(code,".--") 	== 0) return(87);
	if (strcmp(code,"-..-") == 0) return(88);
	if (strcmp(code,"-.--") == 0) return(89);
	if (strcmp(code,"--..") == 0) return(90);

	if (strcmp(code,".----") == 0) return(49);
	if (strcmp(code,"..---") == 0) return(50);
	if (strcmp(code,"...--") == 0) return(51);
	if (strcmp(code,"....-") == 0) return(52);
	if (strcmp(code,".....") == 0) return(53);
	if (strcmp(code,"-....") == 0) return(54);
	if (strcmp(code,"--...") == 0) return(55);
	if (strcmp(code,"---..") == 0) return(56);
	if (strcmp(code,"----.") == 0) return(57);
	if (strcmp(code,"-----") == 0) return(48);

	if (strcmp(code,"..--..") == 0) return(63);
	if (strcmp(code,".-.-.-") == 0) return(46);
	if (strcmp(code,"--..--") == 0) return(44);
	if (strcmp(code,"-.-.--") == 0) return(33);
	if (strcmp(code,".--.-.") == 0) return(64);
	if (strcmp(code,"---...") == 0) return(58);
	if (strcmp(code,"-....-") == 0) return(45);
	if (strcmp(code,"-..-.")  == 0) return(47);

	if (strcmp(code,"-.--.") 	== 0) return(40);
	if (strcmp(code,"-.--.-") 	== 0) return(41);
	if (strcmp(code,".-...") 	== 0) return(95);
	if (strcmp(code,"...-..-") 	== 0) return(36);
	if (strcmp(code,"...-.-") 	== 0) return(62);
	if (strcmp(code,".-.-.") 	== 0) return(60);
	if (strcmp(code,"...-.") 	== 0) return(126);
	//////////////////
	// The specials //
	//////////////////
//	if (strcmp(code,".-.-") 	== 0) return(3);
//	if (strcmp(code,"---.") 	== 0) return(4);
//	if (strcmp(code,".--.-") 	== 0) return(6);
	return 0;
}
#endif
