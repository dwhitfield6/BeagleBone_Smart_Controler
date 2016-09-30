/******************************************************************************/
/* Change log                                                                 *
 *
 *
 *
 * Date         Revision    Comments
 * MM/DD/YY
 * --------     ---------   ----------------------------------------------------
 * 04/14/16     16.0_DW0a   New project creation.
 *                                                                            */
/******************************************************************************/

/******************************************************************************/
/* Contains functions to configure and communicate with the EMMC device
 *  (MTFC4GACAAAM-1M WT).
 *                                                                            */
/******************************************************************************/
#ifndef _EMMC_H_
#define _EMMC_H_

/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/******************************************************************************/
/* Pin Definitions			                                                  */
/******************************************************************************/
#define EMMC_RST_MODULE	1
#define EMMC_RST_REGS	SOC_GPIO_1_REGS
#define EMMC_RST_PIN	20

/******************************************************************************/
/* EMMC module base address	                                                  */
/******************************************************************************/
#define EMMC_BASE_ADDRESS SOC_MMCHS_1_REGS

/******************************************************************************/
/* EMMC_BUFFER_SIZE
 *
 * This is the size of the EMMC card buffer in DDR.
 *                                                                            */
/******************************************************************************/
#define EMMC_BUFFER_SIZE 				16384

/******************************************************************************/
/* Structure Declaration                                                      */
/******************************************************************************/

typedef enum e_emmc_response
{
	EMMC_RESPONSE_48BITS	= 0x0000,
	EMMC_RESPONSE_NONE 		= 0x0001,
	EMMC_RESPONSE_STOP 		= 0x0002,
	EMMC_RESPONSE_FS 		= 0x0004,
	EMMC_RESPONSE_ABORT 	= 0x0008,
	EMMC_RESPONSE_BUSY 		= 0x0010,
	EMMC_RESPONSE_136BITS	= 0x0020,
	EMMC_RESPONSE_DATA 		= 0x0040,
	EMMC_RESPONSE_READ 		= 0x0080,
	EMMC_RESPONSE_WRITE 	= 0x0100,
}ENUM_EMMC_RESPONSE;

/******************************************************************************/
/* Defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* MMC_SWITCH access modes
 *
 * The SWITCH command response is of type R1b, therefore, the host should read
 *  the card status, using SEND_STATUS command, after the busy signal is
 *   de-asserted, to check the result of the SWITCH operation.
 *                                                                   		  */
/******************************************************************************/
#define MMC_SWITCH_MODE_CMD_SET         0x00    /* The command set is changed according to the Cmd Set field of the argument */
#define MMC_SWITCH_MODE_SET_BITS        0x01    /* The bits in the pointed byte are set, according to the �1� bits in the Value field. */
#define MMC_SWITCH_MODE_CLEAR_BITS      0x02    /* The bits in the pointed byte are cleared, according to the �1� bits in the Value field. */
#define MMC_SWITCH_MODE_WRITE_BYTE      0x03    /* The Value field is written into the pointed byte. */

/******************************************************************************/
/* EXT_CSD field definitions                                           		  */
/******************************************************************************/

#define EXT_CSD_WR_REL_PARAM_EN         (1<<2)

#define EXT_CSD_BOOT_WP_B_PWR_WP_DIS    (0x40)
#define EXT_CSD_BOOT_WP_B_PERM_WP_DIS   (0x10)
#define EXT_CSD_BOOT_WP_B_PERM_WP_EN    (0x04)
#define EXT_CSD_BOOT_WP_B_PWR_WP_EN     (0x01)

#define EXT_CSD_PART_CONFIG_ACC_MASK    (0x7)
#define EXT_CSD_PART_CONFIG_ACC_BOOT0   (0x1)
#define EXT_CSD_PART_CONFIG_ACC_RPMB    (0x3)
#define EXT_CSD_PART_CONFIG_ACC_GP0     (0x4)

#define EXT_CSD_PART_SUPPORT_PART_EN    (0x1)

#define EXT_CSD_CMD_SET_NORMAL          (1<<0)
#define EXT_CSD_CMD_SET_SECURE          (1<<1)
#define EXT_CSD_CMD_SET_CPSECURE        (1<<2)

#define EXT_CSD_CARD_TYPE_26    (1<<0)  /* Card can run at 26MHz */
#define EXT_CSD_CARD_TYPE_52    (1<<1)  /* Card can run at 52MHz */
#define EXT_CSD_CARD_TYPE_MASK  0x3F    /* Mask out reserved bits */
#define EXT_CSD_CARD_TYPE_DDR_1_8V  (1<<2)   /* Card can run at 52MHz */
                                              /* DDR mode @1.8V or 3V I/O */
#define EXT_CSD_CARD_TYPE_DDR_1_2V  (1<<3)   /* Card can run at 52MHz */
                                              /* DDR mode @1.2V I/O */
#define EXT_CSD_CARD_TYPE_DDR_52       (EXT_CSD_CARD_TYPE_DDR_1_8V  \
                                         | EXT_CSD_CARD_TYPE_DDR_1_2V)
 #define EXT_CSD_CARD_TYPE_SDR_1_8V      (1<<4)  /* Card can run at 200MHz */
#define EXT_CSD_CARD_TYPE_SDR_1_2V      (1<<5)  /* Card can run at 200MHz */
                                                 /* SDR mode @1.2V I/O */

#define EXT_CSD_BUS_WIDTH_1     0       /* Card is in 1 bit mode */
#define EXT_CSD_BUS_WIDTH_4     1       /* Card is in 4 bit mode */
#define EXT_CSD_BUS_WIDTH_8     2       /* Card is in 8 bit mode */
#define EXT_CSD_DDR_BUS_WIDTH_4 5       /* Card is in 4 bit DDR mode */
#define EXT_CSD_DDR_BUS_WIDTH_8 6       /* Card is in 8 bit DDR mode */

#define EXT_CSD_SEC_ER_EN       BIT(0)
#define EXT_CSD_SEC_BD_BLK_EN   BIT(2)
#define EXT_CSD_SEC_GB_CL_EN    BIT(4)
#define EXT_CSD_SEC_SANITIZE    BIT(6)  /* v4.5 only */

#define EXT_CSD_RST_N_EN_MASK   0x3
#define EXT_CSD_RST_N_ENABLED   1       /* RST_n is enabled on card */

#define EXT_CSD_NO_POWER_NOTIFICATION   0
#define EXT_CSD_POWER_ON                1
#define EXT_CSD_POWER_OFF_SHORT         2
#define EXT_CSD_POWER_OFF_LONG          3

#define EXT_CSD_PWR_CL_8BIT_MASK        0xF0    /* 8 bit PWR CLS */
#define EXT_CSD_PWR_CL_4BIT_MASK        0x0F    /* 8 bit PWR CLS */
#define EXT_CSD_PWR_CL_8BIT_SHIFT       4
#define EXT_CSD_PWR_CL_4BIT_SHIFT       0

#define EXT_CSD_PACKED_EVENT_EN BIT(3)

/******************************************************************************/
/* EXT_CSD switch command macros                                       		  */
/******************************************************************************/
#define EXT_CSD_FLUSH_CACHE             32      /* W */
#define EXT_CSD_CACHE_CTRL              33      /* R/W */
#define EXT_CSD_POWER_OFF_NOTIFICATION  34      /* R/W */
#define EXT_CSD_PACKED_FAILURE_INDEX    35      /* RO */
#define EXT_CSD_PACKED_CMD_STATUS       36      /* RO */
#define EXT_CSD_EXP_EVENTS_STATUS       54      /* RO, 2 bytes */
#define EXT_CSD_EXP_EVENTS_CTRL         56      /* R/W, 2 bytes */
#define EXT_CSD_DATA_SECTOR_SIZE        61      /* R */
#define EXT_CSD_GP_SIZE_MULT            143     /* R/W */
#define EXT_CSD_PARTITION_ATTRIBUTE     156     /* R/W */
#define EXT_CSD_PARTITION_SUPPORT       160     /* RO */
#define EXT_CSD_HPI_MGMT                161     /* R/W */
#define EXT_CSD_RST_N_FUNCTION          162     /* R/W */
#define EXT_CSD_BKOPS_EN                163     /* R/W */
#define EXT_CSD_BKOPS_START             164     /* W */
#define EXT_CSD_SANITIZE_START          165     /* W */
#define EXT_CSD_WR_REL_PARAM            166     /* RO */
#define EXT_CSD_RPMB_MULT               168     /* RO */
#define EXT_CSD_BOOT_WP                 173     /* R/W */
#define EXT_CSD_ERASE_GROUP_DEF         175     /* R/W */
#define EXT_CSD_PART_CONFIG             179     /* R/W */
#define EXT_CSD_ERASED_MEM_CONT         181     /* RO */
#define EXT_CSD_BUS_WIDTH               183     /* R/W */
#define EXT_CSD_HS_TIMING               185     /* R/W */
#define EXT_CSD_POWER_CLASS             187     /* R/W */
#define EXT_CSD_REV                     192     /* RO */
#define EXT_CSD_STRUCTURE               194     /* RO */
#define EXT_CSD_CARD_TYPE               196     /* RO */
#define EXT_CSD_OUT_OF_INTERRUPT_TIME   198     /* RO */
#define EXT_CSD_PART_SWITCH_TIME        199     /* RO */
#define EXT_CSD_PWR_CL_52_195           200     /* RO */
#define EXT_CSD_PWR_CL_26_195           201     /* RO */
#define EXT_CSD_PWR_CL_52_360           202     /* RO */
#define EXT_CSD_PWR_CL_26_360           203     /* RO */
#define EXT_CSD_SEC_CNT                 212     /* RO, 4 bytes */
#define EXT_CSD_S_A_TIMEOUT             217     /* RO */
#define EXT_CSD_REL_WR_SEC_C            222     /* RO */
#define EXT_CSD_HC_WP_GRP_SIZE          221     /* RO */
#define EXT_CSD_ERASE_TIMEOUT_MULT      223     /* RO */
#define EXT_CSD_HC_ERASE_GRP_SIZE       224     /* RO */
#define EXT_CSD_BOOT_MULT               226     /* RO */
#define EXT_CSD_SEC_TRIM_MULT           229     /* RO */
#define EXT_CSD_SEC_ERASE_MULT          230     /* RO */
#define EXT_CSD_SEC_FEATURE_SUPPORT     231     /* RO */
#define EXT_CSD_TRIM_MULT               232     /* RO */
#define EXT_CSD_PWR_CL_200_195          236     /* RO */
#define EXT_CSD_PWR_CL_200_360          237     /* RO */
#define EXT_CSD_PWR_CL_DDR_52_195       238     /* RO */
#define EXT_CSD_PWR_CL_DDR_52_360       239     /* RO */
#define EXT_CSD_BKOPS_STATUS            246     /* RO */
#define EXT_CSD_POWER_OFF_LONG_TIME     247     /* RO */
#define EXT_CSD_GENERIC_CMD6_TIME       248     /* RO */
#define EXT_CSD_CACHE_SIZE              249     /* RO, 4 bytes */
#define EXT_CSD_TAG_UNIT_SIZE           498     /* RO */
#define EXT_CSD_DATA_TAG_SUPPORT        499     /* RO */
#define EXT_CSD_MAX_PACKED_WRITES       500     /* RO */
#define EXT_CSD_MAX_PACKED_READS        501     /* RO */
#define EXT_CSD_BKOPS_SUPPORT           502     /* RO */
#define EXT_CSD_HPI_FEATURES            503     /* RO */

/******************************************************************************/
/* Global Variable                                                            */
/******************************************************************************/

/******************************************************************************/
/* Function Declarations                                                      */
/******************************************************************************/
void Init_EMMC(void);
void EMMC_PinMuxSetup(void);
void EMMC_HSMMCSDModuleClkConfig(void);
void EMMC_HardwareReset(unsigned char state);
unsigned int EMMC_SetUpController(void);
unsigned int EMMC_CardInit(void);
unsigned char EMMC_SendCommand(unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_EMMC_RESPONSE type, unsigned int* response);
unsigned char EMMC_SendAppCommand(unsigned int command, unsigned int argument, unsigned int nblks, unsigned int blocksize, ENUM_EMMC_RESPONSE type, unsigned int* response);
void EMMC_ReceiveData(unsigned char* p_buffer, unsigned int length);
void EMMC_TransmitData(unsigned char* p_buffer, unsigned int length);
unsigned int EMMC_WriteBlocks(unsigned int block, unsigned int nblks, unsigned char *ptr);
unsigned int EMMC_ReadBlocks(unsigned int block, unsigned int nblks, unsigned char *ptr);
unsigned int EMMC_GetNumberBlocks(void);
void EMMC_SetInitialized(void);
void EMMC_ClearInitialized(void);
unsigned char EMMC_IsInitialized(void);
unsigned char EMMC_SendCommandSwitch(unsigned int set, unsigned int index, unsigned int value);
unsigned char EMMC_SendStatus(unsigned int* status);
unsigned char EMMC_TestWrite(void);

#endif
/******************************* End of file *********************************/
