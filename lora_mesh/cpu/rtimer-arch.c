/*
 * Copyright (c) 2012, STMicroelectronics.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */
/*---------------------------------------------------------------------------*/
#include "rtimer-arch.h"
/*---------------------------------------------------------------------------*/
#define MCU_WAKE_UP_TIME_TICKS		(MCU_WAKE_UP_TIME / (1000000/RTIMER_SECOND))
/*---------------------------------------------------------------------------*/
static const uint8_t SecondsInMinute = 60;
static const uint16_t SecondsInHour = 3600;
static const uint32_t SecondsInDay = 86400;
static const uint8_t HoursInDay = 24;
static const uint16_t DaysInYear = 365;
static const uint16_t DaysInLeapYear = 366;
static const double DaysInCentury = 36524.219;
static const uint8_t DaysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const uint8_t DaysInMonthLeapYear[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static uint8_t PreviousYear = 0;
static uint8_t Century = 0;
/*---------------------------------------------------------------------------*/
void RTC_Alarm_IRQHandler(void)
{
  ENERGEST_ON(ENERGEST_TYPE_IRQ);

  /* Check on the AlarmA flag */
  if(RTC_GetITStatus(RTC_IT_ALRA) != RESET) 
  {
    lpm_exit_stopmode();
    rtimer_run_next();

    /* Clear RTC AlarmA Flags */
    RTC_ClearITPendingBit(RTC_IT_ALRA);
  }

  /* Clear the EXTI line 17 */
  EXTI_ClearITPendingBit(EXTI_Line17);

  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
/*---------------------------------------------------------------------------*/
void rtimer_arch_init(void)
{
  /* Initialize the RTC clock */
  init_rtc();

  /* Initialize the RTC Alarm interrupt */
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* EXTI configuration */
  EXTI_ClearITPendingBit(EXTI_Line17);
  EXTI_InitStructure.EXTI_Line = EXTI_Line17;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  
  /* Enable the RTC Alarm Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Disable AlarmA interrupt */
  RTC_ITConfig(RTC_IT_ALRA, ENABLE);
  
  /* Disable the AlarmA */
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
}
/*---------------------------------------------------------------------------*/
rtimer_clock_t rtimer_arch_now(void)
{
  rtimer_clock_t calendarValue = 0;
  uint8_t i = 0;

  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateStruct;
  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
  RTC_WaitForSynchro();

  if((PreviousYear == 99) && (RTC_DateStruct.RTC_Year == 0)){
    Century++;
  }
  PreviousYear = RTC_DateStruct.RTC_Year;

  /* Centuries */
  for(i = 0; i < Century; i++){
    calendarValue += (rtimer_clock_t)(DaysInCentury * SecondsInDay);
  }

  /* Years */
  for(i = 0; i < RTC_DateStruct.RTC_Year; i++){
    if((i == 0) || (i % 4 == 0)){
      calendarValue += DaysInLeapYear * SecondsInDay;
    }else{
      calendarValue += DaysInYear * SecondsInDay;
    }
  }

  /* Months */
  if((RTC_DateStruct.RTC_Year == 0) || (RTC_DateStruct.RTC_Year % 4 == 0)){
    for(i = 0; i < (RTC_DateStruct.RTC_Month - 1); i++){
      calendarValue += DaysInMonthLeapYear[i] * SecondsInDay;
    }
  }else{
    for(i = 0;  i < (RTC_DateStruct.RTC_Month - 1); i++){
      calendarValue += DaysInMonth[i] * SecondsInDay;
    }
  }     

  /* Days */
  calendarValue += ((uint32_t)RTC_TimeStruct.RTC_Seconds + 
            ((uint32_t)RTC_TimeStruct.RTC_Minutes * SecondsInMinute) +
            ((uint32_t)RTC_TimeStruct.RTC_Hours * SecondsInHour) + 
            ((uint32_t)(RTC_DateStruct.RTC_Date * SecondsInDay)));

  return calendarValue;
}
/*---------------------------------------------------------------------------*/
void rtimer_arch_schedule(rtimer_clock_t wakeup_time)
{
  uint16_t rtcSeconds = 0;
  uint16_t rtcMinutes = 0;
  uint16_t rtcHours = 0;
  uint16_t rtcDays = 0;

  uint8_t rtcAlarmSeconds = 0;
  uint8_t rtcAlarmMinutes = 0;
  uint8_t rtcAlarmHours = 0;
  uint16_t rtcAlarmDays = 0;

  RTC_AlarmTypeDef RTC_AlarmStructure;
  RTC_TimeTypeDef RTC_TimeStruct;
  RTC_DateTypeDef RTC_DateStruct;

  /* Clear Previous Alarm */
  RTC_ClearFlag(RTC_FLAG_ALRAF);
  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);


  RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
  RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);

  wakeup_time = wakeup_time - floor(MCU_WAKE_UP_TIME_TICKS + 0.5);		// Round ticks

  rtcSeconds = (wakeup_time % SecondsInMinute) + RTC_TimeStruct.RTC_Seconds;
  rtcMinutes = ((wakeup_time/SecondsInMinute) % SecondsInMinute) + RTC_TimeStruct.RTC_Minutes;
  rtcHours = ((wakeup_time/SecondsInHour) % HoursInDay) + RTC_TimeStruct.RTC_Hours;
  rtcDays = (wakeup_time/SecondsInDay) + RTC_DateStruct.RTC_Date;

  rtcAlarmSeconds = (rtcSeconds) % SecondsInMinute;
  rtcAlarmMinutes = ((rtcSeconds/SecondsInMinute) + rtcMinutes) % SecondsInMinute;
  rtcAlarmHours   = ((((rtcSeconds/SecondsInMinute) + rtcMinutes) / SecondsInMinute) + rtcHours) % HoursInDay;
  rtcAlarmDays  = (((((rtcSeconds/SecondsInMinute) + rtcMinutes) / SecondsInMinute) + rtcHours) / HoursInDay) + rtcDays;

  if((RTC_DateStruct.RTC_Year == 0) || (RTC_DateStruct.RTC_Year % 4 == 0))
  {
    if(rtcAlarmDays > DaysInMonthLeapYear[RTC_DateStruct.RTC_Month-1])    
    {   
      rtcAlarmDays = rtcAlarmDays % DaysInMonthLeapYear[RTC_DateStruct.RTC_Month-1];
    }
  }
  else
  {
    if(rtcAlarmDays > DaysInMonth[RTC_DateStruct.RTC_Month-1])    
    {   
      rtcAlarmDays = rtcAlarmDays % DaysInMonth[RTC_DateStruct.RTC_Month-1];
    }
  }

  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = rtcAlarmSeconds;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = rtcAlarmMinutes;
  RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = rtcAlarmHours;
  RTC_AlarmStructure.RTC_AlarmDateWeekDay      = (uint8_t)rtcAlarmDays;
  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel   = RTC_AlarmDateWeekDaySel_Date;
  RTC_AlarmStructure.RTC_AlarmMask             = RTC_AlarmMask_None;

  /* Enable the AlarmA */
  RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);
  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}
/*---------------------------------------------------------------------------*/
