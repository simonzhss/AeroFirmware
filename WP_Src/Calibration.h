/* Copyright (c)  2018-2025 Wuhan Nameless Innovation Technology Co.,Ltd. All rights reserved.*/
/*----------------------------------------------------------------------------------------------------------------------/
*               本程序只供购买者学习使用，版权著作权属于无名科创团队，无名科创团队将飞控程序源码提供给购买者，
*               购买者要为无名科创团队提供保护，未经作者许可，不得将源代码提供给他人，不得将源代码放到网上供他人免费下载， 
*               更不能以此销售牟利，如发现上述行为，无名科创团队将诉之以法律解决！！！
-----------------------------------------------------------------------------------------------------------------------/
*               生命不息、奋斗不止；前人栽树，后人乘凉！！！
*               开源不易，且学且珍惜，祝早日逆袭、进阶成功！！！
*               学习优秀者，简历可推荐到DJI、ZEROTECH、XAG、AEE、GDU、AUTEL、EWATT、HIGH GREAT等公司就业
*               求职简历请发送：15671678205@163.com，需备注求职意向单位、岗位、待遇等
*               无名科创开源飞控QQ群：540707961
*               CSDN博客：http://blog.csdn.net/u011992534
*               优酷ID：NamelessCotrun无名小哥
*               B站教学视频：https://space.bilibili.com/67803559/#/video
*               客户使用心得、改进意见征集贴：http://www.openedv.com/forum.php?mod=viewthread&tid=234214&extra=page=1
*               淘宝店铺：https://shop348646912.taobao.com/?spm=2013.1.1000126.2.5ce78a88ht1sO2
*               百度贴吧:无名科创开源飞控
*               公司官网:www.nameless.tech
*               修改日期:2019/4/12
*               版本：躺赢者——CarryPilot_V1.0
*               版权所有，盗版必究。
*               Copyright(C) 2017-2025 武汉无名创新科技有限公司 
*               All rights reserved
*               重要提示：
*               正常淘宝咸鱼转手的飞控、赠送朋友、传给学弟的都可以进售后群学习交流，
*               不得直接在网上销售无名创新资料，无名创新代码有声明版权，他人不得将
*               资料代码传网上供他人下载，不得以谋利为目的销售资料代码，发现有此操
*               作者，公司会提前告知，请1天内及时处理，否则你的学校、单位、姓名、电
*               话、地址信息会被贴出在公司官网、官方微信公众平台、官方技术博客、知乎
*               专栏以及淘宝店铺首页予以公示公告，此种所作所为，会成为个人污点，影响
*               升学、找工作、社会声誉、很快就很在无人机界出名，后果很严重。
*               因此行为给公司造成重大损失者，会以法律途径解决，感谢您的合作，谢谢！！！
----------------------------------------------------------------------------------------------------------------------*/
#ifndef _CALIBRATION_H
#define _CALIBRATION_H



#define AcceMax_1G      2048
#define GRAVITY_MSS     9.80665f
#define ACCEL_TO_1G     GRAVITY_MSS/AcceMax_1G
#define One_G_TO_Accel  AcceMax_1G/GRAVITY_MSS


typedef struct
{
 float x;
 float y;
 float z;
}Acce_Unit;

typedef struct
{
 float x;
 float y;
 float z;
}Mag_Unit;


typedef struct {
    int16_t x_max;
    int16_t y_max;
    int16_t z_max;
    int16_t x_min;
    int16_t y_min;
    int16_t z_min;
    float x_offset;
    float y_offset;
    float z_offset;
}Calibration;

void Calibrationer(void);
void Accel_Calibration_Check(void);
uint8_t Accel_Calibartion(void);
bool Parameter_Init(void);
void Mag_Calibration_Check(void);
uint8_t Mag_Calibartion(Vector3f *magdata,Vector3f_Body Circle_Angle_Calibartion);
void Reset_Mag_Calibartion(uint8_t Type);
void Reset_Accel_Calibartion(uint8_t Type);

void Mag_LS_Init(void);
uint8_t Mag_Calibartion_LS(Vector3f *magdata,Vector3f_Body Circle_Angle_Calibartion);

void ESC_Calibration(void);
void RC_Calibration_Trigger(void);
bool RC_Calibration_Check(uint16 *rc_date);
void ESC_HardWave_Init(void);//只初始化校准电调的必要资源
void ESC_Calibration_Check(void);
uint8_t Check_Calibration_Flag(void);
void Reset_RC_Calibartion(uint8_t Type);
void Horizontal_Calibration_Check(void);
void Horizontal_Calibration_Init(void);
void Headless_Mode_Calibration_Check(void);
uint8_t Gyro_Calibration_Check(vector3f *gyro);
extern Acce_Unit acce_sample[6];
extern uint8_t  Mag_Calibration_Mode;
extern uint8_t flight_direction;
extern Acce_Unit Accel_Offset_Read,Accel_Scale_Read;
extern Mag_Unit DataMag;
extern Mag_Unit Mag_Offset_Read;
extern Calibration Mag;
extern uint8_t Mag_360_Flag[3][36];
extern uint16_t Mag_Is_Okay_Flag[3];
extern float Yaw_Correct;
extern Vector2f MagN;
extern int16_t Mag_Offset[3];
extern float Mag_Data[3];
extern Vector_RC  RC_Calibration[8];
extern float Hor_Accel_Offset[3];
extern float mag_a,mag_b,mag_c,mag_r;
extern float ESC_Calibration_Flag;
extern float Pitch_Offset,Roll_Offset;
extern float Headless_Mode_Yaw;
extern uint8_t Gyro_Safety_Calibration_Flag;
#endif

