
/*
 * CFile1.c
 *
 * Created: 30.01.2018 00.50.02
 *  Author: medlem
 */ 

#define F_CPU 16000000UL  // 16 MHz
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "tone.h"

#define FLOPPY_STEP		PINA5
#define FLOPPY_ENABLE	PINA1
#define FLOPPY_MOTOR	PINA2
#define FLOPPY_DIR		PINA3

//
// B�lgelengder for alle semitonene. Shiftes til h�yre for oktaver.
//
const uint16_t scale1[] PROGMEM = {61156, 61128, 61101, 61073, 61046, 61018, 60991, 60963, 60936, 60908, 60881, 60853, 60826, 60798, 60771, 60743, 60716, 60689, 60661, 60634, 60607, 60579, 60552, 60525, 60497, 60470, 60443, 60415, 60388, 60361, 60334, 60306, 60279, 60252, 60225, 60198, 60171, 60143, 60116, 60089, 60062, 60035, 60008, 59981, 59954, 59927, 59900, 59873, 59846, 59819, 59792, 59765, 59738, 59711, 59684, 59657, 59630, 59603, 59576, 59549, 59522, 59495, 59469, 59442, 62948, 62920, 62891, 62863, 62834, 62806, 62778, 62749, 62721, 62693, 62665, 62636, 62608, 62580, 62552, 62523, 62495, 62467, 62439, 62411, 62382, 62354, 62326, 62298, 62270, 62242, 62214, 62186, 62158, 62130, 62102, 62074, 62046, 62018, 61990, 61962, 61934, 61906, 61878, 61850, 61822, 61794, 61766, 61738, 61710, 61683, 61655, 61627, 61599, 61571, 61544, 61516, 61488, 61460, 61433, 61405, 61377, 61349, 61322, 61294, 61266, 61239, 61211, 61184};
const uint16_t scale2[] PROGMEM = {57724, 57698, 57672, 57646, 57620, 57594, 57568, 57542, 57516, 57490, 57464, 57438, 57412, 57386, 57360, 57335, 57309, 57283, 57257, 57231, 57205, 57180, 57154, 57128, 57102, 57076, 57051, 57025, 56999, 56974, 56948, 56922, 56896, 56871, 56845, 56819, 56794, 56768, 56743, 56717, 56691, 56666, 56640, 56615, 56589, 56564, 56538, 56513, 56487, 56462, 56436, 56411, 56385, 56360, 56334, 56309, 56284, 56258, 56233, 56207, 56182, 56157, 56131, 56106, 59415, 59389, 59362, 59335, 59308, 59282, 59255, 59228, 59201, 59175, 59148, 59121, 59095, 59068, 59041, 59015, 58988, 58961, 58935, 58908, 58882, 58855, 58828, 58802, 58775, 58749, 58722, 58696, 58669, 58643, 58616, 58590, 58564, 58537, 58511, 58484, 58458, 58432, 58405, 58379, 58353, 58326, 58300, 58274, 58247, 58221, 58195, 58169, 58142, 58116, 58090, 58064, 58037, 58011, 57985, 57959, 57933, 57907, 57881, 57854, 57828, 57802, 57776, 57750};
const uint16_t scale3[] PROGMEM = {54484, 54459, 54435, 54410, 54386, 54361, 54337, 54312, 54288, 54263, 54239, 54214, 54190, 54165, 54141, 54116, 54092, 54068, 54043, 54019, 53994, 53970, 53946, 53921, 53897, 53873, 53848, 53824, 53800, 53776, 53751, 53727, 53703, 53679, 53654, 53630, 53606, 53582, 53558, 53534, 53509, 53485, 53461, 53437, 53413, 53389, 53365, 53341, 53317, 53292, 53268, 53244, 53220, 53196, 53172, 53148, 53124, 53100, 53076, 53053, 53029, 53005, 52981, 52957, 56081, 56055, 56030, 56005, 55979, 55954, 55929, 55904, 55878, 55853, 55828, 55803, 55778, 55752, 55727, 55702, 55677, 55652, 55627, 55602, 55577, 55552, 55526, 55501, 55476, 55451, 55426, 55401, 55376, 55351, 55326, 55301, 55276, 55252, 55227, 55202, 55177, 55152, 55127, 55102, 55077, 55052, 55028, 55003, 54978, 54953, 54928, 54904, 54879, 54854, 54829, 54805, 54780, 54755, 54730, 54706, 54681, 54656, 54632, 54607, 54582, 54558, 54533, 54509};
const uint16_t scale4[] PROGMEM = {51426, 51403, 51380, 51356, 51333, 51310, 51287, 51264, 51241, 51218, 51194, 51171, 51148, 51125, 51102, 51079, 51056, 51033, 51010, 50987, 50964, 50941, 50918, 50895, 50872, 50849, 50826, 50803, 50780, 50757, 50734, 50712, 50689, 50666, 50643, 50620, 50597, 50574, 50552, 50529, 50506, 50483, 50460, 50438, 50415, 50392, 50369, 50347, 50324, 50301, 50279, 50256, 50233, 50211, 50188, 50165, 50143, 50120, 50097, 50075, 50052, 50030, 50007, 49985, 52933, 52909, 52885, 52861, 52837, 52814, 52790, 52766, 52742, 52718, 52695, 52671, 52647, 52623, 52600, 52576, 52552, 52528, 52505, 52481, 52457, 52434, 52410, 52386, 52363, 52339, 52315, 52292, 52268, 52245, 52221, 52198, 52174, 52150, 52127, 52103, 52080, 52056, 52033, 52009, 51986, 51963, 51939, 51916, 51892, 51869, 51845, 51822, 51799, 51775, 51752, 51729, 51705, 51682, 51659, 51635, 51612, 51589, 51565, 51542, 51519, 51496, 51472, 51449};
const uint16_t scale5[] PROGMEM = {48540, 48518, 48496, 48474, 48452, 48431, 48409, 48387, 48365, 48343, 48321, 48300, 48278, 48256, 48234, 48213, 48191, 48169, 48147, 48126, 48104, 48082, 48060, 48039, 48017, 47995, 47974, 47952, 47931, 47909, 47887, 47866, 47844, 47823, 47801, 47779, 47758, 47736, 47715, 47693, 47672, 47650, 47629, 47607, 47586, 47564, 47543, 47521, 47500, 47478, 47457, 47436, 47414, 47393, 47371, 47350, 47329, 47307, 47286, 47265, 47243, 47222, 47201, 47179, 49962, 49940, 49917, 49895, 49872, 49850, 49827, 49805, 49782, 49760, 49737, 49715, 49693, 49670, 49648, 49625, 49603, 49581, 49558, 49536, 49513, 49491, 49469, 49446, 49424, 49402, 49380, 49357, 49335, 49313, 49290, 49268, 49246, 49224, 49202, 49179, 49157, 49135, 49113, 49091, 49069, 49046, 49024, 49002, 48980, 48958, 48936, 48914, 48892, 48870, 48848, 48826, 48804, 48782, 48760, 48738, 48716, 48694, 48672, 48650, 48628, 48606, 48584, 48562};
const uint16_t scale6[] PROGMEM = {45815, 45794, 45774, 45753, 45732, 45712, 45691, 45671, 45650, 45629, 45609, 45588, 45568, 45547, 45526, 45506, 45485, 45465, 45444, 45424, 45403, 45383, 45362, 45342, 45321, 45301, 45281, 45260, 45240, 45219, 45199, 45179, 45158, 45138, 45117, 45097, 45077, 45056, 45036, 45016, 44995, 44975, 44955, 44935, 44914, 44894, 44874, 44854, 44833, 44813, 44793, 44773, 44752, 44732, 44712, 44692, 44672, 44652, 44631, 44611, 44591, 44571, 44551, 44531, 47157, 47136, 47115, 47094, 47072, 47051, 47030, 47009, 46988, 46966, 46945, 46924, 46903, 46882, 46860, 46839, 46818, 46797, 46776, 46755, 46734, 46713, 46692, 46671, 46650, 46628, 46607, 46586, 46565, 46544, 46523, 46502, 46481, 46460, 46439, 46419, 46398, 46377, 46356, 46335, 46314, 46293, 46272, 46251, 46230, 46210, 46189, 46168, 46147, 46126, 46105, 46085, 46064, 46043, 46022, 46001, 45981, 45960, 45939, 45918, 45898, 45877, 45856, 45836};
const uint16_t scale7[] PROGMEM = {43244, 43224, 43205, 43185, 43166, 43147, 43127, 43108, 43088, 43069, 43049, 43030, 43010, 42991, 42972, 42952, 42933, 42914, 42894, 42875, 42855, 42836, 42817, 42797, 42778, 42759, 42740, 42720, 42701, 42682, 42663, 42643, 42624, 42605, 42586, 42566, 42547, 42528, 42509, 42490, 42470, 42451, 42432, 42413, 42394, 42375, 42356, 42336, 42317, 42298, 42279, 42260, 42241, 42222, 42203, 42184, 42165, 42146, 42127, 42108, 42089, 42070, 42051, 42032, 44511, 44491, 44471, 44451, 44431, 44411, 44391, 44371, 44351, 44331, 44311, 44291, 44271, 44251, 44231, 44211, 44191, 44171, 44151, 44131, 44111, 44091, 44071, 44052, 44032, 44012, 43992, 43972, 43952, 43932, 43913, 43893, 43873, 43853, 43833, 43814, 43794, 43774, 43754, 43735, 43715, 43695, 43675, 43656, 43636, 43616, 43597, 43577, 43557, 43538, 43518, 43498, 43479, 43459, 43440, 43420, 43400, 43381, 43361, 43342, 43322, 43303, 43283, 43264};
const uint16_t scale8[] PROGMEM = {40817, 40799, 40780, 40762, 40743, 40725, 40707, 40688, 40670, 40652, 40633, 40615, 40597, 40578, 40560, 40542, 40523, 40505, 40487, 40469, 40450, 40432, 40414, 40396, 40377, 40359, 40341, 40323, 40305, 40286, 40268, 40250, 40232, 40214, 40196, 40177, 40159, 40141, 40123, 40105, 40087, 40069, 40051, 40033, 40015, 39996, 39978, 39960, 39942, 39924, 39906, 39888, 39870, 39852, 39834, 39816, 39798, 39780, 39763, 39745, 39727, 39709, 39691, 39673, 42013, 41994, 41975, 41956, 41937, 41918, 41899, 41881, 41862, 41843, 41824, 41805, 41786, 41767, 41748, 41730, 41711, 41692, 41673, 41654, 41636, 41617, 41598, 41579, 41560, 41542, 41523, 41504, 41486, 41467, 41448, 41429, 41411, 41392, 41373, 41355, 41336, 41317, 41299, 41280, 41261, 41243, 41224, 41206, 41187, 41168, 41150, 41131, 41113, 41094, 41076, 41057, 41039, 41020, 41002, 40983, 40965, 40946, 40928, 40909, 40891, 40872, 40854, 40835};
const uint16_t scale9[] PROGMEM = {38526, 38509, 38491, 38474, 38457, 38439, 38422, 38404, 38387, 38370, 38353, 38335, 38318, 38301, 38283, 38266, 38249, 38232, 38214, 38197, 38180, 38163, 38145, 38128, 38111, 38094, 38077, 38059, 38042, 38025, 38008, 37991, 37974, 37957, 37939, 37922, 37905, 37888, 37871, 37854, 37837, 37820, 37803, 37786, 37769, 37752, 37735, 37717, 37700, 37683, 37666, 37649, 37632, 37615, 37599, 37582, 37565, 37548, 37531, 37514, 37497, 37480, 37463, 37446, 39655, 39637, 39619, 39601, 39583, 39566, 39548, 39530, 39512, 39494, 39476, 39459, 39441, 39423, 39405, 39387, 39370, 39352, 39334, 39316, 39299, 39281, 39263, 39245, 39228, 39210, 39192, 39175, 39157, 39139, 39122, 39104, 39086, 39069, 39051, 39033, 39016, 38998, 38981, 38963, 38946, 38928, 38910, 38893, 38875, 38858, 38840, 38823, 38805, 38788, 38770, 38753, 38735, 38718, 38700, 38683, 38665, 38648, 38630, 38613, 38596, 38578, 38561, 38543};
const uint16_t scale10[] PROGMEM = {36364, 36348, 36331, 36315, 36298, 36282, 36266, 36249, 36233, 36217, 36200, 36184, 36168, 36151, 36135, 36119, 36102, 36086, 36070, 36054, 36037, 36021, 36005, 35989, 35972, 35956, 35940, 35924, 35907, 35891, 35875, 35859, 35843, 35826, 35810, 35794, 35778, 35762, 35746, 35730, 35713, 35697, 35681, 35665, 35649, 35633, 35617, 35601, 35585, 35569, 35553, 35537, 35521, 35505, 35489, 35473, 35457, 35441, 35425, 35409, 35393, 35377, 35361, 35345, 37430, 37413, 37396, 37379, 37362, 37345, 37328, 37311, 37295, 37278, 37261, 37244, 37227, 37211, 37194, 37177, 37160, 37144, 37127, 37110, 37093, 37077, 37060, 37043, 37026, 37010, 36993, 36976, 36960, 36943, 36926, 36910, 36893, 36876, 36860, 36843, 36826, 36810, 36793, 36777, 36760, 36743, 36727, 36710, 36694, 36677, 36661, 36644, 36628, 36611, 36594, 36578, 36561, 36545, 36528, 36512, 36496, 36479, 36463, 36446, 36430, 36413, 36397, 36380};
const uint16_t scale11[] PROGMEM = {34323, 34308, 34292, 34277, 34261, 34246, 34230, 34215, 34199, 34184, 34168, 34153, 34138, 34122, 34107, 34091, 34076, 34061, 34045, 34030, 34015, 33999, 33984, 33969, 33953, 33938, 33923, 33907, 33892, 33877, 33861, 33846, 33831, 33816, 33800, 33785, 33770, 33755, 33739, 33724, 33709, 33694, 33679, 33663, 33648, 33633, 33618, 33603, 33588, 33572, 33557, 33542, 33527, 33512, 33497, 33482, 33466, 33451, 33436, 33421, 33406, 33391, 33376, 33361, 35329, 35313, 35297, 35281, 35265, 35249, 35233, 35217, 35201, 35186, 35170, 35154, 35138, 35122, 35106, 35090, 35075, 35059, 35043, 35027, 35011, 34996, 34980, 34964, 34948, 34932, 34917, 34901, 34885, 34869, 34854, 34838, 34822, 34807, 34791, 34775, 34759, 34744, 34728, 34712, 34697, 34681, 34665, 34650, 34634, 34619, 34603, 34587, 34572, 34556, 34541, 34525, 34509, 34494, 34478, 34463, 34447, 34432, 34416, 34401, 34385, 34369, 34354, 34338};
const uint16_t scale12[] PROGMEM = {32396, 32381, 32367, 32352, 32338, 32323, 32308, 32294, 32279, 32265, 32250, 32236, 32221, 32207, 32192, 32177, 32163, 32148, 32134, 32119, 32105, 32090, 32076, 32061, 32047, 32033, 32018, 32004, 31989, 31975, 31960, 31946, 31932, 31917, 31903, 31888, 31874, 31860, 31845, 31831, 31816, 31802, 31788, 31773, 31759, 31745, 31730, 31716, 31702, 31688, 31673, 31659, 31645, 31630, 31616, 31602, 31588, 31573, 31559, 31545, 31531, 31516, 31502, 31488, 33345, 33330, 33315, 33300, 33285, 33270, 33255, 33240, 33225, 33210, 33195, 33180, 33165, 33150, 33135, 33120, 33105, 33090, 33076, 33061, 33046, 33031, 33016, 33001, 32986, 32971, 32956, 32941, 32927, 32912, 32897, 32882, 32867, 32852, 32838, 32823, 32808, 32793, 32778, 32764, 32749, 32734, 32719, 32704, 32690, 32675, 32660, 32645, 32631, 32616, 32601, 32587, 32572, 32557, 32543, 32528, 32513, 32498, 32484, 32469, 32455, 32440, 32425, 32411};
const uint16_t* scale[] = {scale1, scale2, scale3, scale4, scale5, scale6, scale7, scale8, scale9, scale10, scale11, scale12};

uint8_t current_tone = 0;
uint8_t current_octave = 0;

void setup_lyd()
{
	cli();
	DDRA |= (1<<FLOPPY_ENABLE)|(1<<FLOPPY_MOTOR)|(1<<FLOPPY_DIR)|(1<<FLOPPY_STEP);	// Output pins to drive
	PORTA |= (1<<FLOPPY_ENABLE)|(1<<FLOPPY_MOTOR)|(1<<FLOPPY_DIR);

	TCCR1A |= (1<<COM1B1)|(1<<WGM11)|(1<<WGM10);			// Fast PWM-mode, low after compare till TOP
	TCCR1A &= ~((1<<COM1B0));								// output to OC1B
	TCCR1B |= (1<<CS11)|(1<<WGM12)|(1<<WGM13);				// Use OCR1A as TOP, Prescaler 16MHz/8

	OCR1A = 0xFFFF;
	OCR1B = 0xFF00;

	TIMSK1 |= (1<<TOIE1);
	
	MCUCR &= ~((1<<ISC00)|(1<<ISC01));									// Interrupt on falling edge
	GIMSK |= (1<<INT0);

	
	sei();	
}

//
// Spill en tone
//
void playTone(uint8_t tone, uint8_t oktav)
{
	current_tone = tone;
	current_octave = oktav;
	soundTone(0);
	PORTA &= ~((1<<FLOPPY_ENABLE)|(1<<FLOPPY_MOTOR));
}

//
// Note: bend_amount is 7-bit signed (-64 to 63)
//
void soundTone(uint8_t bend_amount)
{
	uint16_t count = pgm_read_word(&(scale[current_tone])[bend_amount])>>current_octave;
	cli();
	//TCNT1 = 0;
	OCR1A = count;
	OCR1B = count-100;
	sei();
}

//
// Stopp spillende tone
//
void stopTone()
{
	PORTA |= (1<<FLOPPY_ENABLE)|(1<<FLOPPY_MOTOR);
}

uint8_t stepcount;

//
// Interrupt på slutten av hvert klikk
//
ISR(TIM1_OVF_vect)
{
	if(!(PINA&((1<<FLOPPY_ENABLE)|(1<<FLOPPY_MOTOR))))
	{
		stepcount++;
		if(stepcount >= 160)
		{
			stepcount = 0;
			PORTA |= (1<<FLOPPY_DIR);
		}
		else if(stepcount == 80)
		{
			PORTA &= ~(1<<FLOPPY_DIR);
		}
	}
}

ISR(INT0_vect)
{
	stopTone();
}