
#include "Headfile.h"
#include "RC.h"

uint16 Throttle_Control=0;
int16 Pitch_Control=0,Roll_Control=0,Yaw_Control=0;
int16 Target_Angle[2]={0};
uint16 Last_Throttle_Control,Pre_Last_Throttle_Control;
uint16 Throttle_Base=1000,TempThrottle_Control;
uint16_t Controler_State=0;
uint16_t Auto_ReLock_Cnt=0;//自动上锁计数器
uint8_t Auto_Relock_Flag=0;
uint8_t Auto_Relock_Flag_Set=0;
int16 Temp_RC=0;
uint16_t Unlock_Makesure_Cnt=0,Lock_Makesure_Cnt=0;
uint16_t Unwanted_Lock_Flag=0;
uint16_t Forced_Lock_Makesure_Cnt=0;
uint16 RC_NewData[4]={0};

uint16_t RC_Buttom=1000,RC_Top=2000,RC_Middle=1500,RC_Deadband=100;
uint16_t RC_Deadzone_Buttom=0,RC_Deadzone_Top=0;
Butter_BufferData RC_LPF_Buffer[4];
uint16_t PPM_LPF_Databuf[4];

uint8_t Last_Lock_State=UnLock;



int16_t constrain_int16_t(int16_t amt, int16_t low, int16_t high){
  return ((amt)<(low)?(low):((amt)>(high)?(high):(amt)));
}

/***********************************************************
@函数名：RC_Data_LPF_Filter
@入口参数：无
@出口参数：无
功能描述：遥控器原始数据滤波
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void RC_Data_LPF_Filter()
{
  uint16_t i=0;
  for(i=0;i<4;i++)//对遥控器前四个通道滤波处理
  {
    PPM_LPF_Databuf[i]=(uint16_t)(LPButterworth(PPM_Databuf[i],&RC_LPF_Buffer[i],&Butter_5HZ_Parameter_Acce));
  }
}


int16_t Throttle_Rate=0;
/***********************************************************
@函数名：Get_Thr_Rate
@入口参数：float Thr
@出口参数：无
功能描述：油门数据变化率
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
int16_t Get_Thr_Rate(float Thr)//油门变化量
{
  static float Thr_Rec[20];
  uint16_t i=0;
  for(i=19;i>0;i--)
  {
    Thr_Rec[i]=Thr_Rec[i-1];
  }
  Thr_Rec[0]=Thr;
  return (int16_t)((Thr_Rec[0]-Thr_Rec[9])/1.0f);
}

/***********************************************************
@函数名：RC_Scale_Set
@入口参数：Vector_RC *rc_date
@出口参数：无
功能描述：遥控器通道行程获取
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void RC_Scale_Set(Vector_RC *rc_date)
{
  RC_Buttom=(uint16_t)(rc_date->min);
  RC_Top=(uint16_t)(rc_date->max);
  RC_Middle=(uint16_t)(rc_date->middle);
  RC_Deadband=(uint16_t)(rc_date->deadband);
  RC_Deadzone_Buttom=RC_Middle-RC_Deadband/2;
  RC_Deadzone_Top=RC_Middle+RC_Deadband/2;
}

/***********************************************************
@函数名：Remote_Control
@入口参数：无
@出口参数：无
功能描述：遥控器数据转换
@作者：无名小哥
@日期：2019年01月27日
*************************************************************/
void Remote_Control(void )
{
	if(PPM_Safety_Flag==1)
	{
			RC_Data_LPF_Filter();
			RC_Scale_Set(&RC_Calibration[0]);
			if(PPM_Databuf[0]<=RC_Deadzone_Buttom)  Roll_Control=(RC_Deadzone_Buttom-PPM_LPF_Databuf[0])*Pit_Rol_Max/(RC_Deadzone_Buttom-RC_Buttom);//最大行程控制量+-45
			else if(PPM_Databuf[0]>=RC_Deadzone_Top)  Roll_Control=(RC_Deadzone_Top-PPM_LPF_Databuf[0])*Pit_Rol_Max/(RC_Top-RC_Deadzone_Top);
			else Roll_Control=0;
			Roll_Control=constrain_int16_t(Roll_Control,-Pit_Rol_Max,Pit_Rol_Max);
			
			RC_Scale_Set(&RC_Calibration[1]);
			if(PPM_Databuf[1]<=RC_Deadzone_Buttom)  Pitch_Control=(RC_Deadzone_Buttom-PPM_LPF_Databuf[1])*Pit_Rol_Max/(RC_Deadzone_Buttom-RC_Buttom);//最大行程控制量+-45
			else if(PPM_Databuf[1]>=RC_Deadzone_Top)  Pitch_Control=(RC_Deadzone_Top-PPM_LPF_Databuf[1])*Pit_Rol_Max/(RC_Top-RC_Deadzone_Top);
			else Pitch_Control=0;
			Pitch_Control=constrain_int16_t(Pitch_Control,-Pit_Rol_Max,Pit_Rol_Max);
			
			Target_Angle[0]=-Pitch_Control;//自稳时，期望俯仰角
			Target_Angle[1]=-Roll_Control;//自稳时，期望横滚角
			
			RC_Scale_Set(&RC_Calibration[2]);
			Temp_RC=(PPM_LPF_Databuf[2]-(RC_Buttom+Buttom_Safe_Deadband));//为了安全，油门杆低位死区为Buttom_Safe_Deadband
			Throttle_Control=constrain_int16_t(Temp_RC,0,1000);
			
			RC_Scale_Set(&RC_Calibration[3]);
			if(PPM_Databuf[3]<=RC_Deadzone_Buttom)  Yaw_Control=-(PPM_LPF_Databuf[3]-RC_Deadzone_Buttom)*Yaw_Max/(RC_Deadzone_Buttom-RC_Buttom);//偏航最大行程控制量+-150
			else if(PPM_Databuf[3]>=RC_Deadzone_Top)  Yaw_Control=-(PPM_LPF_Databuf[3]-RC_Deadzone_Top)*Yaw_Max/(RC_Top-RC_Deadzone_Top);
			else Yaw_Control=0;
			Yaw_Control=constrain_int16_t(Yaw_Control,-Yaw_Max,Yaw_Max);
			RC_NewData[0]=Throttle_Control;//遥感油门原始行程量
			Throttle_Rate=Get_Thr_Rate(Throttle_Control);
			Throttle_Control=Throttle_Base+Throttle_Control;
			
			
			/***************************************************************
			左手油门时，遥控器解锁动作：SwF向下
			左手油门时，遥控器上锁动作：SwF向上
			***************************************************************/
      			
			if(PPM_Databuf[5]<=(RC_Calibration[5].min+RC_Calibration[5].deadband)
//							 &&(Unlock_Makesure_Cnt>200*0.1//持续0.5S
//									||Forced_Lock_Makesure_Cnt>=200*1)
     // &&Forced_Lock_Makesure_Cnt>=200*1
                       &&Last_Lock_State==UnLock)//仅维持一段时间该动作，防止不能跳出过程
			{
				Controler_State=Lock_Controler;
				Unlock_Makesure_Cnt=0;
				Lock_Makesure_Cnt=0;
				Forced_Lock_Makesure_Cnt=0;
				Bling_Set(&Light_1,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
				Bling_Set(&Light_2,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
				Bling_Set(&Light_3,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
				Bling_Mode=0;
				Page_Number=0;//OLED恢复首页
				Reset_Mag_Calibartion(1);
				Reset_Accel_Calibartion(1);
				Reset_RC_Calibartion(1);
        Last_Lock_State=Lock;
			}
			
//			if(PPM_Databuf[5]>=(RC_Calibration[5].max-RC_Calibration[5].deadband)
//          &&Throttle_Control==1000
//             &&Roll_Control==0
//               &&Pitch_Control==0
//                  &&Last_Lock_State==UnLock)
//				Lock_Makesure_Cnt++;
			
			if(PPM_Databuf[5]>=(RC_Calibration[5].max-RC_Calibration[5].deadband)
          &&Throttle_Control==1000
					 &&Roll_Control==0
						 &&Pitch_Control==0
//              &&Lock_Makesure_Cnt>200*0.5//持续2.0S
                &&Last_Lock_State==Lock
                  &&Gyro_Safety_Calibration_Flag==1
                    &&Check_Calibration_Flag()==0x00)
			{
				Controler_State=Unlock_Controler;
				if(Controler_High_Mode==2)//如果是在定高模式下解锁 
				{  
					 Unwanted_Lock_Flag=1;   //不需要自动进入上锁模式
		#if    (YAW_Pos_Control_Accel_Disable==1)
					Total_Controller.High_Speed_Control.Integrate=-Total_Controller.High_Speed_Control.Integrate_Max;
		#else
					Total_Controller.High_Acce_Control.Integrate=-Total_Controller.High_Acce_Control.Integrate_Max;
		#endif
				}
				else Unwanted_Lock_Flag=0;
				Lock_Makesure_Cnt=0;
				Unlock_Makesure_Cnt=0;
				Forced_Lock_Makesure_Cnt=0;
				Bling_Set(&Light_1,5000,2000,0.9,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
				Bling_Set(&Light_2,5000,2000,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
				Bling_Set(&Light_3,5000,2000,0.1,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
				Bling_Mode=0;
				Page_Number=0;//OLED恢复首页
				Reset_Mag_Calibartion(1);
				Reset_Accel_Calibartion(1);
				Reset_RC_Calibartion(1);
				Auto_ReLock_Cnt=200*8;//持续6S
				Auto_Relock_Flag_Set=0;
        Last_Lock_State=UnLock;
			}
			
			
			
			if(Controler_State==Unlock_Controler
				 &&Auto_Relock_Flag_Set==0//自动上锁位未设置
					 &&Throttle_Control==1000
						 &&Pitch_Control==0
							 &&Roll_Control==0
								 &&Yaw_Control==0//解锁后遥感回中
									 &&Unwanted_Lock_Flag==0)
			{
				Auto_Relock_Flag=1;//解锁初始时开启自动上锁标志位
				Auto_Relock_Flag_Set=1;//单次解锁后，只置位一次
			}
			
			
			if(Auto_Relock_Flag==1)
			{
				if(Throttle_Control==1000
					 &&Pitch_Control==0
						 &&Roll_Control==0
							 &&Yaw_Control==0
								 &&Controler_State==Unlock_Controler//自动上锁计数器
									 &&Unwanted_Lock_Flag==0)
				{
					Auto_ReLock_Cnt--;
					if(Auto_ReLock_Cnt<=0)  Auto_ReLock_Cnt=0;
					if(Auto_ReLock_Cnt==0)
					{
						Controler_State=Lock_Controler;//再次上锁
						Bling_Set(&Light_1,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
						Bling_Set(&Light_2,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
						Bling_Set(&Light_3,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
						Bling_Mode=0;
					}
				}
				
				if((Throttle_Control!=1000
						||Pitch_Control!=0
							||Roll_Control!=0
								||Yaw_Control!=0)
					 &&Auto_ReLock_Cnt>0//自动上锁计数器
						 &&Unwanted_Lock_Flag==0)
				{
					Auto_Relock_Flag=0;//只要遥感有动作，本次解锁后，自动上锁就不开启
					Auto_ReLock_Cnt=0;
					Bling_Set(&Light_1,5000,2000,0.3,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
					Bling_Set(&Light_2,5000,2000,0.3,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
					Bling_Set(&Light_3,5000,2000,0.3,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
					Bling_Mode=0;
				}
			}
	}
}
			/***************************************************************
			左手油门时，遥控器解锁动作：左边摇杆处于低位并向内侧打杆
			左手油门时，遥控器上锁动作：左边摇杆处于低位并向外侧打杆

			if(Throttle_Control==1000
				 &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
					 &&Roll_Control==0
						 &&Pitch_Control==0)
			{
				Unlock_Makesure_Cnt++;
				if(Forced_Lock_Makesure_Cnt<10000) Forced_Lock_Makesure_Cnt++;
			}
			
			
			
			if(Throttle_Control==1000
				 &&Yaw_Control>=Yaw_Max*Scale_Pecent_Max
					 &&Roll_Control==0
						 &&Pitch_Control==0
							 &&(Unlock_Makesure_Cnt>200*2.0//持续2.0S
									||Forced_Lock_Makesure_Cnt>=200*5))
			{
				Controler_State=Lock_Controler;
				Unlock_Makesure_Cnt=0;
				Lock_Makesure_Cnt=0;
				Forced_Lock_Makesure_Cnt=0;
				Bling_Set(&Light_1,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_1,0);
				Bling_Set(&Light_2,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_2,0);
				Bling_Set(&Light_3,3000,200,0.5,0,GPIO_PORTF_BASE,GPIO_PIN_3,0);
				Bling_Mode=0;
				Page_Number=0;//OLED恢复首页
				Reset_Mag_Calibartion(1);
				Reset_Accel_Calibartion(1);
				Reset_RC_Calibartion(1);
			}
			
			if(Throttle_Control==1000
				 &&Yaw_Control<=-Yaw_Max*Scale_Pecent_Max
					 &&Roll_Control==0
						 &&Pitch_Control==0)
				Lock_Makesure_Cnt++;
			
			if(Throttle_Control==1000
				 &&Yaw_Control<=-Yaw_Max*Scale_Pecent_Max
					 &&Roll_Control==0
						 &&Pitch_Control==0
							 //&&Lock_Makesure_Cnt>200*2.0//持续2.0S
      						&&Lock_Makesure_Cnt>200*2.0//持续0.5S

								 &&Gyro_Safety_Calibration_Flag==1
									&&Check_Calibration_Flag()==0x00)
			***************************************************************/
