/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdarg.h>


#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "wizchip_conf.h"
#include "socket.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

wiz_NetInfo net_info = {
		.mac  = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED},
		.ip   = {192, 168, 2, 99},
		.sn   = {255, 255, 255, 0},
		.gw   = {192, 168, 2, 1},
		.dns  = {8, 8, 8, 8},	// accessible globally by Google
		.dhcp = NETINFO_STATIC  // Use static IP
		//    .dhcp = NETINFO_DHCP
};

#define TCP_PORT 5000


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


// Buffer for received data
#define RX_BUFFER_SIZE 256 // Buffer size for receiving data
typedef struct uartStream
{
	uint8_t rxBuffer[RX_BUFFER_SIZE];
	uint8_t txBuffer[RX_BUFFER_SIZE];
	uint8_t rxByte;
	uint8_t rxFillIndex;
	uint8_t txFillIndex;
	uint8_t rxReadIndex;
	uint8_t txReadIndex;
	uint32_t lastByteTimestamp;
} uartStream;

uartStream uart2Stream;


void myPrintf(/*uint8_t debugLevel,*/ const char *format, ...)
{
	//	if (debugLevel <= DEBUG_LEVEL)
	{
		char buffer[128];
		va_list args;
		va_start(args, format);
		vsnprintf(buffer, sizeof(buffer), format, args);
		va_end(args);

		// Ensure the buffer does not overflow
		size_t length = strlen(buffer);
		size_t index = 0;
		while (length)
		{
			uart2Stream.txBuffer[uart2Stream.txFillIndex++] = buffer[index++];
			if (uart2Stream.txFillIndex >= RX_BUFFER_SIZE)
			{
				uart2Stream.txFillIndex = 0;  // Reset index on overflow
			}
			length--;
		}
	}

	while (uart2Stream.txFillIndex != uart2Stream.txReadIndex)
	{
		// Calculate the number of bytes to send

		// Send all available data from txBuffer
		HAL_UART_Transmit(&huart2, &uart2Stream.txBuffer[uart2Stream.txReadIndex], 1, HAL_MAX_DELAY);
		uart2Stream.txReadIndex++;

		// Prevent buffer overflow
		if (uart2Stream.txReadIndex >= RX_BUFFER_SIZE)
		{
			uart2Stream.txReadIndex = 0;  // Reset index on overflow
		}
	}
}


void wizchipSelect(void) {
	//	myPrintf("wizchipSelect\n");
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}

void wizchipUnselect(void) {
	//	myPrintf("wizchipUnselect\n");
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

void wizchipReadBurst(uint8_t* buff, uint16_t len) {
	//	myPrintf("wizchipReadBurst\n");
	HAL_SPI_Receive(&hspi1, buff, len, HAL_MAX_DELAY);
}

void wizchipWriteBurst(uint8_t* buff, uint16_t len) {
	//	myPrintf("wizchipWriteBurst\n");
	HAL_SPI_Transmit(&hspi1, buff, len, HAL_MAX_DELAY);
}

uint8_t wizchipReadByte(void) {
	//	myPrintf("wizchipReadByte\n");
	uint8_t byte;
	wizchipReadBurst(&byte, sizeof(byte));
	return byte;
}

void wizchipWriteByte(uint8_t byte) {
	//	myPrintf("wizchipWriteByte\n");
	wizchipWriteBurst(&byte, sizeof(byte));
}

void wizChipInit()
{
	reg_wizchip_cs_cbfunc(wizchipSelect, wizchipUnselect);
	reg_wizchip_spi_cbfunc(wizchipReadByte, wizchipWriteByte);
	reg_wizchip_spiburst_cbfunc(wizchipReadBurst, wizchipWriteBurst);

	myPrintf("wiznet functions registered\n");

	/*The W5500 has its own internal 32KB RAM for network data buffering
	 *(16 KB for RX, 16 KB for TX, or You can split each 16 KB block 
	 * among the 8 sockets as needed).
	 */
	uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};  // 2KB assigned for Rx & TX for each
	wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);
}


uint8_t checkLinkStatus(void)
{
	uint8_t phycfgr = getPHYCFGR();
	if (phycfgr & PHYCFGR_LNK_ON) {
		return 1; // Link is up
	} else {
		return 0; // Link is down
	}
}



/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  wizChipInit();

  wizchip_setnetinfo(&net_info);

	while (checkLinkStatus() == 0)
	{
		myPrintf("Waiting for link to be established...\n");
		HAL_Delay(1000); // Wait for 1 second before checking again
	}

	myPrintf("Link established, initializing W5500...\n");

	/**
	 * Creates a new socket for TCP communication.
	 * @param 0 : The socket number or channel. Typically, this is an integer value (0-7) 
	 * 	representing the hardware socket/channel to use.
	 * @param Sn_MR_TCP : The protocol mode for the socket. This constant specifies the socket will operate in TCP mode.
	 * @param 5000 :  The port number to bind the socket. For TCP, gave random port no 5000.
	 * @param SF_TCP_NODELAY :  Socket flag options. This controls socket behavior.
	 * @return :    Returns the socket descriptor (integer >= 0) on success, or a negative value on error.
	 */

	int tcp_sock = socket(0, Sn_MR_TCP, TCP_PORT, SF_TCP_NODELAY);

	if (tcp_sock != 0)
	{
		myPrintf("Failed to create TCP socket\r\n");
	}
	else
	{
		int8_t ret = listen(tcp_sock);
		if (ret != SOCK_OK)
		{
			myPrintf("Failed to listen on TCP socket: %d\r\n", ret);
			close(tcp_sock);
		}
		else
		{
			myPrintf("TCP server listening on port %d\r\n",TCP_PORT);
		}
	}


	uint8_t connectionStatus = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{

		if (checkLinkStatus() == 0)  // checking if link disconnected
		{
			if (connectionStatus != 0)  // connectionStatus is flag to keep track connection
			{
				myPrintf("Ethernet link lost! Closing TCP connection.\r\n");
				connectionStatus = 0;
				disconnect(0); // socket number 0 is disconnected
			}
			while (checkLinkStatus() == 0)
			{
				myPrintf("Waiting for link to be established...\n");
				HAL_Delay(1000);
			}

			myPrintf("Ethernet link re-established.\n");

			// Re-create the socket after link is up/established
			int tcp_sock = socket(0, Sn_MR_TCP, TCP_PORT, SF_TCP_NODELAY);

			if (tcp_sock == 0)
			{
				int8_t ret = listen(tcp_sock);
				if (ret == SOCK_OK)
				{
					myPrintf("TCP server re-listening on port %d\r\n", TCP_PORT);
				}
			}
			continue; // Skip rest of loop until link is up
		}


		// This is part is for TCP Client connection
		// Check TCP socket connection status
		uint8_t sock_status = getSn_SR(0); //non blocking call
		if (sock_status == SOCK_ESTABLISHED)
		{
			if(connectionStatus == 0)
			{
				connectionStatus = 1; // Set connection status to connected
				myPrintf("TCP client connected\r\n");
				const char msgOnConnection[] = "Welcome to DevHeads\n";
				int32_t sent_len = send(0, msgOnConnection, strlen(msgOnConnection));
				if (sent_len != strlen(msgOnConnection)) {
					myPrintf("Failed to send data back to client\r\n");
				}
			}

			if(connectionStatus == 1)
			{
				uint8_t tcp_rx_buf[64]={0};
				uint8_t tcp_tx_buf[64]={0};
				int32_t recv_len = recv(0, tcp_rx_buf, sizeof(tcp_rx_buf)); //blocking call
				if (recv_len > 0)
				{
					myPrintf("Received %ld bytes from TCP client\r\n", recv_len);
					myPrintf("data received =%s\n", tcp_rx_buf);
					tcp_rx_buf[recv_len] = '\0';

					if(strcmp((char*)tcp_rx_buf,"ON") == 0)
					{
						HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET); // Turn on LED
						// Echo back the received data (for testing)
						strcpy((char *)tcp_tx_buf, "\nLED IS ON");
					}
					else if(strcmp((char*)tcp_rx_buf, "OFF") == 0)
					{
						HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET); // Turn off LED
						// Echo back the received data (for testing)
						strcpy((char *)tcp_tx_buf, "\nLED IS OFF");
					}

					int32_t sent_len = send(0, tcp_tx_buf, strlen(tcp_tx_buf));
					if (sent_len != strlen(tcp_tx_buf)) {
						myPrintf("Failed to send data back to client\r\n");
					}
				}
			}
		}
		else if (sock_status == SOCK_CLOSE_WAIT)
		{
			connectionStatus = 0;
			myPrintf("TCP client disconnected\r\n");
			disconnect(0);

			while (checkLinkStatus() == 0)
			{
				myPrintf("Waiting for link to be established...\n");
				HAL_Delay(1000); // Wait for 1 second before checking again
			}

		}
		else if (sock_status == SOCK_CLOSED)
		{
			// Re-listen if socket closed
			connectionStatus = 0;
			int tcp_sock = socket(0, Sn_MR_TCP, TCP_PORT, SF_TCP_NODELAY);
			if (tcp_sock == 0)
			{
				int8_t ret = listen(tcp_sock);
				if (ret == SOCK_OK)
				{
					myPrintf("TCP server re-listening on port 5000\r\n");
				}
			}
		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
