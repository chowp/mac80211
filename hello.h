#ifndef HELLO_H
#define HELLO_H
#include <byteswap.h>
#define le64toh(x) bswap_64(x)
#define le32toh(x) bswap_32(x)
#define le16toh(x) bswap_16(x)

#define tolower(c)     c - 'A' + 'a'
#define MAC_LEN			6
#define HOLD_TIME       1000
#define CS_NUMBER       100
#define CONST_TIME_24   0.000070
#define CONST_TIME_5    0.000076    //50+16+32Byte*8/24Mbps   

#define PHY_FLAG_SHORTPRE	0x0001
#define PHY_FLAG_BADFCS		0x0002
#define PHY_FLAG_A		0x0010
#define PHY_FLAG_B		0x0020
#define PHY_FLAG_G		0x0040
#define PHY_FLAG_MODE_MASK	0x00f0

#define NUM_MICROS_PER_SECOND 1e6
#define NUM_NANO_PER_SECOND   1e9

static char mac_zero[12] = "000000000000";
static char mac_ffff[12] = "FFFFFFFFFFFF";
static int inf_end_timestamp = 0;
static int inf_start_timestamp = 0;

static int FREQUENT_UPDATE_PERIOD_SECONDS;
struct inf_info {
	unsigned char wlan_src[MAC_LEN];
	unsigned char wlan_dst[MAC_LEN];
	float  value;
	float percentage;
};
struct summary_info{
	float sniffer_bytes;
	float inf_packets;
	float mine_packets;
	float inf_bytes;
	float mine_bytes;
	float overall_extra_time;
	double overall_busywait;
	int inf_num;
};
/***************
 store the delay break down info
****************/
struct delay_info {
	float udelay;
	float ddelay;
	float rtt;
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
extern int current_index;
extern struct inf_info cs[CS_NUMBER]; /* used to store cs info in time gamma */
extern struct summary_info summary;
extern struct packet_info last_p;
extern struct packet_info ppp;
extern float ht;
/*declaration of function*/
int parse_80211_header(const unsigned char * buf,  struct packet_info* p);
static int parse_radiotap_header(unsigned char * buf,  struct packet_info* p);
int cal_inf(struct packet_info * p);


#endif
