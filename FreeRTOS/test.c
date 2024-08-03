#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "xil_printf.h"
#include "sleep.h"
#include "semphr.h"
#include "xparameters.h"
#include "xbasic_types.h"
#include "xil_io.h""
#include <stdio.h>
#include <stdlib.h>

#include "DES.h"
#include "AES.h"
#include "GCD.h"

xSemaphoreHandle xMutex;
unsigned int currentTask = 1; // initial is 1, began from task1
/*
void vPeriodicTask(void *pvParameters)
{
    portTickType xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        xil_printf("!!! Periodic task running !!!\r\n");
        vTaskDelayUntil(&xLastWakeTime, (2000 / portTICK_RATE_MS));
    }
}
*/

void clear_input_buffer() {
	xil_printf("Start clear buffer\r\n");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void Task1(void *pvParameters)
{
    char *pcTaskName = (char *)pvParameters;
    for (;;)
    {
        if (currentTask == 1)
        {
        	xil_printf("%s\r\n", pcTaskName);
        	prvNewPrintString();
        	currentTask = 2;
        }
        else
        {
            vTaskDelay(2000 / portTICK_RATE_MS); // Idle state
        }
    }
}

void Task2(void *pvParameters)
{
    char *pcTaskName = (char *)pvParameters;
    for (;;)
    {
        if (currentTask == 2)
        {
        	xil_printf("%s\r\n", pcTaskName);
        	prvNewPrintString();
        	currentTask = 3;
        }
        else
        {
            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    }
}

void Task3(void *pvParameters)
{
    char *pcTaskName = (char *)pvParameters;
    for (;;)
    {
        if (currentTask == 3)
        {
        	xil_printf("%s\r\n", pcTaskName);
        	prvNewPrintString();
        	currentTask = 1;
        }
        else
        {
            vTaskDelay(2000 / portTICK_RATE_MS);
        }
    }
}

void prvNewPrintString(void)
{
    xSemaphoreTake(xMutex, portMAX_DELAY);
    {
    	if(currentTask == 1)
    	{
    		Xuint32 baseaddr = XPAR_AES_0_S00_AXI_BASEADDR;
    		Xuint32 AES_Encrypt_KEY_1 = 0x00010203;
    		Xuint32 AES_Encrypt_KEY_2 = 0x04050607;
    		Xuint32 AES_Encrypt_KEY_3 = 0x08090a0b;
    		Xuint32 AES_Encrypt_KEY_4 = 0x0c0d0e0f;

    		Xuint32 AES_Encrypt_DATA_1 = 0x00112233;
    		Xuint32 AES_Encrypt_DATA_2 = 0x44556677;
    		Xuint32 AES_Encrypt_DATA_3 = 0x8899aabb;
    		Xuint32 AES_Encrypt_DATA_4 = 0xccddeeff;

    		Xuint32 AES_Decrypt_KEY_1 = 0x13111D7F;
    		Xuint32 AES_Decrypt_KEY_2 = 0xE3944A17;
    		Xuint32 AES_Decrypt_KEY_3 = 0xF307A78B;
    		Xuint32 AES_Decrypt_KEY_4 = 0x4D2B30C5;

    		Xuint32 tmp_data1, tmp_data2, tmp_data3, tmp_data4;

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG12_OFFSET, 1);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG12_OFFSET, 0);

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG1_OFFSET, 1);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG3_OFFSET, AES_Encrypt_KEY_1);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG4_OFFSET, AES_Encrypt_KEY_2);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG5_OFFSET, AES_Encrypt_DATA_1);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG6_OFFSET, AES_Encrypt_DATA_2);

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG2_OFFSET, 1);

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG3_OFFSET, AES_Encrypt_KEY_3);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG4_OFFSET, AES_Encrypt_KEY_4);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG5_OFFSET, AES_Encrypt_DATA_3);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG6_OFFSET, AES_Encrypt_DATA_4);

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG2_OFFSET, 0);

    		xil_printf("Start encryption!\n\r");

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG0_OFFSET, 1);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG0_OFFSET, 0);

    		while(AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG11_OFFSET) == 0);

    		tmp_data1 = AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG7_OFFSET);
    		tmp_data2 = AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG8_OFFSET);
    		tmp_data3 = AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG9_OFFSET);
    		tmp_data4 = AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG10_OFFSET);

    		xil_printf("Encrypted data: %08x, %08x, %08x, %08x\n\r", tmp_data1, tmp_data2, tmp_data3, tmp_data4);

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG1_OFFSET, 0);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG3_OFFSET, AES_Decrypt_KEY_1);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG4_OFFSET, AES_Decrypt_KEY_2);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG5_OFFSET, tmp_data1);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG6_OFFSET, tmp_data2);

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG2_OFFSET, 1);

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG3_OFFSET, AES_Decrypt_KEY_3);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG4_OFFSET, AES_Decrypt_KEY_4);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG5_OFFSET, tmp_data3);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG6_OFFSET, tmp_data4);

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG2_OFFSET, 0);

    		xil_printf("Start decryption!\n\r");

    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG0_OFFSET, 1);
    		AES_mWriteReg(baseaddr, AES_S00_AXI_SLV_REG0_OFFSET, 0);

    		while(AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG11_OFFSET) == 0);

    		tmp_data1 = AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG7_OFFSET);
    		tmp_data2 = AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG8_OFFSET);
    		tmp_data3 = AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG9_OFFSET);
    		tmp_data4 = AES_mReadReg(baseaddr, AES_S00_AXI_SLV_REG10_OFFSET);

    		xil_printf("Decrypted data: %08x, %08x, %08x, %08x\n\r", tmp_data1, tmp_data2, tmp_data3, tmp_data4);
    	}
    	else if(currentTask == 2)
    	{
    		Xuint32 baseaddr = XPAR_DES_0_S00_AXI_BASEADDR;
    		Xuint32 DES_Encrypt_KEY_1 = 0x00010203;
    		Xuint32 DES_Encrypt_KEY_2 = 0x04050607;


    		Xuint32 DES_Encrypt_DATA_1 = 0x00112233;
    		Xuint32 DES_Encrypt_DATA_2 = 0x44556677;




    		        	//de/encrypt use the same key
    		Xuint32 DES_Decrypt_KEY_1 = 0x00010203;
    		Xuint32 DES_Decrypt_KEY_2 = 0x04050607;

    		Xuint32 tmp_data1, tmp_data2;

    		        	//reset
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG0_OFFSET, 1);
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG0_OFFSET, 0);

    		        	//decrypt  (select mode)
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG3_OFFSET, 1);

    		        	//e_data_rdy (load data)

    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG7_OFFSET, DES_Encrypt_KEY_1);
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG8_OFFSET, DES_Encrypt_KEY_2);
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG5_OFFSET, DES_Encrypt_DATA_1);
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG6_OFFSET, DES_Encrypt_DATA_2);





    		xil_printf("Start encryption!\n\r");

    		        	//(start) e_data_rdy
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG1_OFFSET, 1);
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG1_OFFSET, 0);


    		tmp_data1 = DES_mReadReg(baseaddr, DES_S00_AXI_SLV_REG9_OFFSET);
    		tmp_data2 = DES_mReadReg(baseaddr, DES_S00_AXI_SLV_REG10_OFFSET);

    		xil_printf("Encrypted data: %08x, %08x\n\r", tmp_data1, tmp_data2);

    		        	//decrypt
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG3_OFFSET, 0);

    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG7_OFFSET, DES_Decrypt_KEY_1);
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG8_OFFSET, DES_Decrypt_KEY_2);
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG5_OFFSET, tmp_data1);
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG6_OFFSET, tmp_data2);

    		xil_printf("Start decryption!\n\r");

    		        	//start e_data_rdy
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG1_OFFSET, 1);
    		DES_mWriteReg(baseaddr, DES_S00_AXI_SLV_REG1_OFFSET, 0);

    		tmp_data1 = DES_mReadReg(baseaddr, DES_S00_AXI_SLV_REG9_OFFSET);
    		tmp_data2 = DES_mReadReg(baseaddr, DES_S00_AXI_SLV_REG10_OFFSET);


    		xil_printf("Decrypted data: %08x, %08x\n\r", tmp_data1, tmp_data2);
    	}
    	else if(currentTask == 3)
    	{
    		Xuint32 baseaddr = XPAR_GCD_0_S00_AXI_BASEADDR; //設定gcd位址資訊
    		int  x, y;
    		while(1)
    		{
    			//set the input buffer mode
    			setvbuf(stdin, NULL, _IONBF, 0);//unbuffer :input will not stored in buffer,it will passed to program immediately

    			GCD_mWriteReg(baseaddr, GCD_S00_AXI_SLV_REG0_OFFSET, 2); //將slv_reg0(30)寫1做reset
    		    xil_printf("Input the value of x:");
    		    scanf("%d",&x);
    		    xil_printf("\n\rInput the value of y:");
    		    scanf("%d",&y);
    		    xil_printf("\n\r");
    		    if(x == 0 || y ==0)
    		    {
    		       xil_printf("Bye!\n\r");
    		       //fflush(stdin); clear the buffer
    		       break;
    		    }
    		    else
    		    {
    		       GCD_mWriteReg(baseaddr, GCD_S00_AXI_SLV_REG1_OFFSET, x); //將x值寫入slv_reg1，即x_i
    		       GCD_mWriteReg(baseaddr, GCD_S00_AXI_SLV_REG2_OFFSET, y); //將y值寫入slv_reg2，即y_i
    		       GCD_mWriteReg(baseaddr, GCD_S00_AXI_SLV_REG0_OFFSET, 1); //將slv_reg0(31)寫1，即go_i設定為1
    		       xil_printf("GCD(%d, %d) = %d\n\r", x, y, GCD_mReadReg(baseaddr, GCD_S00_AXI_SLV_REG3_OFFSET));
    		    }
    		}
    	}

    }
    xSemaphoreGive(xMutex);
}

int main(void)
{
    char *pcTextForTask1 = "********************AES*******************";
    char *pcTextForTask2 = "--------------------DES-------------------";
    char *pcTextForTask3 = "====================GCD===================";
    xMutex = xSemaphoreCreateMutex();
    if (xMutex != NULL)
    {
        xTaskCreate(Task1, (const char *)"AES", 1000, (void *)pcTextForTask1, 1, NULL);
        xTaskCreate(Task2, (const char *)"DES", 1000, (void *)pcTextForTask2, 2, NULL);
        xTaskCreate(Task3, (const char *)"GCD", 1000, (void *)pcTextForTask3, 3, NULL);
        //xTaskCreate(vPeriodicTask, (const char *)"Task 3", 1000, NULL, 4, NULL);
        vTaskStartScheduler();
    }
    for (;;);

    return 0;
}

















void vApplicationMallocFailedHook(void)
{
    /* do something */
    for (;;)
        ;
}

void vApplicationSetupHardware(void)
{
    /* Do nothing */
}
