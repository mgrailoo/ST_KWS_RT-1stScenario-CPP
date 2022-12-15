#include <string.h>
#include <stdio.h>
#include <math.h>
#include "main.h"

#include "usbd_cdc.h"
#include "usbd_cdc_interface.h"


#define 			AUDIO_CHANNELS 				        1
#define 			AUDIO_SAMPLING_FREQUENCY 		    16000
//#define 			AUDIO_IN_BUF_LEN   (AUDIO_CHANNELS*AUDIO_SAMPLING_FREQUENCY/1000)
#define 			AUDIO_IN_BUF_LEN   (AUDIO_CHANNELS*AUDIO_SAMPLING_FREQUENCY/1000*8    *10)
#define 			AUDIO_OUT_BUF_LEN  (AUDIO_IN_BUF_LEN*8)

volatile int16_t	audio_out_buffer[AUDIO_OUT_BUF_LEN];
extern void 		CDC_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
static void			*PCM1774_X_0_handle = NULL;
extern				USBD_DescriptorsTypeDef VCP_Desc;
extern 				TIM_HandleTypeDef TimHandle;

USBD_HandleTypeDef  USBD_Device;
TIM_HandleTypeDef 	TimCCHandle;
uint8_t 			BufferToWrite[256];
int32_t 			BytesToWrite;
uint16_t			PCM_Buffer[AUDIO_IN_BUF_LEN];



//#include "kws_ds_cnn.h"
#include "kws_SensorTile.h"

char output_class[12][8] = {"Silence", "Unknown","yes","no","up","down","left","right","on","off","stop","go"};
int recording_win = 2;
// Averaging window for smoothing out the output predictions
int averaging_window_len = 3;
int detection_threshold = 90;  //in percent
static KWS_SensorTile *kws= new KWS_SensorTile(recording_win,averaging_window_len);

#define timer_start()    	*((volatile uint32_t*)0xE0001000) = 0x40000001  	// Enable CYCCNT register
#define timer_stop()   *((volatile uint32_t*)0xE0001000) = 0x40000000  	// Disable CYCCNT register
#define timer_get()   *((volatile uint32_t*)0xE0001004)               // Get value from CYCCNT register

uint32_t timer_0;
uint32_t timer_1;


int main( void ) 
{

  HAL_Init();
  
  SystemClock_Config();


  BSP_AUDIO_IN_Init(AUDIO_SAMPLING_FREQUENCY, 16, AUDIO_CHANNELS);

  memset(kws->audio_buffer_in, 0, kws->audio_block_size);
  memset(kws->audio_buffer, 0, kws->audio_buffer_size);

  BSP_AUDIO_IN_SetVolume(90);
  BSP_AUDIO_IN_Record(PCM_Buffer,0);
  
 // STLBLE_PRINTF("\t 07 Welcome to the second version of RT-KWS in ALOHA Project!!!\n");

  while (1)
  {
    __WFI();
  }
}


/**
* @brief  User function that is called when 80 ms of PDM data is available.
* 		  In this application only PDM to PCM conversion and USB streaming
*                  is performed.
* 		  User can add his own code here to perform some DSP or audio analysis.
* @param  none
* @retval None
*/
void AudioProcess(void)
{
	//int t1,t2;
	//	t1=(int)HAL_GetTick();
	  timer_start();
	  timer_0 = timer_get();
	 static uint32_t IndexOut = 0;
	  static uint32_t AudioOutActive = 0;
	  static uint32_t indexIn = 0;
	  static uint32_t chk = 0;
	  static int prevind=-10;

	   uint32_t i;
	   for(i=0;i<AUDIO_IN_BUF_LEN;i++)
	    {
	      kws->audio_buffer_in[indexIn++] =PCM_Buffer[i];
	    }
	   if(indexIn==kws->audio_block_size){


	        	indexIn=0;
	        	int j=0;
	        	for (int i=(kws->audio_buffer_size-(kws->frame_len-kws->frame_shift));i<(kws->audio_buffer_size);i++) {
	        		    	     		    		    kws->audio_buffer[j] = kws->audio_buffer[i];
	        		    	     		    		   j++;
	        		    	     		    		  }
                // copy the new recording data
	        	for (int i=0;i<kws->audio_block_size;i++) {
	        		    		    kws->audio_buffer[kws->frame_len-kws->frame_shift+i] = kws->audio_buffer_in[i];
	        		    		                            }
	    		 kws->extract_features();    //extract mfcc features
	        	 kws->classify();	      //classify using dnn
	        	 kws->average_predictions();
	        	 int max_ind = kws->get_top_class(kws->averaged_output);
	        	 if(kws->averaged_output[max_ind]>detection_threshold*128/100) {
	        	            }

	             }
   	timer_1 = timer_get();
   	 timer_stop();
}

/**
* @brief  Transfer Complete user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
  AudioProcess();
}

/**
* @brief  Half Transfer Complete user callback, called by BSP functions.
* @param  None
* @retval None
*/
void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
  AudioProcess();
}



void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == (&TimHandle))
	{
		CDC_TIM_PeriodElapsedCallback(htim);
	}
}



/**
 * @brief  This function is executed in case of error occurrence
 * @param  None
 * @retval None
 */
void Error_Handler( void )
{

  while (1)
  {}
}



#ifdef  USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *   where the assert_param error has occurred
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed( uint8_t *file, uint32_t line )
{

  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  
  while (1)
  {}
}

#endif

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
