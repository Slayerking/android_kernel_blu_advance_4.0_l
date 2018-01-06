/********************************************************************************************
 *     LEGAL DISCLAIMER
 *
 *     (Header of MediaTek Software/Firmware Release or Documentation)
 *
 *     BY OPENING OR USING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 *     THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE") RECEIVED
 *     FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON AN "AS-IS" BASIS
 *     ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED,
 *     INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
 *     A PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY
 *     WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 *     INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK
 *     ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
 *     NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION
 *     OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
 *
 *     BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE LIABILITY WITH
 *     RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION,
TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE
 *     FEES OR SERVICE CHARGE PAID BY BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 *     THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE WITH THE LAWS
 *     OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF LAWS PRINCIPLES.
 ************************************************************************************************/

#if 0

#include "drv_features.h"
#if (defined(ISP_SUPPORT)&&defined(DRV_ISP_6238_SERIES))
#include "camera_para.h"
#include "med_api.h"

#define HI257_MAX_SCENE_MODE_NO 1/* code gen decided */

camcorder_info_struct	HI257_dsc_support_info={
	{1,1,1,1,1,1,0,0,0,1,1,1,1,1,1,1,1,1},						/* dscmode: 18 AE Mode */
	{1,1},												/* dsccomp: flash / af */
	{1,1,1,0},											/* flashlight: 4 flash mode */
	{{1,1,1,1,1,1,1},{1,1,1,1,1,1,1}}};
	/* af:af mode */
device_info_struct HI257_device_support_info={
/* ae_info: step num/step/minEV/maxEV/No.Iris/sensor_basegain/binning enable/binning offset */
{0,0,0,0,0,0,0,0},
/* flash_info: pol/lumIdx/duty/offset/shutter/minShut/maxShut/R/G/B/sensorgain/ispgain */
{0,0,0,0,0,0,0,0,0,0,0,0}};
//<====================== what listed are fixed codes
/*[Scene][video as 1]*/
static const device_info_struct *HI257_device_support_info_list[HI257_MAX_SCENE_MODE_NO] = {
NULL
};

static const exposure_lut_struct *HI257_AE_LUTs[HI257_MAX_SCENE_MODE_NO][2]={ /*[Scene][60Hz as 1]*/
NULL,    NULL
};

static const kal_uint8 *HI257_IRIS_LUTs[HI257_MAX_SCENE_MODE_NO]={ /*[Scene]*/
NULL
};

const exposure_lut_struct *HI257_get_ae_lut(ae_lut_info_struct info)
{
	register const exposure_lut_struct *pAeLut = NULL;
	register kal_uint32 mode = info.dscmode;
	register kal_uint32 idxHz = 0;/* default 50 Hz */
	/* check banding freq 60 Hz */
	if (CAM_BANDING_60HZ == info.band)
	{
		idxHz = 1;
	}
	/* check valid scene mode id */
	if (mode < HI257_MAX_SCENE_MODE_NO)
	{
		kal_mem_cpy(&HI257_device_support_info, HI257_device_support_info_list[mode], sizeof(device_info_struct));
		pAeLut = HI257_AE_LUTs[mode][idxHz];
	}
	ASSERT(pAeLut != NULL);
	return pAeLut;
}

const kal_uint8 *HI257_get_iris_lut(ae_lut_info_struct info)
{
  	const kal_uint8 *pIrisLut = NULL;
	register kal_uint32 mode = info.dscmode;
	/* check valid scene mode id */
	if (mode < HI257_MAX_SCENE_MODE_NO)
	{
	  	pIrisLut = HI257_IRIS_LUTs[mode];
	}
  	ASSERT(pIrisLut != NULL);
  	return pIrisLut;
}

#else
#if defined(ISP_SUPPORT)
#include "drv_comm.h"
#include "stdio.h"
#include "isp_if.h"
#include "image_sensor.h"
#include "sccb.h"
#include "ae_awb.h"
#include "camera_para.h"
#include "med_api.h"

#if (!(defined(__CAMERA_NVRAM_REDUCTION__) && (defined(DRV_ISP_6225_SERIES) || defined(DRV_ISP_6235_SERIES))))

ae_iso_sensor_gain_struct ae_iso_sensor_gain_info = {0, 0};

dsc_info_struct	dsc_support_info={
	{1,0,0,0,1,1,0,0,0},								/* dscmode: 9 AE Mode */
	{0,0},												/* dsccomp: flash / af */
	{0,0,0,0},											/* flashlight: 4 flash mode */
	{0,0,0,0}};											/* af: 4 af mode */

device_info_struct device_support_info={
/* ae_info: step num/step/minEV/maxEV/No.Iris */
	{121,100,4000,16000,1},
/* flash_info: pol/lumIdx/duty/offset/shutter/minShut/maxShut/R/G/B/sensorgain/ispgain */
	{0,6,560,1200,2672,2112,2840,143,128,129,256,64}};

kal_bool const APERTURE_PRI_TABLE[AV_NO]={0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
kal_bool const ISO_PRI_TABLE[ISO_NO]={1,1,1};
kal_uint16 const ISO_INFO_TABLE[ISO_NO]={64,64,64};

const exposure_lut_struct AE_AUTO_60HZ[121]={
{690 	,256 	,127 },
{690 	,256 	,118 },
{690 	,256 	,111 },
{690 	,256 	,103 },
{690 	,256 	,96  },
{690 	,256 	,90  },
{690 	,256 	,84  },
{690 	,256 	,78  },
{690 	,256 	,73  },
{690 	,256 	,68  },
{690 	,128 	,127 },
{690 	,128 	,119 },
{690 	,128 	,111 },
{690 	,128 	,103 },
{690 	,128 	,96  },
{690 	,128 	,90  },
{690 	,128 	,84  },
{690 	,128 	,78  },
{690 	,128 	,73  },
{690 	,128 	,68  },
{690 	,128 	,64  },
{621 	,128 	,66  },
{621 	,124 	,63  },
{552 	,128 	,64  },
{552 	,120 	,64  },
{483 	,128 	,64  },
{483 	,120 	,64  },
{414 	,128 	,65  },
{414 	,120 	,65  },
{345 	,128 	,68  },
{345 	,128 	,63  },
{345 	,120 	,63  },
{276 	,128 	,69  },
{276 	,128 	,64  },
{276 	,120 	,64  },
{276 	,112 	,64  },
{207 	,128 	,70  },
{207 	,128 	,65  },
{207 	,124 	,63  },
{207 	,112 	,65  },
{138 	,128 	,79  },
{138 	,128 	,74  },
{138 	,128 	,69  },
{138 	,128 	,64  },
{138 	,120 	,64  },
{138 	,112 	,64  },
{138 	,104 	,65  },
{138 	,96 	,65  },
{69 	,128 	,92  },
{69 	,128 	,86  },
{69 	,128 	,79  },
{69 	,128 	,74  },
{69 	,128 	,69  },
{69 	,128 	,64  },
{69 	,120 	,64  },
{69 	,112 	,64  },
{69 	,104 	,64  },
{69 	,100 	,63  },
{69 	,92 	,63  },
{69 	,84 	,65  },
{69 	,80 	,63  },
{69 	,76 	,62  },
{69 	,68 	,65  },
{69 	,64 	,64  },
{65 	,64 	,64  },
{60 	,64 	,64  },
{56 	,64 	,64  },
{53 	,64 	,64  },
{49 	,64 	,64  },
{46 	,64 	,64  },
{43 	,64 	,64  },
{40 	,64 	,64  },
{37 	,64 	,64  },
{35 	,64 	,63  },
{32 	,64 	,65  },
{30 	,64 	,64  },
{28 	,64 	,64  },
{26 	,64 	,65  },
{25 	,64 	,63  },
{23 	,64 	,64  },
{21 	,64 	,65  },
{20 	,64 	,64  },
{19 	,64 	,63  },
{17 	,64 	,65  },
{16 	,64 	,65  },
{15 	,64 	,64  },
{14 	,64 	,64  },
{13 	,64 	,65  },
{12 	,64 	,65  },
{11 	,68 	,63  },
{11 	,64 	,62  },
{10 	,64 	,64  },
{9 	,68 	,62  },
{9 	,64 	,62  },
{8 	,64 	,65  },
{8 	,64 	,60  },
{7 	,64 	,64  },
{7 	,64 	,60  },
{6 	,64 	,65  },
{6 	,64 	,61  },
{5 	,68 	,64  },
{5 	,64 	,64  },
{5 	,64 	,60  },
{4 	,68 	,65  },
{4 	,64 	,65  },
{4 	,64 	,60  },
{3 	,64 	,75  },
{3 	,72 	,62  },
{3 	,64 	,65  },
{3 	,64 	,61  },
{3 	,64 	,57  },
{2 	,80 	,64  },
{2 	,64 	,75  },
{2 	,68 	,65  },
{2 	,64 	,65  },
{2 	,64 	,60  },
{1 	,64 	,112 },
{1 	,64 	,106 },
{1 	,64 	,98  },
{1 	,92 	,64  },
{1 	,84 	,65  }};

const exposure_lut_struct AE_AUTO_60HZ_NIGHT[121]={
{1380 ,256 	,127 },
{1380 ,256 	,118 },
{1380 ,256 	,111 },
{1380 ,256 	,103 },
{1380 ,256 	,96  },
{1380 ,256 	,90  },
{1380 ,256 	,84  },
{1380 ,256 	,78  },
{1380 ,256 	,73  },
{1380 ,256 	,68  },
{1380 ,128 	,127 },
{1380 ,128 	,119 },
{1380 ,128 	,111 },
{1380 ,128 	,103 },
{1380 ,128 	,96  },
{1380 ,128 	,90  },
{1380 ,128 	,84  },
{1380 ,128 	,78  },
{1380 ,128 	,73  },
{1380 ,128 	,68  },
{1380 ,128 	,64  },
{1242 ,128 	,66  },
{1173 ,128 	,65  },
{1104 ,128 	,64  },
{1035 ,128 	,64  },
{966 	,128 	,64  },
{897 	,128 	,65  },
{828 	,128 	,65  },
{759 	,128 	,66  },
{759 	,124 	,64  },
{690 	,128 	,63  },
{621 	,128 	,66  },
{621 	,124 	,63  },
{552 	,128 	,64  },
{552 	,120 	,64  },
{483 	,128 	,64  },
{483 	,120 	,63  },
{414 	,128 	,65  },
{414 	,120 	,65  },
{345 	,128 	,68  },
{345 	,128 	,63  },
{345 	,120 	,63  },
{276 	,128 	,69  },
{276 	,128 	,64  },
{276 	,120 	,64  },
{276 	,112 	,64  },
{207 	,128 	,70  },
{207 	,128 	,65  },
{207 	,124 	,63  },
{207 	,112 	,65  },
{138 	,128 	,79  },
{138 	,128 	,74  },
{138 	,128 	,69  },
{138 	,128 	,64  },
{138 	,120 	,64  },
{138 	,112 	,64  },
{138 	,104 	,65  },
{138 	,96 	,65  },
{69 	,128 	,92  },
{69 	,128 	,86  },
{69 	,128 	,79  },
{69 	,128 	,74  },
{69 	,128 	,69  },
{69 	,128 	,64  },
{69 	,120 	,64  },
{69 	,112 	,64  },
{69 	,104 	,64  },
{69 	,100 	,63  },
{69 	,92 	,63  },
{69 	,84 	,65  },
{69 	,80 	,63  },
{69 	,76 	,62  },
{69 	,68 	,65  },
{69 	,64 	,64  },
{65 	,64 	,64  },
{60 	,64 	,64  },
{56 	,64 	,64  },
{53 	,64 	,64  },
{49 	,64 	,64  },
{46 	,64 	,64  },
{43 	,64 	,64  },
{40 	,64 	,64  },
{37 	,64 	,64  },
{35 	,64 	,63  },
{32 	,64 	,65  },
{30 	,64 	,64  },
{28 	,64 	,64  },
{26 	,64 	,65  },
{25 	,64 	,63  },
{23 	,64 	,64  },
{21 	,64 	,65  },
{20 	,64 	,64  },
{19 	,64 	,63  },
{17 	,64 	,65  },
{16 	,64 	,65  },
{15 	,64 	,64  },
{14 	,64 	,64  },
{13 	,64 	,65  },
{12 	,64 	,65  },
{11 	,68 	,63  },
{11 	,64 	,62  },
{10 	,64 	,64  },
{9 	,68 	,62  },
{9 	,64 	,62  },
{8 	,64 	,65  },
{8 	,64 	,60  },
{7 	,64 	,64  },
{7 	,64 	,60  },
{6 	,64 	,65  },
{6 	,64 	,61  },
{5 	,68 	,64  },
{5 	,64 	,64  },
{4 	,76 	,62  },
{4 	,68 	,65  },
{4 	,64 	,65  },
{4 	,64 	,60  },
{3 	,76 	,63  },
{3 	,72 	,62  },
{3 	,64 	,65  },
{3 	,64 	,61  },
{3 	,64 	,57  }};

const exposure_lut_struct AE_AUTO_50HZ[121]={
{664 	,256 	,123 },
{664 	,256 	,115 },
{664 	,256 	,107 },
{664 	,256 	,100 },
{664 	,256 	,93  },
{664 	,256 	,87  },
{664 	,256 	,81  },
{664 	,256 	,76  },
{664 	,256 	,71  },
{664 	,256 	,66  },
{664 	,128 	,123 },
{664 	,128 	,115 },
{664 	,128 	,107 },
{664 	,128 	,100 },
{664 	,128 	,93  },
{664 	,128 	,87  },
{664 	,128 	,81  },
{664 	,128 	,76  },
{664 	,128 	,71  },
{664 	,128 	,66  },
{664 	,124 	,63  },
{581 	,128 	,66  },
{581 	,124 	,63  },
{498 	,128 	,67  },
{498 	,124 	,64  },
{415 	,128 	,70  },
{415 	,128 	,65  },
{415 	,120 	,65  },
{332 	,128 	,71  },
{332 	,128 	,66  },
{332 	,124 	,63  },
{332 	,116 	,64  },
{249 	,128 	,71  },
{249 	,128 	,67  },
{249 	,124 	,64  },
{249 	,116 	,64  },
{249 	,108 	,64  },
{166 	,128 	,76  },
{166 	,128 	,71  },
{166 	,128 	,66  },
{166 	,124 	,63  },
{166 	,116 	,63  },
{166 	,108 	,63  },
{166 	,100 	,64  },
{83 	,128 	,93  },
{83 	,128 	,87  },
{83 	,128 	,81  },
{83 	,128 	,76  },
{83 	,128 	,71  },
{83 	,128 	,66  },
{83 	,124 	,63  },
{83 	,116 	,63  },
{83 	,108 	,64  },
{83 	,100 	,64  },
{83 	,92 	,65  },
{83 	,88 	,63  },
{83 	,80 	,65  },
{83 	,76 	,64  },
{83 	,72 	,63  },
{83 	,68 	,62  },
{80 	,64 	,64  },
{74 	,64 	,64  },
{69 	,64 	,64  },
{65 	,64 	,64  },
{60 	,64 	,64  },
{56 	,64 	,64  },
{53 	,64 	,64  },
{49 	,64 	,64  },
{46 	,64 	,64  },
{43 	,64 	,64  },
{40 	,64 	,64  },
{37 	,64 	,64  },
{35 	,64 	,63  },
{32 	,64 	,65  },
{30 	,64 	,64  },
{28 	,64 	,64  },
{26 	,64 	,65  },
{25 	,64 	,63  },
{23 	,64 	,64  },
{21 	,64 	,65  },
{20 	,64 	,64  },
{19 	,64 	,63  },
{17 	,64 	,65  },
{16 	,64 	,65  },
{15 	,64 	,64  },
{14 	,64 	,64  },
{13 	,64 	,65  },
{12 	,64 	,65  },
{11 	,68 	,63  },
{11 	,64 	,62  },
{10 	,64 	,64  },
{9 	,68 	,62  },
{9 	,64 	,62  },
{8 	,64 	,65  },
{8 	,64 	,60  },
{7 	,64 	,64  },
{7 	,64 	,60  },
{6 	,64 	,65  },
{6 	,64 	,61  },
{5 	,68 	,64  },
{5 	,64 	,64  },
{5 	,64 	,60  },
{4 	,68 	,65  },
{4 	,64 	,65  },
{4 	,64 	,60  },
{3 	,64 	,75  },
{3 	,72 	,62  },
{3 	,64 	,65  },
{3 	,64 	,61  },
{3 	,64 	,57  },
{2 	,80 	,64  },
{2 	,76 	,62  },
{2 	,68 	,65  },
{2 	,64 	,65  },
{2 	,64 	,60  },
{1 	,64 	,112 },
{1 	,64 	,106 },
{1 	,64 	,98  },
{1 	,92 	,64  },
{1 	,84 	,65  },
{1 	,80 	,64  }};

const exposure_lut_struct AE_AUTO_50HZ_NIGHT[121]={
{1411 ,256 	,124 },
{1411 ,256 	,116 },
{1411 ,256 	,108 },
{1411 ,256 	,101 },
{1411 ,256 	,94  },
{1411 ,256 	,88  },
{1411 ,256 	,82  },
{1411 ,256 	,76  },
{1411 ,256 	,71  },
{1411 ,256 	,67  },
{1411 ,128 	,124 },
{1411 ,128 	,116 },
{1411 ,128 	,108 },
{1411 ,128 	,101 },
{1411 ,128 	,94  },
{1411 ,128 	,88  },
{1411 ,128 	,82  },
{1411 ,128 	,77  },
{1411 ,128 	,71  },
{1411 ,128 	,67  },
{1328 ,128 	,66  },
{1245 ,128 	,66  },
{1162 ,128 	,66  },
{1079 ,128 	,66  },
{996 	,128 	,67  },
{996 	,124 	,64  },
{913 	,128 	,63  },
{830 	,128 	,65  },
{747 	,128 	,67  },
{747 	,124 	,64  },
{664 	,128 	,66  },
{664 	,124 	,64  },
{581 	,128 	,66  },
{581 	,124 	,63  },
{498 	,128 	,67  },
{498 	,124 	,64  },
{415 	,128 	,69  },
{415 	,128 	,65  },
{415 	,120 	,64  },
{332 	,128 	,71  },
{332 	,128 	,66  },
{332 	,124 	,63  },
{332 	,116 	,64  },
{249 	,128 	,71  },
{249 	,128 	,67  },
{249 	,124 	,64  },
{249 	,116 	,64  },
{249 	,108 	,64  },
{166 	,128 	,76  },
{166 	,128 	,71  },
{166 	,128 	,66  },
{166 	,124 	,63  },
{166 	,116 	,63  },
{166 	,108 	,63  },
{166 	,100 	,64  },
{83 	,128 	,93  },
{83 	,128 	,87  },
{83 	,128 	,81  },
{83 	,128 	,76  },
{83 	,128 	,71  },
{83 	,128 	,66  },
{83 	,124 	,63  },
{83 	,116 	,63  },
{83 	,108 	,64  },
{83 	,100 	,64  },
{83 	,92 	,65  },
{83 	,88 	,63  },
{83 	,80 	,65  },
{83 	,76 	,64  },
{83 	,72 	,63  },
{83 	,68 	,62  },
{80 	,64 	,64  },
{74 	,64 	,64  },
{69 	,64 	,64  },
{65 	,64 	,64  },
{60 	,64 	,64  },
{56 	,64 	,64  },
{53 	,64 	,64  },
{49 	,64 	,64  },
{46 	,64 	,64  },
{43 	,64 	,64  },
{40 	,64 	,64  },
{37 	,64 	,64  },
{35 	,64 	,63  },
{32 	,64 	,65  },
{30 	,64 	,64  },
{28 	,64 	,64  },
{26 	,64 	,65  },
{25 	,64 	,63  },
{23 	,64 	,64  },
{21 	,64 	,65  },
{20 	,64 	,64  },
{19 	,64 	,63  },
{17 	,64 	,65  },
{16 	,64 	,65  },
{15 	,64 	,64  },
{14 	,64 	,64  },
{13 	,64 	,65  },
{12 	,64 	,65  },
{11 	,68 	,63  },
{11 	,64 	,62  },
{10 	,64 	,64  },
{9 	,68 	,62  },
{9 	,64 	,62  },
{8 	,64 	,65  },
{8 	,64 	,60  },
{7 	,64 	,64  },
{7 	,64 	,60  },
{6 	,64 	,65  },
{6 	,64 	,61  },
{5 	,68 	,64  },
{5 	,64 	,64  },
{4 	,76 	,62  },
{4 	,68 	,65  },
{4 	,64 	,65  },
{4 	,64 	,60  },
{3 	,76 	,63  },
{3 	,72 	,62  },
{3 	,64 	,65  },
{3 	,64 	,61  },
{3 	,64 	,57  }};

const kal_uint8 IRIS_AUTO_LUT[121]={
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28,
28};

/* AF */
nvram_camera_lens_struct	camera_lens;
const nvram_camera_lens_struct CAMERA_LENS_DEFAULT_VALUE={
	{ /*af_table_num, af_home_idx, af_macro_idx, af_infinity_idx, ae_hyper_pos, af_me_home_pos, af_me_macro_pos, af_calibration_offset */
   	11, 0, 8, 3, 0, 0, 130, 0
   },
	{	{   0,  250}, {   6, 0x00}, {  12, 0x00}, {  18, 0x00},
		{  30, 0x00}, {  42, 0x00}, {  57, 0x00}, {  75, 0x00},
		{  93, 0x00}, { 111, 0x00}, { 130, 0x00}, {0x00, 0x00},
		{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},
		{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},
		{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},
		{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00},
		{0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}, {0x00, 0x00}
	}
};


const exposure_lut_struct *get_ae_lut(ae_lut_info_struct info)
{
	switch(info.dscmode)
	{
		case CAM_AUTO_DSC :
		case CAM_FLOWER :
			if(info.band==CAM_BANDING_60HZ)
			{
//				if(info.videomode==KAL_FALSE)
					return AE_AUTO_60HZ;
//				else
//					return AE_AUTO_60HZ_VIDEO;
			}
			else
			{
//				if(info.videomode==KAL_FALSE)
					return AE_AUTO_50HZ;
//				else
//					return AE_AUTO_50HZ_VIDEO;
			}
		break;
		case CAM_NIGHTSCENE :
			if(info.band==CAM_BANDING_60HZ)
			{
//				if(info.videomode==KAL_FALSE)
					return AE_AUTO_60HZ_NIGHT;
//				else
//					return AE_AUTO_60HZ_VIDEO_NIGHT;
			}
			else
			{
//				if(info.videomode==KAL_FALSE)
					return AE_AUTO_50HZ_NIGHT;
//				else
//					return AE_AUTO_50HZ_VIDEO_NIGHT;
			}
		break;
		case CAM_PORTRAIT :
		case CAM_LANDSCAPE :
		case CAM_SPORT :
		case CAM_TV_MODE :
		case CAM_AV_MODE :
		case CAM_ISO_MODE :
			//for test only
			ASSERT(0);
		break;
	}

    return 0;
}

const kal_uint8 *get_iris_lut(ae_lut_info_struct info)
{
	switch(info.dscmode)
	{
		case CAM_AUTO_DSC :
				return IRIS_AUTO_LUT;
		break;
	}
	return IRIS_AUTO_LUT;	/* default */
}

#endif
#endif
#endif

#endif

