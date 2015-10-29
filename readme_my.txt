打印出的变量有6个分别是cs ht 累加dmac phy_rate dmaci te-th
其中phy_rate dmaci te-th 三个变量是在一行打印出来的
eg
[ 6472.520000] [ampdu]1442406342.133670175,722,0.221279,0.305018
[ 6472.690000] [ampdu]1442406342.134500504,722,0.202161,0.285344
[ 6496.290000] [unampdu]1442406365.872802738,300,0.125632882,0.125740482

其中ampdu 和 unampdu表示是否是聚合包你可以不用管作为匹配字段就行
后面就是te,rate,dmaci,te-th 注意其中te dmaci te-th 都是精确到小数点后9位
然后是cs ht 累加dmac打印

eg
[  119.610000] gamma:0->1442334785
[  119.610000] OVERALL TRANSMITTING TIME:
[  119.610000]    0 second(s) + 6147685 nanoseconds
[  119.610000] OVERALL BUSYWAIT TIME:
[  119.610000]    0 second(s) + 5839285 nanoseconds
[  119.610000] 
[  119.610000] CS:
[  119.610000]    0 second(s) + 583928 nanoseconds
[  119.610000]    0 second(s) + 992678 nanoseconds
[  119.610000]    0 second(s) + 1109464 nanoseconds
[  119.610000]    0 second(s) + 2043749 nanoseconds
[  119.610000]    0 second(s) + 992678 nanoseconds
[  119.610000] 
[  119.610000] HT:0 second(s) + 0 nanoseconds

其中gamma 表示时间的范围用时间戳表示，如果是0 就舍弃，取结束的时间戳作为这个时间点 
后面busywait time就是累计dmac
cs ht就在后面
每次一定是这种格式打印的

