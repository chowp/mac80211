#ifndef HELLO_H
#define HELLO_H

#include <stdlib.h>
#include <sys/time.h>

#define MAC_LEN			6
#define HOLD_TIME       1000
#define CS_NUMBER       100
#define CONST_TIME_24   0.000070
#define CONST_TIME_5    0.000076    //50+16+32Byte*8/24Mbps   

#define NUM_MICROS_PER_SECOND 1e6
#define NUM_NANO_PER_SECOND   1e9

static char mac_zero[12] = "000000000000";
static char mac_ffff[12] = "FFFFFFFFFFFF";


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
	int recved;
	int dropped;
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

	u_int64_t		timestamp;	/* timestamp from mactime */
};

/*global struct*/
extern struct packet_info store[HOLD_TIME];
extern int current_index;
extern struct inf_info cs[CS_NUMBER]; /* used to store cs info in time gamma */
extern struct summary_info summary;
extern struct packet_info last_p;
/* rate in 100kbps */
int
rate_to_index(int rate)
{
	switch (rate) {
		case 540: return 12;
		case 480: return 11;
		case 360: return 10;
		case 240: return 9;
		case 180: return 8;
		case 120: return 7;
		case 110: return 6;
		case 90: return 5;
		case 60: return 4;
		case 55: return 3;
		case 20: return 2;
		case 10: return 1;
		default: return 0;
	}
}

/*
To check whether the current packet is in the cs list(\gamma) 
*/
bool matched(struct inf_info *inf,int i, unsigned char mac1[], unsigned char mac2[]){
	if ( (str_equal(ether_sprintf(mac1),ether_sprintf2(inf[i].wlan_src),2*MAC_LEN) != 1) &&
	   (str_equal(ether_sprintf(mac1),ether_sprintf2(inf[i].wlan_dst),2*MAC_LEN) != 1) ) 
		return false;
	if ( (str_equal(ether_sprintf(mac2),ether_sprintf2(inf[i].wlan_src),2*MAC_LEN) != 1) &&
	   (str_equal(ether_sprintf(mac2),ether_sprintf2(inf[i].wlan_dst),2*MAC_LEN) != 1) ) 
		return false;
	return true;
}
/*
To judge whether the current packet are broadcast, cts, ack or control packet(\gamma) 
*/
bool non_control_packet(struct inf_info *inf,unsigned char mac1[], unsigned char mac2[]){
	if (str_equal(mac_zero,ether_sprintf(mac1),2*MAC_LEN) == 1)
		return false;
	if (str_equal(mac_zero,ether_sprintf(mac2),2*MAC_LEN) == 1)
		return false;
	return true;
}
/*
Insert a packet to the carrier sense or hidden teriminal list
*/
bool update_list(struct inf_info *inf,int NUMBER, unsigned char mac1[], unsigned char mac2[], float value){	
	//if (debug == LOG_DEBUG)
	//	printf("neighbor packets width %s+%s:%f\n",ether_sprintf(mac1),ether_sprintf2(mac2),value);
	//printf("\n*******************************\n");

	int i;
	for(i=0;i<NUMBER;i++){
		//printf("*  %s+%s:%f\n",ether_sprintf(inf[i].wlan_src),ether_sprintf2(inf[i].wlan_dst),inf[i].value);
		if (inf[i].value == 0)
			break;
		if (!non_control_packet(inf,mac1,mac2)){
			continue;
		} 
		if (matched(inf,i,mac1,mac2)){
			inf[i].value = inf[i].value + value;
		}
	}
	/* there is no match!!*/
	for(i=0;i<NUMBER;i++)
	{
		if (inf[i].value ==0 )
		{
			memcpy(inf[i].wlan_src,mac1,MAC_LEN);
			memcpy(inf[i].wlan_dst,mac2,MAC_LEN);
			
			inf[i].value = value;
			summary.inf_num = summary.inf_num + 1;
			//printf("C  %s+%s:%f\n",ether_sprintf(mac1),ether_sprintf2(mac2),inf[i].value);
			//printf("*******************************\n\n");
			return; /*pay attention whether it will jump out!*/
		}
	}
}

static void print_summay(){
	printf("\ninterferes          =%d\n",summary.inf_num);
	printf("mine_packets        =%.1f\n",summary.mine_packets);
	printf("inf_packets         =%.1f\n",summary.inf_packets);
	printf("overall_tx_airtime  =%.2f seconds\n",summary.overall_extra_time);
	printf("overall_busywait    =%.2f seconds\n",summary.overall_busywait);
	printf("mine_throughput     =%.2f KB/s\n",(float)summary.mine_bytes*0.001/(float)FREQUENT_UPDATE_PERIOD_SECONDS);
	printf("inf_throughput      =%.2f KB/s\n",(float)summary.inf_bytes*0.001/(float)FREQUENT_UPDATE_PERIOD_SECONDS);
	printf("sniffer_throughput  =%.2f KB/s\n",(double)summary.sniffer_bytes*0.001/(double)FREQUENT_UPDATE_PERIOD_SECONDS);
}
static void reset_summary(){
	summary.mine_bytes = 0;
	summary.mine_packets = 0;
	summary.inf_bytes = 0;
	summary.inf_packets = 0;
	summary.inf_num =0;
	summary.overall_busywait = 0;
	summary.overall_extra_time = 0;
	summary.sniffer_bytes = 0;
	summary.recved = 0;
	summary.dropped = 0;
}

/*
print out the carrier sense's interference seperately
*/
static void print_inf() {
 	
	printf("\nCS:");	
	for(j = 0 ; j < CS_NUMBER ; j ++){
		if (cs[j].value == 0)
			break;
		cs[j].percentage = 100.0*((double)cs[j].value/(double)summary.overall_busywait);
		printk(KERN_DEBUG "%.1f%%,",cs[j].percentage); 
	}

	printk(KERN_DEBUG "\nHT,%lf,%lf,%f\n",
			inf_start_timestamp,inf_end_timestamp,
			ht_sum);
  	fclose(handle);

	// print summary info
	print_summay();
	
  	memset(&summary, 0, sizeof(summary));
	summary.recved = recv;
	summary.dropped = drop;

}

int cal_inf(struct packet_info p){
	double te = (double)p.timestamp/(double)NUM_NANO_PER_SECOND;
	double tw = p.tv.tv_sec + (double)p.tv.tv_usec/(double)NUM_MICROS_PER_SECOND;
	float th = tw;
	double last_tw = last_p.tv.tv_sec + (double)last_p.tv.tv_usec/(double)NUM_MICROS_PER_SECOND;
	double dmaci = te - th - (double)p.len*8*10/(float)p.phy_rate - CONST_TIME_24; 
	if (last_tw > th){
		th = last_tw;
	}
	float overall_busywait = 0;
	int j = 0;
	/*first round*/
	for (j =current_index;j>=0; j--){
		double tr = store[j].tv.tv_sec + (double)store[j].tv.tv_usec/(double)NUM_MICROS_PER_SECOND; 
	
		if ((tr > th) && (tr < te)){
			float busywait = (float)store[j].len * 8 * 10 / (float)store[j].phy_rate;
			busywait = busywait/(float)NUM_MICROS_PER_SECOND;
			overall_busywait = overall_busywait + busywait
		}
	}
	/*second round*/
	for (j =current_index;j>=0; j--){
		double tr = store[j].tv.tv_sec + (double)store[j].tv.tv_usec/(double)NUM_MICROS_PER_SECOND; 
	
		if ((tr > th) && (tr < te)){
			double busywait = (double)store[j].len * 8 * 10 / (double)store[j].phy_rate;
			double inf = busywait/dmaci;
			if ( p.wlan_retry == 0){ 
				update_list(cs,CS_NUMBER,store[j].wlan_src,store[j].wlan_dst,inf);
			}
			else{
				ht = ht + te - th;
			}
		}
	}

	inf_end_timestamp = p.tv.tv_sec + (double)p.tv.tv_usec/(double)NUM_MICROS_PER_SECOND;
	//printf("start time is %f, end time is %f\n",inf_start_timestamp,inf_end_timestamp);
	if ((inf_end_timestamp - inf_start_timestamp) > FREQUENT_UPDATE_PERIOD_SECONDS)
	{
		/*print out*/
		print_inf();
		memset(cs,0,sizeof(cs));
		ht = 0;
		inf_start_timestamp = inf_end_timestamp;
	}

}

/* return rate in 100kbps */
int
rate_index_to_rate(unsigned int idx)
{
	switch (idx) {
		case 12: return 540;
		case 11: return 480;
		case 10: return 360;
		case 9: return 240;
		case 8: return 180;
		case 7: return 120;
		case 6: return 110;
		case 5: return 90;
		case 4: return 60;
		case 3: return 55;
		case 2: return 20;
		case 1: return 10;
		default: return 0;
	}
}

/* return rate in 100kbps */
int
mcs_index_to_rate(int mcs, int ht20, int lgi)
{
	/* MCS Index, http://en.wikipedia.org/wiki/IEEE_802.11n-2009#Data_rates */
	switch (mcs) {
		case 0:  return ht20 ? (lgi ? 65 : 72) : (lgi ? 135 : 150);
		case 1:  return ht20 ? (lgi ? 130 : 144) : (lgi ? 270 : 300);
		case 2:  return ht20 ? (lgi ? 195 : 217) : (lgi ? 405 : 450);
		case 3:  return ht20 ? (lgi ? 260 : 289) : (lgi ? 540 : 600);
		case 4:  return ht20 ? (lgi ? 390 : 433) : (lgi ? 810 : 900);
		case 5:  return ht20 ? (lgi ? 520 : 578) : (lgi ? 1080 : 1200);
		case 6:  return ht20 ? (lgi ? 585 : 650) : (lgi ? 1215 : 1350);
		case 7:  return ht20 ? (lgi ? 650 : 722) : (lgi ? 1350 : 1500);
		case 8:  return ht20 ? (lgi ? 130 : 144) : (lgi ? 270 : 300);
		case 9:  return ht20 ? (lgi ? 260 : 289) : (lgi ? 540 : 600);
		case 10: return ht20 ? (lgi ? 390 : 433) : (lgi ? 810 : 900);
		case 11: return ht20 ? (lgi ? 520 : 578) : (lgi ? 1080 : 1200);
		case 12: return ht20 ? (lgi ? 780 : 867) : (lgi ? 1620 : 1800);
		case 13: return ht20 ? (lgi ? 1040 : 1156) : (lgi ? 2160 : 2400);
		case 14: return ht20 ? (lgi ? 1170 : 1300) : (lgi ? 2430 : 2700);
		case 15: return ht20 ? (lgi ? 1300 : 1444) : (lgi ? 2700 : 3000);
		case 16: return ht20 ? (lgi ? 195 : 217) : (lgi ? 405 : 450);
		case 17: return ht20 ? (lgi ? 39 : 433) : (lgi ? 810 : 900);
		case 18: return ht20 ? (lgi ? 585 : 650) : (lgi ? 1215 : 1350);
		case 19: return ht20 ? (lgi ? 78 : 867) : (lgi ? 1620 : 1800);
		case 20: return ht20 ? (lgi ? 1170 : 1300) : (lgi ? 2430 : 2700);
		case 21: return ht20 ? (lgi ? 1560 : 1733) : (lgi ? 3240 : 3600);
		case 22: return ht20 ? (lgi ? 1755 : 1950) : (lgi ? 3645 : 4050);
		case 23: return ht20 ? (lgi ? 1950 : 2167) : (lgi ? 4050 : 4500);
		case 24: return ht20 ? (lgi ? 260 : 288) : (lgi ? 540 : 600);
		case 25: return ht20 ? (lgi ? 520 : 576) : (lgi ? 1080 : 1200);
		case 26: return ht20 ? (lgi ? 780 : 868) : (lgi ? 1620 : 1800);
		case 27: return ht20 ? (lgi ? 1040 : 1156) : (lgi ? 2160 : 2400);
		case 28: return ht20 ? (lgi ? 1560 : 1732) : (lgi ? 3240 : 3600);
		case 29: return ht20 ? (lgi ? 2080 : 2312) : (lgi ? 4320 : 4800);
		case 30: return ht20 ? (lgi ? 2340 : 2600) : (lgi ? 4860 : 5400);
		case 31: return ht20 ? (lgi ? 2600 : 2888) : (lgi ? 5400 : 6000);
	}
	return 0;
}


const char*
ether_sprintf(const unsigned char *mac)
{
	static char etherbuf[13];
	snprintf(etherbuf, sizeof(etherbuf), "%02x%02x%02x%02x%02x%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return etherbuf;
}

const char*
ether_sprintf2(const unsigned char *mac)
{
	static char etherbuf2[13];
	snprintf(etherbuf2, sizeof(etherbuf2), "%02x%02x%02x%02x%02x%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return etherbuf2;
}

int ieee80211_get_hdrlen(u16 fc)
{
	int hdrlen = 24;

	switch (fc & IEEE80211_FCTL_FTYPE) {
	case IEEE80211_FTYPE_DATA:
		if ((fc & IEEE80211_FCTL_FROMDS) && (fc & IEEE80211_FCTL_TODS))
			hdrlen = 30; /* Addr4 */
		/*
		 * The QoS Control field is two bytes and its presence is
		 * indicated by the IEEE80211_STYPE_QOS_DATA bit. Add 2 to
		 * hdrlen if that bit is set.
		 * This works by masking out the bit and shifting it to
		 * bit position 1 so the result has the value 0 or 2.
		 */
		hdrlen += (fc & IEEE80211_STYPE_QOS_DATA) >> 6;
		break;
	case IEEE80211_FTYPE_CTL:
		/*
		 * ACK and CTS are 10 bytes, all others 16. To see how
		 * to get this condition consider
		 *   subtype mask:   0b0000000011110000 (0x00F0)
		 *   ACK subtype:    0b0000000011010000 (0x00D0)
		 *   CTS subtype:    0b0000000011000000 (0x00C0)
		 *   bits that matter:         ^^^      (0x00E0)
		 *   value of those: 0b0000000011000000 (0x00C0)
		 */
		if ((fc & 0xE0) == 0xC0)
			hdrlen = 10;
		else
			hdrlen = 16;
		break;
	}

	return hdrlen;
}
	  

const char*
digest_sprintf16(const unsigned char *mac)   
{
	static char etherbuf[33];
	snprintf(etherbuf, sizeof(etherbuf), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],mac[6]
		,mac[7], mac[8], mac[9], mac[10], mac[11], mac[12],mac[13]
		,mac[14], mac[15]);
	return etherbuf;
}
const char*
digest_sprintf30(const unsigned char *mac)   
{
	static char etherbuf[61];
	snprintf(etherbuf, sizeof(etherbuf), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],mac[6]
		,mac[7], mac[8], mac[9], mac[10], mac[11], mac[12],mac[13]
		,mac[14], mac[15], mac[16], mac[17], mac[18], mac[19]
		,mac[20], mac[21], mac[22], mac[23], mac[24], mac[25]
		,mac[26], mac[27], mac[28], mac[29]);
	return etherbuf;
}

int str_equal(const unsigned char *s1,const unsigned char *s2,int len){
	int i ;
	for (i = 0; i < len ; i++)
	{
		if(( s1[i] != s2[i] )&&(tolower(s1[i]) != s2[i]))
			return 0;
	}
	return 1;
}

#endif
