#include <linux/time.h>
#include <byteswap.h>
#define le64toh(x) bswap_64(x)
#define le32toh(x) bswap_32(x)
#define le16toh(x) bswap_16(x)

//#define tolower(c)     c - 'A' + 'a'
#define MAC_LEN			6
#define HOLD_TIME       1
#define CS_NUMBER       1 
#define CONST_TIME_24   70
#define CONST_TIME_5    76    //50+16+32Byte*8/24Mbps   
#define NUM_MICROS_PER_SECOND 1e6
#define NUM_NANO_PER_SECOND   1e9

#define PHY_FLAG_SHORTPRE	0x0001
#define PHY_FLAG_BADFCS		0x0002
#define PHY_FLAG_A		0x0010
#define PHY_FLAG_B		0x0020
#define PHY_FLAG_G		0x0040
#define PHY_FLAG_MODE_MASK	0x00f0

const static char mac_zero[12] = "000000000000";
const static char mac_ffff[12] = "FFFFFFFFFFFF";
static int inf_end_timestamp = 0;
static int inf_start_timestamp = 0;

static int FREQUENT_UPDATE_PERIOD_SECONDS = 10;
//int tolower(char c){
//	return c-'A' + 'a';
//}
struct inf_info {
	int  value;
	int percentage;
	unsigned char wlan_src[MAC_LEN];
	unsigned char wlan_dst[MAC_LEN];
};
struct summary_info{
	int sniffer_bytes;
	int  inf_packets;
	int  mine_packets;
	int inf_bytes;
	int mine_bytes;
	int overall_extra_time;
	int overall_busywait;
	int inf_num;
};
struct packet_info {
	/* general */
	struct timeval tv;
	int len;
	/*wlan phy*/
	int phy_signal;
	unsigned int phy_rate;

	/* wlan mac */
	u16		wlan_type;	/* frame control field */
	unsigned char		wlan_src[MAC_LEN];
	unsigned char		wlan_dst[MAC_LEN];
	unsigned int		wlan_retry;
	int phy_noise;
	unsigned int phy_snr;
	unsigned int		wlan_nav;
	u_int64_t		timestamp;	/* timestamp from mactime */
};

/*global struct*/

extern struct packet_info store[HOLD_TIME];
extern int current_index ;
extern int debug_index;
extern struct inf_info cs[CS_NUMBER]; 
extern struct summary_info summary;
extern struct packet_info last_p;
extern struct packet_info ppp;
extern int ht;

/*declaration of function*/
//extern int parse_80211_header(const unsigned char * buf,  struct packet_info* p);
//extern int parse_radiotap_header(unsigned char * buf,  struct packet_info* p);
int cal_inf(struct packet_info * p);


