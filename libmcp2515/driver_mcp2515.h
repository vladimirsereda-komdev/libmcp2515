#ifndef DRIVER_MCP2515_H
#define DRIVER_MCP2515_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MCP_Instance MCP_Instance;

#define MCP_BUFFER_SIZE (uint8_t) 32U

#define MCP_OK (int32_t) 0
#define MCP_ERROR (int32_t)(-1)
#define MCP_ERROR_BUFFER (int32_t)(-2)

/// @brief Структура для описания конкретного экземпляра драйвера
struct MCP_Instance
{
  /// @brief 
  void (*chipSelect)(bool select);

  /// @brief 
  int32_t (*transaction)(uint8_t* data, uint8_t len);

  /// @brief 
  uint8_t buffer[MCP_BUFFER_SIZE];
};

/// @brief Читает данные из регистров по выбранному адресу
int32_t mcpRead(MCP_Instance* ins, uint8_t addr, uint8_t** data, uint8_t len);

/// @brief 
typedef enum
{
  MCP_READRXBUFFER_RXB0SIDH = 0x90U,
  MCP_READRXBUFFER_RXB0D0   = 0x92U,
  MCP_READRXBUFFER_RXB1SIDH = 0x94U,
  MCP_READRXBUFFER_RXB1D0   = 0x96U
} MCPReadRxBufferType;

/// @brief
/// @param
/// @param
/// @param
int32_t mcpReadRxBuffer(MCP_Instance* ins, MCPReadRxBufferType type, uint8_t** data, uint8_t* len);

/// @brief
/// @param
/// @param
/// @param
/// @param
int32_t mcpWrite(MCP_Instance* ins, uint8_t addr, uint8_t* data, uint8_t len);

/// @brief 
typedef enum
{
  MCP_LOADTXBUFFER_TXB0SIDH = 0x40U,
  MCP_LOADTXBUFFER_TXB0D0   = 0x41U,
  MCP_LOADTXBUFFER_TXB1SIDH = 0x42U,
  MCP_LOADTXBUFFER_TXB1D0   = 0x43U,
  MCP_LOADTXBUFFER_TXB2SIDH = 0x44U,
  MCP_LOADTXBUFFER_TXB2D0   = 0x45U
} MCPLoadTxBufferType;

/// @brief
/// @param
/// @param
int32_t mcpLoadTxBuffer(MCP_Instance* ins, MCPLoadTxBufferType type, uint8_t* data);

/// @brief
/// @param
/// @param
/// @param
/// @param
int32_t mcpBitModify(MCP_Instance* ins, uint8_t addr, uint8_t mask, uint8_t data);

#define MCP_RTSCMD_BUFFER0 0x80U
#define MCP_RTSCMD_BUFFER1 0x82U
#define MCP_RTSCMD_BUFFER2 0x84U

/// @brief
/// @param
int32_t mcpRTS(MCP_Instance* ins, uint8_t cmd);

/// @brief
/// @param
int32_t mcpReadStatus(MCP_Instance* ins);

/// @brief
/// @param
int32_t mcpRxStatus(MCP_Instance* ins);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // DRIVER_MCP2515_H
