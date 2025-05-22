#include "NRF24.h"


void CS_Select (void)
{
	GPIO_ResetBits(SPI1_GPIO, NRF24_CSN);
}

void CS_UnSelect (void)
{
	GPIO_SetBits(SPI1_GPIO, NRF24_CSN);
}

void CE_Enable (void)
{
	GPIO_SetBits(SPI1_GPIO, NRF24_CE);
}

void CE_Disable (void)
{
	GPIO_ResetBits(SPI1_GPIO, NRF24_CE);
}

void SPI_Send1Byte(uint8_t data){
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}
    SPI_I2S_SendData(SPI1, (uint16_t)data);
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET){}
}

uint8_t SPI_Receive1Byte(void)
{
    uint8_t temp;
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    temp = (uint8_t)SPI_I2S_ReceiveData(SPI1);

    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

    return temp;
}

// write a single byte to the particular register
void nrf24_WriteReg (uint8_t Reg, uint8_t Data)
{
	uint8_t buf[2];
	buf[0] = (Reg|(1<<5));
	buf[1] = Data;
	
	CS_Select();
	for(int i = 0; i < 2; i++){
		SPI_Send1Byte(buf[i]);
	}
	CS_UnSelect();
}

//write multiple bytes starting from a particular register
void nrf24_WriteRegMulti(uint8_t Reg, uint8_t *data, int size)
{
    uint8_t cmd = Reg | (1 << 5);  // Form the write command: 0x20 | register address

    CS_Select();  // Pull CS low to select the nRF24L01 device

    // Transmit the command byte
    SPI_Send1Byte(cmd);

    // Transmit multiple bytes of data to the register
    for (int i = 0; i < size; i++) {
        SPI_Send1Byte(data[i]);
    }

    CS_UnSelect();  // Pull CS high to release the device
}


uint8_t nrf24_ReadReg(uint8_t Reg)
{
    uint8_t data = 0;

    // Pull the CS Pin LOW to select the device
    CS_Select();

    // Send the register address to the NRF24 module
    SPI_Send1Byte(Reg);

    // Receive the data from the NRF24 module using the SPI_Receive1Byte function
    data = SPI_Receive1Byte(); // Use the function to receive the byte

    // Pull the CS HIGH to release the device
    CS_UnSelect();

    return data;
}

/* Read multiple bytes from the register */
void nrf24_ReadReg_Multi (uint8_t Reg, uint8_t *data, int size)
{
	// Pull the CS Pin LOW to select the device
	CS_Select();

	// Send the register address to the NRF24 module
  SPI_Send1Byte(Reg);
	
	for(int i = 0; i < size; i++)
	{
	data[i] = SPI_Receive1Byte();
	}

	// Pull the CS HIGH to release the device
	CS_UnSelect();
}

// send the command to the NRF
void nrfsendCmd (uint8_t cmd)
{
	// Pull the CS Pin LOW to select the device
	CS_Select();

	SPI_Send1Byte(cmd);

	// Pull the CS HIGH to release the device
	CS_UnSelect();
}

void nrf24_reset(uint8_t REG){
	if (REG == STATUS)
	{
		nrf24_WriteReg(STATUS, 0x00);
	}

	else if (REG == FIFO_STATUS)
	{
		nrf24_WriteReg(FIFO_STATUS, 0x11);
	}

	else {
	nrf24_WriteReg(CONFIG, 0x08);
	nrf24_WriteReg(EN_AA, 0x3F);
	nrf24_WriteReg(EN_RXADDR, 0x03);
	nrf24_WriteReg(SETUP_AW, 0x03);
	nrf24_WriteReg(SETUP_RETR, 0x03);
	nrf24_WriteReg(RF_CH, 0x02);
	nrf24_WriteReg(RF_SETUP, 0x0E);
	nrf24_WriteReg(STATUS, 0x00);
	nrf24_WriteReg(OBSERVE_TX, 0x00);
	nrf24_WriteReg(CD, 0x00);
	uint8_t rx_addr_p0_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	nrf24_WriteRegMulti(RX_ADDR_P0, rx_addr_p0_def, 5);
	uint8_t rx_addr_p1_def[5] = {0xC2, 0xC2, 0xC2, 0xC2, 0xC2};
	nrf24_WriteRegMulti(RX_ADDR_P1, rx_addr_p1_def, 5);
	nrf24_WriteReg(RX_ADDR_P2, 0xC3);
	nrf24_WriteReg(RX_ADDR_P3, 0xC4);
	nrf24_WriteReg(RX_ADDR_P4, 0xC5);
	nrf24_WriteReg(RX_ADDR_P5, 0xC6);
	uint8_t tx_addr_def[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};
	nrf24_WriteRegMulti(TX_ADDR, tx_addr_def, 5);
	nrf24_WriteReg(RX_PW_P0, 0);
	nrf24_WriteReg(RX_PW_P1, 0);
	nrf24_WriteReg(RX_PW_P2, 0);
	nrf24_WriteReg(RX_PW_P3, 0);
	nrf24_WriteReg(RX_PW_P4, 0);
	nrf24_WriteReg(RX_PW_P5, 0);
	nrf24_WriteReg(FIFO_STATUS, 0x11);
	nrf24_WriteReg(DYNPD, 0);
	nrf24_WriteReg(FEATURE, 0);
	}
}

void NRF24_Init (void)
{
	// disable the chip before configuring the device
	CE_Disable();
	
	// reset everything
  nrf24_reset (0);

	nrf24_WriteReg(CONFIG, 0);  // will be configured later

	nrf24_WriteReg(EN_AA, 0);  // No Auto ACK

	nrf24_WriteReg (EN_RXADDR, 0);  // Not Enabling any data pipe right now

	nrf24_WriteReg (SETUP_AW, 0x03);  // 5 Bytes for the TX/RX address

	nrf24_WriteReg (SETUP_RETR, 0);   // No retransmission

	nrf24_WriteReg (RF_CH, 0);  // will be setup during Tx or RX

	nrf24_WriteReg (RF_SETUP, 0x0E);   // Power= 0db, data rate = 2Mbps

	// Enable the chip after configuring the device
	CE_Enable();
}

// set up the Tx mode

void NRF24_TxMode (uint8_t *Address, uint8_t channel)
{
	// disable the chip before configuring the device
	CE_Disable();

	nrf24_WriteReg (RF_CH, channel);  // select the channel

	nrf24_WriteRegMulti(TX_ADDR, Address, 5);  // Write the TX address


	// power up the device
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config & (0xF2); 
  nrf24_WriteReg(CONFIG, config);


	// Enable the chip after configuring the device
	CE_Enable();
}

// transmit the data

/**
 * @brief  Transmit 32-byte payload via nRF24L01 using SPL.
 * @param  data: con tr? t?i m?ng 32 byte payload.
 * @retval 1 n?u TX FIFO empty sau khi g?i và dã flush; 0 n?u v?n còn d? li?u ch?.
 */
uint8_t NRF24_Transmit(uint8_t *data)
{
    uint8_t fifostatus;

    // 1) g?i payload
    CS_Select();
    SPI_Send1Byte(W_TX_PAYLOAD);
    for (int i = 0; i < 32; i++) {
        SPI_Send1Byte(data[i]);
    }
    CS_UnSelect();

    // 2) d?i 1 ms cho quá trình chuy?n
		CE_Enable();
    delay_ms(1);
		CE_Disable();

    // 3) d?c FIFO_STATUS
    fifostatus = nrf24_ReadReg(FIFO_STATUS);

    // bit4 = TX_EMPTY, bit3 = TX_FULL
    if ((fifostatus & (1 << 4)) && !(fifostatus & (1 << 3))) {
        // 4) flush TX FIFO
        nrfsendCmd(FLUSH_TX);
        // 5) reset các flag trên FIFO_STATUS
        nrf24_reset(FIFO_STATUS);
        return 1;
    }

    return 0;
}


void NRF24_RxMode (uint8_t *Address, uint8_t channel)
{
	// disable the chip before configuring the device
	CE_Disable();

	nrf24_reset (STATUS);

	nrf24_WriteReg (RF_CH, channel);  // select the channel

	// select data pipe 2
	uint8_t en_rxaddr = nrf24_ReadReg(EN_RXADDR);
	en_rxaddr = en_rxaddr | (1<<2);
	nrf24_WriteReg (EN_RXADDR, en_rxaddr);

	/* We must write the address for Data Pipe 1, if we want to use any pipe from 2 to 5
	 * The Address from DATA Pipe 2 to Data Pipe 5 differs only in the LSB
	 * Their 4 MSB Bytes will still be same as Data Pipe 1
	 *
	 * For Eg->
	 * Pipe 1 ADDR = 0xAABBCCDD11
	 * Pipe 2 ADDR = 0xAABBCCDD22
	 * Pipe 3 ADDR = 0xAABBCCDD33
	 *
	 */
	nrf24_WriteRegMulti(RX_ADDR_P1, Address, 5);  // Write the Pipe1 address
	nrf24_WriteReg(RX_ADDR_P2, 0xEE);  // Write the Pipe2 LSB address

	nrf24_WriteReg (RX_PW_P2, 32);   // 32 bit payload size for pipe 2


	// power up the device in Rx mode
	uint8_t config = nrf24_ReadReg(CONFIG);
	config = config | (1<<1) | (1<<0);
	nrf24_WriteReg (CONFIG, config);

	// Enable the chip after configuring the device
	CE_Enable();
}


uint8_t isDataAvailable (int pipenum)
{
	uint8_t status = nrf24_ReadReg(STATUS);

	if ((status&(1<<6))&&(status&(pipenum<<1)))
	{

		nrf24_WriteReg(STATUS, (1<<6));

		return 1;
	}

	return 0;
}


void NRF24_Receive (uint8_t *data)
{
	uint8_t cmdtosend = 0;

	// select the device
	CS_Select();

	// payload command
	cmdtosend = R_RX_PAYLOAD;
	SPI_Send1Byte(cmdtosend);

	// Receive the payload
	for(int i = 0; i < 32; i++)
	{
		data[i] = SPI_Receive1Byte();
	}

	// Unselect the device
	CS_UnSelect();

	delay_ms(1);

	cmdtosend = FLUSH_RX;
	nrfsendCmd(cmdtosend);
}



// Read all the Register data
void NRF24_ReadAll (uint8_t *data)
{
	for (int i=0; i<10; i++)
	{
		*(data+i) = nrf24_ReadReg(i);
	}

	nrf24_ReadReg_Multi(RX_ADDR_P0, (data+10), 5);

	nrf24_ReadReg_Multi(RX_ADDR_P1, (data+15), 5);

	*(data+20) = nrf24_ReadReg(RX_ADDR_P2);
	*(data+21) = nrf24_ReadReg(RX_ADDR_P3);
	*(data+22) = nrf24_ReadReg(RX_ADDR_P4);
	*(data+23) = nrf24_ReadReg(RX_ADDR_P5);

	nrf24_ReadReg_Multi(RX_ADDR_P0, (data+24), 5);

	for (int i=29; i<38; i++)
	{
		*(data+i) = nrf24_ReadReg(i-12);
	}

}


