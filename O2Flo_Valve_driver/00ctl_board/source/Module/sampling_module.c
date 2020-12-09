/*******************************************************************************
** 文件名: 		sampling_module.c
** 版本：  		1.0
** 工作环境: 	RealView MDK-ARM 4.20
** 作者: 		吴国炎
** 生成日期: 	2011-04-10
** 功能:		模版程序（用户可以在这里简单说明工程的功能）
** 相关文件:	无
** 修改日志：	2011-04-10   创建文档
*******************************************************************************/
/* 包含头文件 *****************************************************************/
#include "main.h"
#include "stm32f0xx_hal.h"
#include "cmsis_os.h"
#include "datatype.h"
#include "fifo.h"
#include "main.h"
#include "sampling_module.h"
#include "interface_module.h"
#include "comm_module.h"
#include "fifo.h"
#include "common.h"
#include "bsp_iic2_flow.h"
#include "sf05.h"
#include "comm_module.h"
#include "bsp_spi_flash.h"


/* 类型声明 ------------------------------------------------------------------*/

/* 宏定义 --------------------------------------------------------------------*/

/* 变量 ----------------------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart2;
extern ADC_HandleTypeDef hadc;
//extern DMA_HandleTypeDef hdma_adc;
extern uint16_t ADC_Value[4];
extern uint8_t ackerror;
extern uint16_t hardware_versions;



//采样模块
SAMPLING_MODULE sampling_module;
FIFO_TYPE comm_fifo;
uint16_t adc_data_buf[10] = {0};
uint16_t led_delay = 0;
uint32_t uiID = 0;
uint8_t out_flash = 0;
uint32_t write_data = 0xaaaaaaaa;
uint32_t read_data = 0;



const u16 NTC_Plate_and_Neb_TAB[1300] =	
{
	3950	,	3949	,	3948	,	3947	,	3946	,	3945	,	3945	,	3944	,	3943	,	3942	,	//	-25
	3941	,	3940	,	3939	,	3938	,	3938	,	3937	,	3936	,	3935	,	3934	,	3933	,	//	-24
	3932	,	3931	,	3930	,	3929	,	3928	,	3927	,	3926	,	3925	,	3925	,	3924	,	//	-23
	3923	,	3922	,	3921	,	3920	,	3919	,	3918	,	3917	,	3916	,	3915	,	3914	,	//	-22
	3913	,	3912	,	3911	,	3910	,	3908	,	3907	,	3906	,	3905	,	3904	,	3903	,	//	-21
	3902	,	3901	,	3900	,	3899	,	3898	,	3897	,	3896	,	3895	,	3894	,	3892	,	//	-20
	3891	,	3890	,	3889	,	3888	,	3887	,	3886	,	3885	,	3883	,	3882	,	3881	,	//	-19
	3880	,	3879	,	3878	,	3876	,	3875	,	3874	,	3873	,	3872	,	3870	,	3869	,	//	-18
	3868	,	3867	,	3866	,	3864	,	3863	,	3862	,	3861	,	3859	,	3858	,	3857	,	//	-17
	3856	,	3854	,	3853	,	3852	,	3851	,	3849	,	3848	,	3847	,	3845	,	3844	,	//	-16
	3843	,	3841	,	3840	,	3839	,	3837	,	3836	,	3835	,	3833	,	3832	,	3831	,	//	-15
	3829	,	3828	,	3827	,	3825	,	3824	,	3822	,	3821	,	3820	,	3818	,	3817	,	//	-14
	3815	,	3814	,	3812	,	3811	,	3809	,	3808	,	3807	,	3805	,	3804	,	3802	,	//	-13
	3801	,	3799	,	3798	,	3796	,	3795	,	3793	,	3792	,	3790	,	3789	,	3787	,	//	-12
	3785	,	3784	,	3782	,	3781	,	3779	,	3778	,	3776	,	3774	,	3773	,	3771	,	//	-11
	3770	,	3768	,	3766	,	3765	,	3763	,	3761	,	3760	,	3758	,	3757	,	3755	,	//	-10
	3753	,	3752	,	3750	,	3748	,	3746	,	3745	,	3743	,	3741	,	3740	,	3738	,	//	-9
	3736	,	3734	,	3733	,	3731	,	3729	,	3727	,	3726	,	3724	,	3722	,	3720	,	//	-8
	3718	,	3717	,	3715	,	3713	,	3711	,	3709	,	3707	,	3706	,	3704	,	3702	,	//	-7
	3700	,	3698	,	3696	,	3694	,	3693	,	3691	,	3689	,	3687	,	3685	,	3683	,	//	-6
	3681	,	3679	,	3677	,	3675	,	3673	,	3671	,	3669	,	3667	,	3665	,	3663	,	//	-5
	3661	,	3659	,	3657	,	3655	,	3653	,	3651	,	3649	,	3647	,	3645	,	3643	,	//	-4
	3641	,	3639	,	3637	,	3635	,	3633	,	3630	,	3628	,	3626	,	3624	,	3622	,	//	-3
	3620	,	3618	,	3616	,	3613	,	3611	,	3609	,	3607	,	3605	,	3602	,	3600	,	//	-2
	3598	,	3596	,	3594	,	3591	,	3589	,	3587	,	3585	,	3582	,	3580	,	3578	,	//	-1
	3576	,	3573	,	3571	,	3569	,	3566	,	3564	,	3562	,	3559	,	3557	,	3555	,	//	0
	3552	,	3550	,	3548	,	3545	,	3543	,	3540	,	3538	,	3536	,	3533	,	3531	,	//	1
	3528	,	3526	,	3524	,	3521	,	3519	,	3516	,	3514	,	3511	,	3509	,	3506	,	//	2
	3504	,	3501	,	3499	,	3496	,	3494	,	3491	,	3489	,	3486	,	3483	,	3481	,	//	3
	3478	,	3476	,	3473	,	3471	,	3468	,	3465	,	3463	,	3460	,	3457	,	3455	,	//	4
	3452	,	3450	,	3447	,	3444	,	3442	,	3439	,	3436	,	3433	,	3431	,	3428	,	//	5
	3425	,	3423	,	3420	,	3417	,	3414	,	3412	,	3409	,	3406	,	3403	,	3401	,	//	6
	3398	,	3395	,	3392	,	3389	,	3386	,	3384	,	3381	,	3378	,	3375	,	3372	,	//	7
	3369	,	3367	,	3364	,	3361	,	3358	,	3355	,	3352	,	3349	,	3346	,	3343	,	//	8
	3340	,	3337	,	3334	,	3331	,	3328	,	3326	,	3323	,	3320	,	3317	,	3314	,	//	9
	3311	,	3308	,	3305	,	3301	,	3298	,	3295	,	3292	,	3289	,	3286	,	3283	,	//	10
	3280	,	3277	,	3274	,	3271	,	3268	,	3265	,	3261	,	3258	,	3255	,	3252	,	//	11
	3249	,	3246	,	3243	,	3239	,	3236	,	3233	,	3230	,	3227	,	3223	,	3220	,	//	12
	3217	,	3214	,	3211	,	3207	,	3204	,	3201	,	3198	,	3194	,	3191	,	3188	,	//	13
	3184	,	3181	,	3178	,	3175	,	3171	,	3168	,	3165	,	3161	,	3158	,	3155	,	//	14
	3151	,	3148	,	3144	,	3141	,	3138	,	3134	,	3131	,	3128	,	3124	,	3121	,	//	15
	3117	,	3114	,	3110	,	3107	,	3104	,	3100	,	3097	,	3093	,	3090	,	3086	,	//	16
	3083	,	3079	,	3076	,	3072	,	3069	,	3065	,	3062	,	3058	,	3055	,	3051	,	//	17
	3048	,	3044	,	3041	,	3037	,	3033	,	3030	,	3026	,	3023	,	3019	,	3016	,	//	18
	3012	,	3008	,	3005	,	3001	,	2997	,	2994	,	2990	,	2987	,	2983	,	2979	,	//	19
	2976	,	2972	,	2968	,	2965	,	2961	,	2957	,	2954	,	2950	,	2946	,	2942	,	//	20
	2939	,	2935	,	2931	,	2928	,	2924	,	2920	,	2916	,	2913	,	2909	,	2905	,	//	21
	2901	,	2898	,	2894	,	2890	,	2886	,	2882	,	2879	,	2875	,	2871	,	2867	,	//	22
	2864	,	2860	,	2856	,	2852	,	2848	,	2844	,	2841	,	2837	,	2833	,	2829	,	//	23
	2825	,	2821	,	2817	,	2814	,	2810	,	2806	,	2802	,	2798	,	2794	,	2790	,	//	24
	2786	,	2782	,	2779	,	2775	,	2771	,	2767	,	2763	,	2759	,	2755	,	2751	,	//	25
	2747	,	2743	,	2739	,	2735	,	2731	,	2727	,	2723	,	2719	,	2716	,	2712	,	//	26
	2708	,	2704	,	2700	,	2696	,	2692	,	2688	,	2684	,	2680	,	2676	,	2672	,	//	27
	2668	,	2664	,	2660	,	2656	,	2652	,	2648	,	2644	,	2640	,	2636	,	2631	,	//	28
	2627	,	2623	,	2619	,	2615	,	2611	,	2607	,	2603	,	2599	,	2595	,	2591	,	//	29
	2587	,	2583	,	2579	,	2575	,	2571	,	2567	,	2562	,	2558	,	2554	,	2550	,	//	30
	2546	,	2542	,	2538	,	2534	,	2530	,	2526	,	2522	,	2518	,	2513	,	2509	,	//	31
	2505	,	2501	,	2497	,	2493	,	2489	,	2485	,	2481	,	2476	,	2472	,	2468	,	//	32
	2464	,	2460	,	2456	,	2452	,	2448	,	2443	,	2439	,	2435	,	2431	,	2427	,	//	33
	2423	,	2419	,	2415	,	2410	,	2406	,	2402	,	2398	,	2394	,	2390	,	2386	,	//	34
	2381	,	2377	,	2373	,	2369	,	2365	,	2361	,	2357	,	2352	,	2348	,	2344	,	//	35
	2340	,	2336	,	2332	,	2328	,	2323	,	2319	,	2315	,	2311	,	2307	,	2303	,	//	36
	2299	,	2294	,	2290	,	2286	,	2282	,	2278	,	2274	,	2270	,	2265	,	2261	,	//	37
	2257	,	2253	,	2249	,	2245	,	2241	,	2236	,	2232	,	2228	,	2224	,	2220	,	//	38
	2216	,	2212	,	2208	,	2203	,	2199	,	2195	,	2191	,	2187	,	2183	,	2179	,	//	39
	2174	,	2170	,	2166	,	2162	,	2158	,	2154	,	2150	,	2146	,	2142	,	2137	,	//	40
	2133	,	2129	,	2125	,	2121	,	2117	,	2113	,	2109	,	2105	,	2100	,	2096	,	//	41
	2092	,	2088	,	2084	,	2080	,	2076	,	2072	,	2068	,	2064	,	2060	,	2056	,	//	42
	2051	,	2047	,	2043	,	2039	,	2035	,	2031	,	2027	,	2023	,	2019	,	2015	,	//	43
	2011	,	2007	,	2003	,	1999	,	1995	,	1991	,	1987	,	1983	,	1978	,	1974	,	//	44
	1970	,	1966	,	1962	,	1958	,	1954	,	1950	,	1946	,	1942	,	1938	,	1934	,	//	45
	1930	,	1926	,	1922	,	1918	,	1914	,	1910	,	1906	,	1902	,	1898	,	1894	,	//	46
	1891	,	1887	,	1883	,	1879	,	1875	,	1871	,	1867	,	1863	,	1859	,	1855	,	//	47
	1851	,	1847	,	1843	,	1839	,	1835	,	1832	,	1828	,	1824	,	1820	,	1816	,	//	48
	1812	,	1808	,	1804	,	1800	,	1796	,	1793	,	1789	,	1785	,	1781	,	1777	,	//	49
	1773	,	1769	,	1766	,	1762	,	1758	,	1754	,	1751	,	1747	,	1743	,	1739	,	//	50
	1735	,	1732	,	1728	,	1724	,	1720	,	1717	,	1713	,	1709	,	1705	,	1702	,	//	51
	1698	,	1694	,	1690	,	1687	,	1683	,	1679	,	1676	,	1672	,	1668	,	1664	,	//	52
	1661	,	1657	,	1653	,	1650	,	1646	,	1642	,	1639	,	1635	,	1631	,	1628	,	//	53
	1624	,	1621	,	1617	,	1613	,	1610	,	1606	,	1602	,	1599	,	1595	,	1592	,	//	54
	1588	,	1584	,	1581	,	1577	,	1574	,	1570	,	1567	,	1563	,	1559	,	1556	,	//	55
	1552	,	1549	,	1545	,	1542	,	1538	,	1535	,	1531	,	1528	,	1524	,	1521	,	//	56
	1517	,	1514	,	1510	,	1507	,	1503	,	1500	,	1496	,	1493	,	1489	,	1486	,	//	57
	1483	,	1479	,	1476	,	1472	,	1469	,	1465	,	1462	,	1459	,	1455	,	1452	,	//	58
	1448	,	1445	,	1442	,	1438	,	1435	,	1432	,	1428	,	1425	,	1422	,	1418	,	//	59
	1415	,	1412	,	1408	,	1405	,	1402	,	1398	,	1395	,	1392	,	1388	,	1385	,	//	60
	1382	,	1379	,	1375	,	1372	,	1369	,	1366	,	1362	,	1359	,	1356	,	1353	,	//	61
	1349	,	1346	,	1343	,	1340	,	1337	,	1333	,	1330	,	1327	,	1324	,	1321	,	//	62
	1317	,	1314	,	1311	,	1308	,	1305	,	1302	,	1299	,	1295	,	1292	,	1289	,	//	63
	1286	,	1283	,	1280	,	1277	,	1274	,	1271	,	1267	,	1264	,	1261	,	1258	,	//	64
	1255	,	1252	,	1249	,	1246	,	1243	,	1240	,	1237	,	1234	,	1231	,	1228	,	//	65
	1225	,	1222	,	1219	,	1216	,	1213	,	1210	,	1207	,	1204	,	1201	,	1198	,	//	66
	1195	,	1192	,	1189	,	1186	,	1184	,	1181	,	1178	,	1175	,	1172	,	1169	,	//	67
	1166	,	1163	,	1160	,	1157	,	1155	,	1152	,	1149	,	1146	,	1143	,	1140	,	//	68
	1138	,	1135	,	1132	,	1129	,	1126	,	1124	,	1121	,	1118	,	1115	,	1112	,	//	69
	1110	,	1107	,	1104	,	1101	,	1099	,	1096	,	1093	,	1090	,	1088	,	1085	,	//	70
	1082	,	1079	,	1077	,	1074	,	1071	,	1069	,	1066	,	1063	,	1061	,	1058	,	//	71
	1055	,	1053	,	1050	,	1047	,	1045	,	1042	,	1039	,	1037	,	1034	,	1032	,	//	72
	1029	,	1026	,	1024	,	1021	,	1019	,	1016	,	1013	,	1011	,	1008	,	1006	,	//	73
	1003	,	1001	,	998 	,	996 	,	993 	,	991 	,	988 	,	986 	,	983 	,	980 	,	//	74
	978 	,	975 	,	973 	,	971 	,	968 	,	966 	,	963 	,	961 	,	958 	,	956 	,	//	75
	953 	,	951 	,	948 	,	946 	,	944 	,	941 	,	939 	,	936 	,	934 	,	932 	,	//	76
	929 	,	927 	,	925 	,	922 	,	920 	,	917 	,	915 	,	913 	,	910 	,	908 	,	//	77
	906 	,	903 	,	901 	,	899 	,	896 	,	894 	,	892 	,	890 	,	887 	,	885 	,	//	78
	883 	,	880 	,	878 	,	876 	,	874 	,	871 	,	869 	,	867 	,	865 	,	862 	,	//	79
	860 	,	858 	,	856 	,	854 	,	851 	,	849 	,	847 	,	845 	,	843 	,	840 	,	//	80
	838 	,	836 	,	834 	,	832 	,	830 	,	827 	,	825 	,	823 	,	821 	,	819 	,	//	81
	817 	,	815 	,	812 	,	810 	,	808 	,	806 	,	804 	,	802 	,	800 	,	798 	,	//	82
	796 	,	794 	,	792 	,	790 	,	788 	,	786 	,	783 	,	781 	,	779 	,	777 	,	//	83
	775 	,	773 	,	771 	,	769 	,	767 	,	765 	,	763 	,	761 	,	759 	,	757 	,	//	84
	755 	,	753 	,	751 	,	750 	,	748 	,	746 	,	744 	,	742 	,	740 	,	738 	,	//	85
	736 	,	734 	,	732 	,	730 	,	728 	,	726 	,	724 	,	723 	,	721 	,	719 	,	//	86
	717 	,	715 	,	713 	,	711 	,	709 	,	708 	,	706 	,	704 	,	702 	,	700 	,	//	87
	698 	,	697 	,	695 	,	693 	,	691 	,	689 	,	687 	,	686 	,	684 	,	682 	,	//	88
	680 	,	678 	,	677 	,	675 	,	673 	,	671 	,	670 	,	668 	,	666 	,	664 	,	//	89
	663 	,	661 	,	659 	,	657 	,	656 	,	654 	,	652 	,	651 	,	649 	,	647 	,	//	90
	645 	,	644 	,	642 	,	640 	,	639 	,	637 	,	635 	,	634 	,	632 	,	630 	,	//	91
	629 	,	627 	,	625 	,	624 	,	622 	,	621 	,	619 	,	617 	,	616 	,	614 	,	//	92
	612 	,	611 	,	609 	,	608 	,	606 	,	604 	,	603 	,	601 	,	600 	,	598 	,	//	93
	596 	,	595 	,	593 	,	592 	,	590 	,	589 	,	587 	,	586 	,	584 	,	583 	,	//	94
	581 	,	579 	,	578 	,	576 	,	575 	,	573 	,	572 	,	570 	,	569 	,	567 	,	//	95
	566 	,	564 	,	563 	,	561 	,	560 	,	559 	,	557 	,	556 	,	554 	,	553 	,	//	96
	551 	,	550 	,	548 	,	547 	,	545 	,	544 	,	543 	,	541 	,	540 	,	538 	,	//	97
	537 	,	535 	,	534 	,	533 	,	531 	,	530 	,	528 	,	527 	,	526 	,	524 	,	//	98
	523 	,	522 	,	520 	,	519 	,	517 	,	516 	,	515 	,	513 	,	512 	,	511 	,	//	99
	509 	,	508 	,	507 	,	505 	,	504 	,	503 	,	501 	,	500 	,	499 	,	497 	,	//	100

};


/* 函数声明 ------------------------------------------------------------------*/
void sampling_module_detect_flow(SAMPLING_MODULE* module); 
BOOL _get_memsic_flow(SAMPLING_MODULE* module,u8 channel, u16 *flow, u16 sampling_cnt);
BOOL _get_senirion_flow(SAMPLING_MODULE* module,u8 channel,u16 offset, u16 scale, u16 *flow, u16 sampling_cnt);

/* 函数功能 ------------------------------------------------------------------*/
/******************************************************
*内部函数定义
******************************************************/
void sampling_module_init(SAMPLING_MODULE * module)
{
	module->flow = 0;
	module->sensor_err = NO_ERROR;
	module->sensor_type = SENIRION;
	
	
	//fifoInit(&curr_fifo, (u8*)_curr_fifo_buffer, CURR_FIFO_BUFFER_LEN<<1);
}
//打开MEMSIC流量传感器电源
void sampling_module_open_flow_sensor_power(u8 flg)
{
	if(flg)
	{
		HAL_GPIO_WritePin(VFLOW_EN_GPIO_Port,VFLOW_EN_Pin,GPIO_PIN_SET);
	}
	else
	{
		HAL_GPIO_WritePin(VFLOW_EN_GPIO_Port,VFLOW_EN_Pin,GPIO_PIN_RESET);
	}
}

/********************************************************************************
*流量采样部件
*********************************************************************************/
//SENIRION
BOOL _get_senirion_flow(SAMPLING_MODULE* module,u8 channel,u16 offset, u16 scale, u16 *flow, u16 sampling_cnt)
{
	u8 cnt;
	u16 senirion_sampling_cnt = sampling_cnt;
	static u16 flow_err_cnt = 0;
	etError error = NO_ERROR;
	u8 errorcnt = 0;

	for(cnt = 0; cnt < senirion_sampling_cnt; cnt ++)
	{
		error = SF05_GetFlow(channel, offset, scale, flow, SENIRION);
		if(NO_ERROR == error)
		{
			flow_err_cnt = 0;
			ackerror = 0;
			if(channel == EEPROM_CH)
			module->sensor_err &= ~OPT_SENSOR_ERR;
			else if(channel == FLOW_CH)
				module->sensor_err &= ~OXY_SENSOR_ERR;
			else
				module->sensor_err &= ~AIR_SENSOR_ERR;
			
			return TRUE;
		}
		else
		{		
			errorcnt++;
			if(errorcnt >= 20)
				ackerror = 1;
			flow_err_cnt++;
			if(flow_err_cnt > 200)
			{
				flow_err_cnt = 1000;
				if(channel == EEPROM_CH)
					module->sensor_err |= OPT_SENSOR_ERR;
				else if(channel == FLOW_CH)
					module->sensor_err |= OXY_SENSOR_ERR;
				else
					module->sensor_err |= AIR_SENSOR_ERR;
				
				//错误
				return FALSE;
			}
		}
	}
	return FALSE;
	
#if 0	
	for(cnt = 0; cnt < senirion_sampling_cnt; cnt ++)
	{
		//得到opt采样值
		if(NO_ERROR == SF05_GetFlow(channel, offset, scale, flow, SENIRION) && flow_err_cnt < 100)
		{			
			//OK
			flow_err_cnt = 0;
			break;
		}
		
		//延时10ms
		OSTimeDlyHMSM(0,0,0,10);
	}

	if(cnt >= senirion_sampling_cnt)
	{
		flow_err_cnt++;
		if(flow_err_cnt > 15)
		{
			flow_err_cnt = 100;
			//错误
			if(channel == EEPROM_CH)
				module->sensor_err |= OPT_SENSOR_ERR;
			else if(channel == FLOW_CH)
				module->sensor_err |= OXY_SENSOR_ERR;
			else
				module->sensor_err |= AIR_SENSOR_ERR;
			
			//错误
			return FALSE;
		}
	}
	
	if(channel == EEPROM_CH)
		module->sensor_err &= ~OPT_SENSOR_ERR;
	else if(channel == FLOW_CH)
		module->sensor_err &= ~OXY_SENSOR_ERR;
	else
		module->sensor_err &= ~AIR_SENSOR_ERR;
	
	
	return TRUE;
	
#endif
}
//MEMSIC
BOOL _get_memsic_flow(SAMPLING_MODULE* module,u8 channel, u16 *flow, u16 sampling_cnt)
{
	u8 cnt;
	u16 memsic_sampling_cnt = sampling_cnt;
	static u16 flow_err_cnt = 0;
	etError error = NO_ERROR;
	u8 errorcnt = 0;
	#if 0
	//得到opt采样值
		if(NO_ERROR == SF05_GetFlow(channel, 0, 0, flow, MEMSIC))
		{
			flow_err_cnt = 0;
			if(channel == EEPROM_CH)
			module->sensor_err &= ~OPT_SENSOR_ERR;
			else if(channel == FLOW_CH)
				module->sensor_err &= ~OXY_SENSOR_ERR;
			else
				module->sensor_err &= ~AIR_SENSOR_ERR;
			
			return TRUE;
		}
		else
		{
			flow_err_cnt++;
			if(flow_err_cnt > 200)
			{
				flow_err_cnt = 1000;
				//错误
				if(channel == EEPROM_CH)
					module->sensor_err |= OPT_SENSOR_ERR;
				else if(channel == FLOW_CH)
					module->sensor_err |= OXY_SENSOR_ERR;
				else
					module->sensor_err |= AIR_SENSOR_ERR;

				return FALSE;
			}
		}
#endif

#if 1	
	for(cnt = 0; cnt < memsic_sampling_cnt; cnt ++)
	{
		error = SF05_GetFlow(channel, 0, 0, flow, MEMSIC);
		//得到opt采样值
		if(NO_ERROR == error)
		{			
			//OK
			ackerror = 0;
			break;
		}else
		{
			errorcnt++;
			if(errorcnt >= 20)
				ackerror = 1;
		}
		
		//读指令
		SF05_SetMeasurement(channel,MEMSIC);
		
		//延时10ms
		vTaskDelay(10/portTICK_RATE_MS ); 
	}
	
	if(cnt >= memsic_sampling_cnt)
	{
		//错误
		if(channel == EEPROM_CH)
			module->sensor_err |= OPT_SENSOR_ERR;
		else if(channel == FLOW_CH)
			module->sensor_err |= OXY_SENSOR_ERR;
		else
			module->sensor_err |= AIR_SENSOR_ERR;

		return FALSE;
	}
	
		//OK
	if(channel == EEPROM_CH)
		module->sensor_err &= ~OPT_SENSOR_ERR;
	else if(channel == FLOW_CH)
		module->sensor_err &= ~OXY_SENSOR_ERR;
	else
		module->sensor_err &= ~AIR_SENSOR_ERR;
	
	return TRUE;
	
#endif
}

void sampling_module_detect_flow(SAMPLING_MODULE* module)  
{
	u32 flow_sum = 0;
	static u16 flow = 0;
	u8 cnt;
	static u16 pre_flow = 0;
	static u16 flow_cnt = 0;
	static u16 flow_zero_cnt = 0;
	//发送测量命令
	if(module->sensor_type == MEMSIC)
		SF05_SetMeasurement(FLOW_CH,module->sensor_type);
	//延时
	vTaskDelay(20 /portTICK_RATE_MS);

	////得到流量
	if(module->sensor_type == MEMSIC)
		_get_memsic_flow(module,FLOW_CH, &flow,50);
	else
		_get_senirion_flow(module,FLOW_CH,OFFSET_FLOW, SCALE_FLOW_O2, &flow, 25);
/* 在设备工作时，关闭传感器电源并软件复位，解决传感器在过EFT时，传感器死机现象*/
		if(module->sensor_type == SENIRION)
		{
			if(pre_flow == flow)
			{
				flow_cnt++;
				if(flow_cnt > 100)
				{
					flow_cnt = 0;
					sampling_module_open_flow_sensor_power(FALSE);		//关闭5V电源
					IIC1_SCL_L;// = 0;
					IIC1_SDA_L;// = 0;
					vTaskDelay(100/portTICK_RATE_MS ); 
					sampling_module_open_flow_sensor_power(TRUE);			//打开5V电源
					IIC1_SCL_H;// = 1;
					IIC1_SDA_H;// = 1;
					vTaskDelay(10/portTICK_RATE_MS ); 
					SF05_SoftReset(FLOW_CH,SENIRION);		//软件复位
				}
			}
			pre_flow = flow;
		}
		if(flow == 0)
		{
			flow_zero_cnt++;
			if(flow_zero_cnt < 200)
			{
				return;
			}
		}
	flow_zero_cnt = 0;
/* end  */	
	
	//得到平均流量
	module->flow = flow;
}

//对应发热盘的温度传感器AD值对应温度值
s16 Plate_Neb_Temp(u16 AD_temp)		//加热盘与雾化散热片上的温度
{
	u16 i = 0;
	s16 jg = 0;
	if(AD_temp > NTC_Plate_and_Neb_TAB[0])
	{
		jg = 0;
		return (jg-250);
	}
	else if(AD_temp < NTC_Plate_and_Neb_TAB[1250])
	{
		jg = 1450;
		return (jg-250);
	}
	else
	{
		for(i=0;i<1451;i++)
		{
			if(AD_temp >= NTC_Plate_and_Neb_TAB[i])
			{
				jg = i;
				return (jg-250);
			}
		}
	}
}

void fill_up_sampling_module(SAMPLING_MODULE *module)
{
	uint16_t adc_5V=0;
	uint16_t adc_12V=0;
	uint16_t temp = 0;

	temp = ADC_Value[0];
	module->cur_adc=ADC_Value[1];  //比例阀电流
	adc_12V=ADC_Value[2];
	adc_5V=ADC_Value[3];

	module->IS_12V_ok = 33*adc_12V*(100+27)/4096/27;

	module->IS_5V_ok = 33*adc_5V*(100+100)/4096/100;

	module->temp = Plate_Neb_Temp(temp);
	#if 0
	if(adc_5V>=500&&adc_5V<=625)
	{
		module->IS_5V_ok=1;
	}
	else
	{
		module->IS_5V_ok=0;
	}
	
	if(adc_12V>=1700&&adc_12V<=2000)
	{
		module->IS_12V_ok=1;
	}
	else
	{
		module->IS_12V_ok=0;
	}
	#endif
}

////得到比例阀电流
//void sampling_module_get_cur_adc(SAMPLING_MODULE *module)
//{
//	
//    
////	HAL_ADC_Start(&hadc);
////	HAL_ADC_PollForConversion(&hadc,500);
////	module->cur_adc = HAL_ADC_GetValue(&hadc);
//	module->cur_adc=ADC_Value[0];
//	
//  
//}

//void sampling_module_is_5V_ok(SAMPLING_MODULE* module)
//{
//	static UINT32 adc_5v=0;
//	HAL_ADC_Start(&hadc_5V);
//	HAL_ADC_PollForConversion(&hadc_5V,500);
//	adc_5v = HAL_ADC_GetValue(&hadc_5V);
//}

//void sampling_module_is_12V_ok(SAMPLING_MODULE* module)
//{
//	static UINT32 adc_12v=0;
//	HAL_ADC_Start(&hadc_12V);
//	HAL_ADC_PollForConversion(&hadc_12V,500);
//	adc_12v = HAL_ADC_GetValue(&hadc_12V);
//}
void read_hardware()
{
	UINT8 temp = 0;
	temp = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8);
	hardware_versions = temp;
	temp = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15);
	hardware_versions = hardware_versions + temp*2;
	temp = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14);
	hardware_versions = hardware_versions + temp*4;
}
//采样任务
void sampling_task(void* pvParamemters)
{
	//任务初始化
	sampling_module_init(&sampling_module);

	read_hardware();

	uiID = sf_ReadID();
	if((uiID > 0x00000000)&&(uiID < 0x00ffffff))
	{
		out_flash = 1;
	}
	//interface_module_write_flash(0x08036000,&write_data,1);
	//interface_module_read_flash(0x08036000,&read_data,1);
//	HAL_ADC_Start_DMA(&hadc,(uint32_t *)&adc_data_buf,sizeof(adc_data_buf));
	
	while(1)
	{    	
//		sampling_module_get_cur_adc(&sampling_module);	//得到比例阀电流

		sampling_module_detect_flow(&sampling_module);	//得到流量

		fill_up_sampling_module(&sampling_module);
		
		#ifdef PROCESS_RCV_IN_TASK
		ProcessRcvedData();
		#endif
		led_delay++;
		if(led_delay == 20)
		{
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_SET);
		}else if(led_delay >= 40){
			HAL_GPIO_WritePin(GPIOA, GPIO_PIN_9, GPIO_PIN_RESET);
			led_delay = 0;
		}
		
		vTaskDelay(20/portTICK_RATE_MS ); 
	}		
}
