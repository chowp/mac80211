#include "hello.h"
#include <stdlib.h>
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
int parse_80211_header(const unsigned char * buf,  struct packet_info* p)
{

        struct ieee80211_hdr* wh;
        struct ieee80211_mgmt* whm;
        int hdrlen = 0;
        u8* sa = NULL;
        u8* da = NULL;
        u16 fc;
        //u16 type;



        wh = (struct ieee80211_hdr*)buf;
        fc = le16toh(wh->frame_control);
        //hdrlen = ieee80211_get_hdrlen(fc); //no need

        p->wlan_type = (fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE));
        //type = (fc & (IEEE80211_FCTL_FTYPE | IEEE80211_FCTL_STYPE));

        switch (p->wlan_type & IEEE80211_FCTL_FTYPE) {
        case IEEE80211_FTYPE_DATA:
                hdrlen = 24;
                switch (p->wlan_type & IEEE80211_FCTL_STYPE) {
                case IEEE80211_STYPE_NULLFUNC:
                        break;
                case IEEE80211_STYPE_QOS_DATA:
                        hdrlen = 26;
                        break;
                }

                p->wlan_nav = le16toh(wh->duration_id);

                sa = ieee80211_get_SA(wh);
                da = ieee80211_get_DA(wh);

                if (fc & IEEE80211_FCTL_PROTECTED)
                       hdrlen = 34;
                if (fc & IEEE80211_FCTL_RETRY)
                        p->wlan_retry = 1;

                break;

        case IEEE80211_FTYPE_CTL:
                switch (p->wlan_type & IEEE80211_FCTL_STYPE) {
                case IEEE80211_STYPE_RTS:
                        p->wlan_nav = le16toh(wh->duration_id);
                        sa = wh->addr2;
                        da = wh->addr1;
                        break;

                case IEEE80211_STYPE_CTS:
                        p->wlan_nav = le16toh(wh->duration_id);
                        da = wh->addr1;
                        break;

                case IEEE80211_STYPE_ACK:
                        p->wlan_nav = le16toh(wh->duration_id);
                        da = wh->addr1;
                        break;

                case IEEE80211_STYPE_PSPOLL:
                        sa = wh->addr2;
                        break;

                case IEEE80211_STYPE_CFEND:
                case IEEE80211_STYPE_CFENDACK:
                        da = wh->addr1;
                        sa = wh->addr2;
                        break;
              case IEEE80211_STYPE_BACK_REQ:
                case IEEE80211_STYPE_BACK:
                        p->wlan_nav = le16toh(wh->duration_id);
                        da = wh->addr1;
                        sa = wh->addr2;
                }
                break;

        case IEEE80211_FTYPE_MGMT:
                //hdrlen = 24;
                whm = (struct ieee80211_mgmt*)buf;
                sa = whm->sa;
                da = whm->da;
                if (fc & IEEE80211_FCTL_RETRY)
                        p->wlan_retry = 1;
                switch ( p->wlan_type & IEEE80211_FCTL_STYPE) {
                case IEEE80211_STYPE_BEACON:
                case IEEE80211_STYPE_PROBE_RESP:
/*              {
                        if(debug == 1)
                                printf("begin getting timestamp!\n");
                        struct wlan_frame_beacon* bc = (struct wlan_frame_beacon*)((buf + 24));
                        p->wlan_tsf = le64toh(bc->tsf);
                        if(debug == 1)
                                printf("find a beacon!!\n");
                        break;
                }*/
                case IEEE80211_STYPE_PROBE_REQ:
                case IEEE80211_STYPE_ASSOC_REQ:
                case IEEE80211_STYPE_ASSOC_RESP:
                case IEEE80211_STYPE_REASSOC_REQ:
                case IEEE80211_STYPE_REASSOC_RESP:
                case IEEE80211_STYPE_DISASSOC:
                case IEEE80211_STYPE_AUTH:
                case IEEE80211_STYPE_DEAUTH:
                        break;
                }
                break;

        }

        if (sa != NULL) {
                memcpy(p->wlan_src, sa, MAC_LEN);
        }
        if (da != NULL) {
                memcpy(p->wlan_dst, da, MAC_LEN);
        }

        return hdrlen;

}
static int
parse_radiotap_header(unsigned char * buf,  struct packet_info* p)
{
        struct ieee80211_radiotap_header* rh;
        __le32 present; /* the present bitmap */
        unsigned char* b; /* current byte */
        int i;
        u16 rt_len, x;
        unsigned char known, flags, ht20, lgi;

        unsigned int pch_flags;


        rh = (struct ieee80211_radiotap_header*)buf;
        b = buf + sizeof(struct ieee80211_radiotap_header);
        present = le32toh(rh->it_present);
        rt_len = le16toh(rh->it_len);

        /* check for header extension - ignore for now, just advance current position */
        while (present & 0x80000000  && b - buf < rt_len) {
                present = le32toh(*(__le32*)b);
                b = b + 4;
        }
        present = le32toh(rh->it_present); // in case it moved
        /* radiotap bitmap has 32 bit, but we are only interrested until
         * bit 19 (IEEE80211_RADIOTAP_MCS) => i<20 */
        for (i = 0; i < 20 && b - buf < rt_len; i++) {
                if ((present >> i) & 1) {

                        switch (i) {
                                /* just ignore the following (advance position only) */
                                case IEEE80211_RADIOTAP_TSFT:

                                        p->timestamp = le64toh(*(u_int64_t*)b);//changhua
                                        b = b + 8;
                                        break;
                                case IEEE80211_RADIOTAP_DBM_TX_POWER:
                                case IEEE80211_RADIOTAP_ANTENNA:

                                case IEEE80211_RADIOTAP_RTS_RETRIES:
                                case IEEE80211_RADIOTAP_DATA_RETRIES:

                                        b++;
                                        break;
                                case IEEE80211_RADIOTAP_EXT:

                                        b = b + 4;
                                        break;
                                case IEEE80211_RADIOTAP_FHSS:
                                case IEEE80211_RADIOTAP_LOCK_QUALITY:
                                case IEEE80211_RADIOTAP_TX_ATTENUATION:
                                case IEEE80211_RADIOTAP_RX_FLAGS:
                                case IEEE80211_RADIOTAP_TX_FLAGS:
                                case IEEE80211_RADIOTAP_DB_TX_ATTENUATION:

                                        b = b + 2;
                                        break;
                                /* we are only interrested in these: */
                                case IEEE80211_RADIOTAP_RATE:
                                        p->phy_rate = (*b)*5; /* rate is in 500kbps */
                                        //p->phy_rate_idx = rate_to_index(p->phy_rate);
                                        b++;
                                        break;
                                case IEEE80211_RADIOTAP_DBM_ANTSIGNAL:
                                        p->phy_signal = *(char*)b;
                                        b++;
                                        break;
                                case IEEE80211_RADIOTAP_DBM_ANTNOISE:

                                        p->phy_noise = *(char*)b;
                                        b++;
                                        break;
                                case IEEE80211_RADIOTAP_DB_ANTSIGNAL:

                                        p->phy_snr = *b;
                                        b++;
                                        break;
                                case IEEE80211_RADIOTAP_FLAGS:
                                        /* short preamble */

                                        if (*b & IEEE80211_RADIOTAP_F_SHORTPRE) {
                                                pch_flags |= PHY_FLAG_SHORTPRE;

                                        }
                                        if (*b & IEEE80211_RADIOTAP_F_BADFCS) {
                                                pch_flags |= PHY_FLAG_BADFCS;

                                        }

                                        /*here to get the potential tcp seq, only the outgoing tcp packet is valibale*/
                                        b++;
                                        break;
                                case IEEE80211_RADIOTAP_CHANNEL:
                                        /* channel & channel type */
                                        if (((long)b)%2) b++; // align to 16 bit boundary

                                        b = b + 2;
                                        b = b + 2;
                                        break;
                                case IEEE80211_RADIOTAP_MCS:
                                        /* Ref http://www.radiotap.org/defined-fields/MCS */
                                        known = *b++;
                                        flags = *b++;


                                        if (known & IEEE80211_RADIOTAP_MCS_HAVE_BW)
                                                ht20 = (flags & IEEE80211_RADIOTAP_MCS_BW_MASK) == IEEE80211_RADIOTAP_MCS_BW_20;
                                        else
                                                ht20 = 1; /* assume HT20 if not present */

                                        if (known & IEEE80211_RADIOTAP_MCS_HAVE_GI)
                                                lgi = !(flags & IEEE80211_RADIOTAP_MCS_SGI);
                                        else
                                                lgi = 1; /* assume long GI if not present */



                                        //p->phy_rate_idx = 12 + *b;
                                        /*to fix the debug of openwrt*/
                                        if (*(b-1) == 0x27)
                                                b++;
                                        p->phy_rate = mcs_index_to_rate(*b, ht20, lgi);


                                        b++;
                                        break;
                        }
                }
                else {
                }
        }


        if (!(present & (1 << IEEE80211_RADIOTAP_DB_ANTSIGNAL))) {
                /* no SNR in radiotap, try to calculate */
                if (present & (1 << IEEE80211_RADIOTAP_DBM_ANTSIGNAL) &&
                    present & (1 << IEEE80211_RADIOTAP_DBM_ANTNOISE) &&
                    p->phy_noise < 0)
                        p->phy_snr = p->phy_signal - p->phy_noise;
                /* HACK: here we just assume noise to be -95dBm */
                else {
                        p->phy_snr = p->phy_signal + 95;
                        //simulate noise: p->phy_noise = -90;
                }
        }

        /* sanitize */
        if (p->phy_snr > 99)
                p->phy_snr = 99;
        if (p->phy_rate == 0 || p->phy_rate > 6000) {
                /* assume min rate for mode */
                if (pch_flags & PHY_FLAG_A)
                        p->phy_rate = 120; /* 6 * 2 */
                else if (pch_flags & PHY_FLAG_B)
                        p->phy_rate = 20; /* 1 * 2 */
                else if (pch_flags & PHY_FLAG_G)
                        p->phy_rate = 120; /* 6 * 2 */
                else
                        p->phy_rate = 20;
        }
        return rt_len;
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
        //      printf("neighbor packets width %s+%s:%f\n",ether_sprintf(mac1),ether_sprintf2(mac2),value);
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
        printk(KERN_DEBUG "\ninterferes          =%d\n",summary.inf_num);
        printk(KERN_DEBUG "mine_packets        =%.1f\n",summary.mine_packets);
        printk(KERN_DEBUG "inf_packets         =%.1f\n",summary.inf_packets);
        printk(KERN_DEBUG "overall_tx_airtime  =%.2f seconds\n",summary.overall_extra_time);
        printk(KERN_DEBUG "overall_busywait    =%.2f seconds\n",summary.overall_busywait);
        printk(KERN_DEBUG "mine_throughput     =%.2f KB/s\n",(float)summary.mine_bytes*0.001/(float)FREQUENT_UPDATE_PERIOD_SECONDS);
        printk(KERN_DEBUG "inf_throughput      =%.2f KB/s\n",(float)summary.inf_bytes*0.001/(float)FREQUENT_UPDATE_PERIOD_SECONDS);
        printk(KERN_DEBUG "sniffer_throughput  =%.2f KB/s\n",(double)summary.sniffer_bytes*0.001/(double)FREQUENT_UPDATE_PERIOD_SECONDS);
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
}

/*
print out the carrier sense's interference seperately
*/
static void print_inf() {
        int j;
        printk(KERN_DEBUG "\nCS:");
        for(j = 0 ; j < CS_NUMBER ; j ++){
                if (cs[j].value == 0)
                        break;
                cs[j].percentage = 100.0*((double)cs[j].value/(double)summary.overall_busywait);
                printk(KERN_DEBUG "%.1f%%,",cs[j].percentage);
        }

        printk(KERN_DEBUG "\nHT,%lf,%lf,%f\n",
                        inf_start_timestamp,inf_end_timestamp,
                        ht);

        // print summary info
        print_summay();

        memset(&summary, 0, sizeof(summary));

}
int cal_inf(struct packet_info * p){
        double te = (double)p->timestamp/(double)NUM_NANO_PER_SECOND;
        double tw = p->tv.tv_sec + (double)p->tv.tv_usec/(double)NUM_MICROS_PER_SECOND;
        float th = tw;
        double last_tw = last_p.tv.tv_sec + (double)last_p.tv.tv_usec/(double)NUM_MICROS_PER_SECOND;
        last_p = (*p);
        double dmaci = te - th - (double)p->len*8*10/(float)p->phy_rate/(double)NUM_MICROS_PER_SECOND - CONST_TIME_24;
        summary.overall_busywait = summary.overall_busywait + dmaci;
        summary.overall_extra_time = summary.overall_extra_time + te - tw;
        summary.mine_packets = summary.mine_packets + 1;
        summary.mine_bytes = summary.mine_bytes + p->len;
        if (last_tw > th){
                th = last_tw;
        }
        float overall_busywait = 0;
        int j = 0;
        /*first round*/
        for (j =current_index;; j=(j-1+HOLD_TIME)%HOLD_TIME){
                double tr = store[j].tv.tv_sec + (double)store[j].tv.tv_usec/(double)NUM_MICROS_PER_SECOND;

                if ((tr > th) && (tr < te)){
                        float busywait = (float)store[j].len * 8 * 10 / (float)store[j].phy_rate;
                        busywait = busywait/(float)NUM_MICROS_PER_SECOND;
                        if (p->wlan_retry == 0){
                                overall_busywait = overall_busywait + busywait;
                        }
                        summary.inf_packets = summary.inf_packets + 1;
                        summary.inf_bytes = summary.inf_bytes + store[j].len;
                }
                if ( tr < th ){
                        break;
                }
        }
        /*second round*/
        for (j =current_index;;  j=(j-1+HOLD_TIME)%HOLD_TIME){
                double tr = store[j].tv.tv_sec + (double)store[j].tv.tv_usec/(double)NUM_MICROS_PER_SECOND;

                if ((tr > th) && (tr < te)){
                        double busywait = (double)store[j].len * 8 * 10 / (double)store[j].phy_rate;
                        double inf = busywait/dmaci;
                        if ( p->wlan_retry == 0){
                                update_list(cs,CS_NUMBER,store[j].wlan_src,store[j].wlan_dst,inf);
                        }
                        else{
                                ht = ht + te - th;
                        }
                }
                if ( tr < th ){
                        break;
                }
        }

        inf_end_timestamp = p->tv.tv_sec + (double)p->tv.tv_usec/(double)NUM_MICROS_PER_SECOND;
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

