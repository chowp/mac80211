#include <net/ieee80211_radiotap.h>
#include <linux/ieee80211.h>
#include "hello.h"
//#include <time.h>
#define bool int
#define true 1
#define false 0

struct packet_info store[HOLD_TIME] = {0};
struct packet_info backup_store[HOLD_TIME] = {0};
int current_index = 0 ;
int previous_is_ampdu = 0;
struct inf_info cs[CS_NUMBER] = {0}; /* used to store cs info in time gamma */
struct summary_info summary= {0};
struct packet_info last_p={0};
struct packet_info ppp= {0};
struct timespec  ht = {0};
struct timespec  inf_start_timestamp = {0};
struct timespec  inf_end_timestamp  = {0};
struct mpdu ampdu={0};
/* rate in 100kbps */
int tolower(char c){
	if (c > 'A'){
		return c-'A'-'a';
	}else{
		return c;
	}
}

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
void
ether_sprintf(unsigned char mac[6], char output[13])
{
        snprintf(output, sizeof(output), "%02x%02x%02x%02x%02x%02x",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

char*
ether_sprintf2(unsigned char *mac)
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
char*
digest_sprintf16(const unsigned char *mac)
{
        char etherbuf[33];
        snprintf(etherbuf, sizeof(etherbuf), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],mac[6]
                ,mac[7], mac[8], mac[9], mac[10], mac[11], mac[12],mac[13]
                ,mac[14], mac[15]);
        return etherbuf;
}
char*
digest_sprintf30(const unsigned char *mac)
{
        char etherbuf[61];
        snprintf(etherbuf, sizeof(etherbuf), "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],mac[6]
                ,mac[7], mac[8], mac[9], mac[10], mac[11], mac[12],mac[13]
                ,mac[14], mac[15], mac[16], mac[17], mac[18], mac[19]
                ,mac[20], mac[21], mac[22], mac[23], mac[24], mac[25]
                ,mac[26], mac[27], mac[28], mac[29]);
        return etherbuf;
}

int str_equal(char *s1,char *s2,int len){
        int i ;
        for (i = 0; i < len ; i++)
        {
                if(( s1[i] != s2[i] )&&(tolower(s1[i]) != s2[i]))
                        return 0;
        }
        return 1;
}

/*
To check whether the current packet is in the cs list(\gamma)
*/
bool matched(char src[13], char dst[13], char  mac1[13],char mac2[13]){

	if ( (str_equal(mac1,src,2*MAC_LEN) != 1) &&
           (str_equal(mac1,dst,2*MAC_LEN) != 1) )
                return false;
        if ( (str_equal(mac2,src,2*MAC_LEN) != 1) &&
           (str_equal(mac2,dst,2*MAC_LEN) != 1) )
                return false;
        return true;
}
/*
To judge whether the current packet are broadcast, cts, ack or control packet(\gamma)
*/
bool non_control_packet(char mac1[13], char mac2[13]){
 	if (str_equal(mac_zero,mac1,2*MAC_LEN) == 1)
                return false;
   	if (str_equal(mac_zero,mac2,2*MAC_LEN) == 1)
                return false;
        return true;
}
/*
Insert a packet to the carrier sense or hidden teriminal list
*/
void update_list( unsigned char mac1[6], unsigned char mac2[6],struct timespec  value){
	int i;
        char mac11[13];
	char mac22[13];
	struct inf_info * tmp;
	ether_sprintf(mac1,mac11);
	ether_sprintf(mac2,mac22);
	printk(KERN_DEBUG "%s<->%s:+:%ld.%ld",mac11,mac22,value.tv_sec,value.tv_nsec);
	for(i=0;i<CS_NUMBER;i++){
                tmp = (struct inf_info *)&cs[i];
		if( (tmp->value.tv_nsec != 0) && 
		    (non_control_packet(mac11,mac22) == true) &&
                    (matched(tmp->wlan_src,tmp->wlan_dst,mac11,mac22) == true) ){
                       tmp->value = timespec_add(tmp->value,value);
                }
        }
        // there is no match!!
        for(i=0;i<CS_NUMBER;i++)
        {
                tmp = (struct inf_info *)&cs[i];
                if (cs[i].value.tv_nsec == 0 )
                {
                        memcpy(cs[i].wlan_src,mac1,MAC_LEN);
                        memcpy(cs[i].wlan_dst,mac2,MAC_LEN);

                        cs[i].value = value;
                        summary.inf_num = summary.inf_num + 1;
                        return; 
                }
        }
}
static void print_summay(){
        printk(KERN_EMERG "\n%ld.%ld->%ld.%ld\n",inf_start_timestamp.tv_sec,inf_start_timestamp.tv_nsec,inf_end_timestamp.tv_sec,inf_end_timestamp.tv_nsec);
        printk(KERN_EMERG "interferes          =%d\n",summary.inf_num);
        printk(KERN_EMERG "mine_packets        =%d\n",summary.mine_packets);
        printk(KERN_EMERG "inf_packets         =%d\n",summary.inf_packets);
        printk(KERN_EMERG "overall_tx_airtime  =%ld s\n",summary.overall_extra_time.tv_sec);
        printk(KERN_EMERG "overall_tx_airtime  =%ld ns\n",summary.overall_extra_time.tv_nsec);
        printk(KERN_EMERG "overall_busywait    =%ld s\n",summary.overall_busywait.tv_sec);
        printk(KERN_EMERG "overall_busywait    =%ld ns\n",summary.overall_busywait.tv_nsec);
        printk(KERN_EMERG "mine bytes          =%d Bytes\n",summary.mine_bytes);
        printk(KERN_EMERG "mine_throughput     =%d KB/s\n",(int)summary.mine_bytes/(int)(FREQUENT_UPDATE_PERIOD_SECONDS*1000));
        printk(KERN_EMERG "inf_throughput      =%d KB/s\n",(int)summary.inf_bytes/(int)(FREQUENT_UPDATE_PERIOD_SECONDS*1000));
        printk(KERN_EMERG "sniffer_throughput  =%d KB/s\n",(int)summary.sniffer_bytes/(int)(FREQUENT_UPDATE_PERIOD_SECONDS*1000));
}
static void reset_summary(){
        summary.mine_bytes = 0;
        summary.mine_packets = 0;
        summary.inf_bytes = 0;
        summary.inf_packets = 0;
        summary.inf_num =0;
        summary.overall_busywait.tv_sec = 0;
        summary.overall_busywait.tv_nsec = 0;
        summary.overall_extra_time.tv_sec = 0;
        summary.overall_extra_time.tv_nsec = 0;
        summary.sniffer_bytes = 0;
}

/*
print out the carrier sense's interference seperately
*/
static void print_inf() {
        int j;
        printk(KERN_DEBUG "\ngamma:%ld->%ld\n",inf_start_timestamp.tv_sec,inf_end_timestamp.tv_sec);
        printk(KERN_DEBUG "\nCS:\n");
        for(j = 0 ; j < CS_NUMBER ; j ++){
                if (cs[j].value.tv_nsec == 0)
                        break;
                printk(KERN_DEBUG "   %ld.%ld\n",cs[j].value.tv_sec,cs[j].value.tv_nsec);
        }

        printk(KERN_DEBUG "\nHT:%ld.%ld\n",ht.tv_sec,ht.tv_nsec);

}
void clear_timespec(struct timespec * test){
	test->tv_sec = 0;
	test->tv_nsec = 0;
}
void copy_timespec(struct timespec * dst, struct timespec * src){
	dst->tv_sec = src->tv_sec;
	dst->tv_nsec = src->tv_nsec;
}
struct timespec cal_dmaci_ampdu(){
	struct timespec transmit={0},tmp1={0},tmp2={0},difs={0},dmaci={0};
	ampdu.th = ampdu.tw;
	if (timespec_compare(&ampdu.th,&ampdu.last_te)<0){
		ampdu.th=ampdu.last_te;
	}
	transmit.tv_sec = 0;
	transmit.tv_nsec = ampdu.len*8*10*1000*ampdu.num/(ampdu.rate);
        difs.tv_sec =0;
	difs.tv_nsec = CONST_TIME_24*1000;
	tmp1 = timespec_sub(ampdu.te,ampdu.th);
	tmp2 = timespec_sub(tmp1,transmit);
	dmaci = timespec_sub(tmp2,difs);
//	printk(KERN_DEBUG "[ampdu][ ]:%ld.%ld->%ld.%ld,size=%d,rate=%d,previous_is_ampdu=%d,dmaci=%ld.%ld,mpdu_num=%d,th=%ld.%ld\n",ampdu.tw.tv_sec,ampdu.tw.tv_nsec,ampdu.te.tv_sec,ampdu.te.tv_nsec,ampdu.len,ampdu.rate,previous_is_ampdu,dmaci.tv_sec,dmaci.tv_nsec,ampdu.num,ampdu.th.tv_sec,ampdu.th.tv_nsec);
	return dmaci;
}

void update_summary(struct timespec dmaci,int len,int num){
	struct timespec tmp1={0};
	copy_timespec(&tmp1,&summary.overall_busywait);
	summary.overall_busywait = timespec_add(tmp1,dmaci);
        summary.mine_packets = summary.mine_packets + num;
        summary.mine_bytes = summary.mine_bytes + len;
}
void divide_inf(struct packet_info sniffer[],struct timespec th, struct timespec te, struct timespec dmaci,int retry,int ampdu_type){
        struct timespec tr={0},busywait={0},overall_busywait={0},inf={0},tmp1={0},tmp2={0};
        int j,jump,flag=0;
	int bj,ej = -1;
        //first round
/*	j = (current_index + 5)%HOLD_TIME;
	jump = 0;	
	for (;; j=(j-1+HOLD_TIME)%HOLD_TIME){
		jump = jump + 1;
		if (jump == 6){
			break; 
		}
		clear_timespec(&tr);
                tr = sniffer[j].te;
		printk(KERN_DEBUG "[%ld.%ld-- %d -->%ld.%ld] \t [Jump.%d,No.%d] %ld.%ld\n",th.tv_sec,th.tv_nsec,ampdu_type,te.tv_sec,te.tv_nsec,jump,j,tr.tv_sec,tr.tv_nsec);

	} */
	jump = 0;
	j = current_index;
	for (;; j=(j-1+HOLD_TIME)%HOLD_TIME){
		jump = jump + 1;
		if (jump == HOLD_TIME){
//			printk(KERN_DEBUG "first round traversed all the packets.\n");
			break; 
		}
		clear_timespec(&tr);
                tr = sniffer[j].te;
		//printk(KERN_DEBUG "[%ld.%ld-- %d -->%ld.%ld] \t [Jump.%d,No.%d] %ld.%ld\n",th.tv_sec,th.tv_nsec,ampdu_type,te.tv_sec,te.tv_nsec,jump,j,tr.tv_sec,tr.tv_nsec);
                if ((timespec_compare(&tr,&th)>0) && (timespec_compare(&tr ,&te)<0)){
			if (flag == 0){
				bj = j;
				flag = 1;
			}
			ej = j;
			clear_timespec(&busywait);
			busywait.tv_sec = 0;
			busywait.tv_nsec = sniffer[j].len * 8 * 10 *1000/(sniffer[j].phy_rate);
                        if (retry == 0){
                                overall_busywait = timespec_add(overall_busywait ,busywait);
                        }
                        summary.inf_packets = summary.inf_packets + 1;
                        summary.inf_bytes = summary.inf_bytes + sniffer[j].len;
                }
                if ( timespec_compare(&tr,&th)<0){
                        break;
                }
        }
	printk(KERN_DEBUG "\n[th------>te] : [%d,%d][%ld.%ld->%ld.%ld]\n",retry,ampdu_type,th.tv_sec,th.tv_nsec,te.tv_sec,te.tv_nsec);
	if(bj != -1){
		printk(KERN_DEBUG "[inf packets] : [%d,%d][%ld.%ld->%ld.%ld]\n",bj,ej,sniffer[bj].te.tv_sec,sniffer[bj].te.tv_nsec,sniffer[ej].te.tv_sec,sniffer[ej].te.tv_nsec);
	}
	printk(KERN_DEBUG "[sum tranmit] : %ld.%ld\n",overall_busywait.tv_sec,overall_busywait.tv_nsec);
/*	jump = 0;	
	for (;; j=(j-1+HOLD_TIME)%HOLD_TIME){
		jump = jump + 1;
		if (jump == 6){
			break; 
		}
		clear_timespec(&tr);
                tr = sniffer[j].te;
		printk(KERN_DEBUG "[%ld.%ld-- %d -->%ld.%ld] \t [Jump.%d,No.%d] %ld.%ld\n",th.tv_sec,th.tv_nsec,ampdu_type,te.tv_sec,te.tv_nsec,jump,j,tr.tv_sec,tr.tv_nsec);

	}*/
        //second round
	jump = 0;
        for (j =current_index;;  j=(j-1+HOLD_TIME)%HOLD_TIME){
                jump = jump +1;
		if (jump == HOLD_TIME){
//			printk(KERN_DEBUG "second round traversed all the packets.\n");
			break;
		}
		clear_timespec(&tr);
		tr=sniffer[j].te;

                if ((timespec_compare(&tr,&th)>0) && (timespec_compare(&tr ,&te)<0)){
			clear_timespec(&busywait);
			busywait.tv_sec = 0;
			busywait.tv_nsec = sniffer[j].len * 8 * 10 *1000/ sniffer[j].phy_rate;
                        int ratio = 100*(busywait.tv_sec*1000000000+busywait.tv_nsec)/(overall_busywait.tv_sec*1000000000+overall_busywait.tv_nsec);
	//		printk(KERN_DEBUG "[debug][busywait=%ld.%ld,ratio=%d,dmac=%ld.%ld]\n",busywait.tv_sec,busywait.tv_nsec,ratio,dmaci.tv_sec,dmaci.tv_nsec);
			clear_timespec(&inf);
			inf.tv_sec = dmaci.tv_sec*ratio/100;
			inf.tv_nsec = dmaci.tv_nsec*ratio/100; 
                        if ( retry == 0){
                                update_list(sniffer[j].wlan_src,sniffer[j].wlan_dst,inf);
                        }
                        else{
				clear_timespec(&tmp1);
				tmp1= timespec_sub(te,th);
				ht= timespec_add(ht,tmp1);
                        }
                }
                if ( timespec_compare(&tr,&th)<0){
                        break;
                }
        }
	
}
void check_print(struct packet_info *p){
        copy_timespec(&inf_end_timestamp,&p->te);
        //printf("start time is %f, end time is %f\n",inf_start_timestamp,inf_end_timestamp);
        if (timespec_sub(inf_end_timestamp,inf_start_timestamp).tv_sec > FREQUENT_UPDATE_PERIOD_SECONDS)
        {
                //print out
                print_inf();
		print_summay();
		memset(cs,0,sizeof(cs));
                ht.tv_sec = 0;
		ht.tv_nsec = 0;
		reset_summary();
                copy_timespec(&inf_start_timestamp,&inf_end_timestamp);
        }
}
void backup_sniffer_packet(struct timespec tw, struct timespec te, int ampdu_type){
        struct timespec tr;
	int j = 0;
        //first round
        int jump = 0;
	for (j =current_index;; j=(j-1+HOLD_TIME)%HOLD_TIME){
		jump = jump + 1;
		if (jump == HOLD_TIME){
//			printk(KERN_DEBUG "backup all the packets.\n");
			break; 
		}
		clear_timespec(&tr);
                tr = store[j].te;
//		printk(KERN_DEBUG "[backup][%ld.%ld-- %d -->%ld.%ld] \t [Jump.%d,No.%d] %ld.%ld\n",tw.tv_sec,tw.tv_nsec,ampdu_type,te.tv_sec,te.tv_nsec,jump,j,tr.tv_sec,tr.tv_nsec);
                if ((timespec_compare(&tr,&tw)>0) && (timespec_compare(&tr ,&te)<0)){
			backup_store[j] = store[j];
		}
	}
//	printk(KERN_DEBUG "now current_index come to %d\n",current_index);
}

int cal_inf(struct packet_info * p){
        struct timespec th={0},transmit={0},dmaci={0},tmp1={0},tmp2={0},difs={0},tr={0};
	if (previous_is_ampdu != 0){
		if (p->ampdu != 2){
			dmaci = cal_dmaci_ampdu();
			update_summary(dmaci,ampdu.len*ampdu.num,ampdu.num);
			divide_inf(backup_store,ampdu.th,ampdu.te,dmaci,0,1);
//			check_print(p);
		}
	}
	if(p->ampdu == 1){ //first packet of aggregation
		backup_sniffer_packet(p->tw,p->te,1);
		ampdu.rate = p->phy_rate;
		copy_timespec(&ampdu.te,&p->te);
		copy_timespec(&ampdu.last_te,&last_p.te);
		copy_timespec(&ampdu.tw,&p->tw);
		ampdu.num = 1;
		ampdu.len = p->len;
    		memcpy(&last_p,p,sizeof(last_p));
//		check_print(p);
//		printk(KERN_DEBUG "[  %d  ][%d]:%ld.%ld->%ld.%ld,size=%d,rate=%d,previous_is_ampdu=%d\n",p->ampdu,p->wlan_retry,p->tw.tv_sec,p->tw.tv_nsec,p->te.tv_sec,p->te.tv_nsec,p->len,p->phy_rate,previous_is_ampdu);
       		previous_is_ampdu = p->ampdu; 
	}else if(p->ampdu==2){ //rest of packets of aggregation
		p->phy_rate = ampdu.rate;
		copy_timespec(&ampdu.tw,&p->tw);
		ampdu.num = ampdu.num +  1;
//		printk(KERN_DEBUG "[  %d  ][%d]:%ld.%ld->%ld.%ld,size=%d,rate=%d,previous_is_ampdu=%d\n",p->ampdu,p->wlan_retry,p->tw.tv_sec,p->tw.tv_nsec,p->te.tv_sec,p->te.tv_nsec,p->len,p->phy_rate,previous_is_ampdu);
	}else{
		// non-aggregation packet
		th = p->tw;
		if (timespec_compare(&th,&last_p.te)<0){
			th=last_p.te;
		}
    		memcpy(&last_p,p,sizeof(last_p));
		if (p->tw.tv_nsec == 0){
       			previous_is_ampdu = p->ampdu;
			return; 
		}
		transmit.tv_sec = 0;
		transmit.tv_nsec = p->len*8*10*1000/(p->phy_rate);
        	difs.tv_sec =0;
		difs.tv_nsec = CONST_TIME_24*1000;
		tmp1 = timespec_sub(p->te,th);
		tmp2 = timespec_sub(tmp1,transmit);
		dmaci = timespec_sub(tmp2,difs);
		update_summary(dmaci,p->len,1);
		divide_inf(store,th,p->te,dmaci,p->wlan_retry,0);
//		check_print(p);
//	printk(KERN_DEBUG "[  %d  ][%d]:%ld.%ld->%ld.%ld,size=%d,rate=%d,previous_is_ampdu=%d,dmaci=%ld.%ld,th=%ld.%ld\n",p->ampdu,p->wlan_retry,p->tw.tv_sec,p->tw.tv_nsec,p->te.tv_sec,p->te.tv_nsec,p->len,p->phy_rate,previous_is_ampdu,dmaci.tv_sec,dmaci.tv_nsec,th.tv_sec,th.tv_nsec);
		previous_is_ampdu = p->ampdu; 
	}
//	printk(KERN_DEBUG "[  %d  ][%d]:%ld.%ld->%ld.%ld,size=%d,rate=%d,previous_is_ampdu=%d\n",p->ampdu,p->wlan_retry,p->tw.tv_sec,p->tw.tv_nsec,p->te.tv_sec,p->te.tv_nsec,p->len,p->phy_rate,previous_is_ampdu);

}
