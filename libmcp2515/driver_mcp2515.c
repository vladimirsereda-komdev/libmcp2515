#include "driver_mcp2515.h"

#define OFFSET_CMD_READ 2
#define OFFSET_CMD_READBUFFER 1
#define OFFSET_CMD_WRITE 2
#define OFFSET_CMD_LOADBUFFER 1
#define OFFSET_CMD_BITMODIFY 4
#define OFFSET_CMD_RTS 1
#define OFFSET_CMD_READSTATUS 1
#define OFFSET_CMD_RXSTATUS 1

int32_t mcpRead(MCP_Instance* ins, uint8_t addr, uint8_t** data, uint8_t len)
{
  ins->buffer[0] = 0x03;
  ins->buffer[1] = addr;

  len += OFFSET_CMD_READ;
  if (len > MCP_BUFFER_SIZE)
  {
    return MCP_ERROR_BUFFER;
  }

  ins->chipSelect(true);
  int32_t res = ins->transaction(&ins->buffer[0], len);
  ins->chipSelect(false);

  *data = &ins->buffer[OFFSET_CMD_READ];
  return res;
}

int32_t mcpReadRxBuffer(MCP_Instance* ins, MCPReadRxBufferType type, uint8_t** data, uint8_t* len)
{
  ins->buffer[0] = (uint8_t) type;
  *len           = ((uint8_t) type & (uint8_t) 0x02) ? 8 : 13;

  ins->chipSelect(true);
  int32_t res = ins->transaction(&ins->buffer[0], *len + OFFSET_CMD_READBUFFER);
  ins->chipSelect(false);

  *data = &ins->buffer[OFFSET_CMD_READBUFFER];
  return res;
}

int32_t mcpWrite(MCP_Instance* ins, uint8_t addr, uint8_t* data, uint8_t len)
{
  uint8_t l = len;

  len += OFFSET_CMD_WRITE;
  if (len > MCP_BUFFER_SIZE)
  {
    return MCP_ERROR_BUFFER;
  }

  uint8_t* ptr = &ins->buffer[0];
  *ptr++       = 0x02;
  *ptr++       = addr;
  while (l--)
  {
    *ptr++ = *data++;
  }

  ins->chipSelect(true);
  int32_t res = ins->transaction(&ins->buffer[0], len);
  ins->chipSelect(false);

  return res;
}

int32_t mcpLoadTxBuffer(MCP_Instance* ins, MCPLoadTxBufferType type, uint8_t* data)
{
  uint8_t l   = ((uint8_t) type & (uint8_t) 0x01) ? 8 : 13;
  uint8_t len = l + OFFSET_CMD_LOADBUFFER;

  uint8_t* ptr = &ins->buffer[0];
  *ptr++       = (uint8_t) type;
  while (l--)
  {
    *ptr++ = *data++;
  }

  ins->chipSelect(true);
  int32_t res = ins->transaction(&ins->buffer[0], len);
  ins->chipSelect(false);

  return res;
}

int32_t mcpBitModify(MCP_Instance* ins, uint8_t addr, uint8_t mask, uint8_t data)
{
  ins->buffer[0] = 0x05;
  ins->buffer[1] = addr;
  ins->buffer[2] = mask;
  ins->buffer[3] = data;

  ins->chipSelect(true);
  int32_t res = ins->transaction(&ins->buffer[0], OFFSET_CMD_BITMODIFY);
  ins->chipSelect(false);

  return res;
}

int32_t mcpRTS(MCP_Instance* ins, uint8_t cmd)
{
  ins->buffer[0] = cmd;

  ins->chipSelect(true);
  int32_t res = ins->transaction(&ins->buffer[0], OFFSET_CMD_RTS);
  ins->chipSelect(false);

  return res;
}

int32_t mcpReadStatus(MCP_Instance* ins)
{
  ins->buffer[0] = 0xA0;

  ins->chipSelect(true);
  int32_t res = ins->transaction(&ins->buffer[0], OFFSET_CMD_READSTATUS);
  ins->chipSelect(false);

  return (res + (int32_t) ins->buffer[1]);
}

int32_t mcpRxStatus(MCP_Instance* ins)
{
  ins->buffer[0] = 0xB0;

  ins->chipSelect(true);
  int32_t res = ins->transaction(&ins->buffer[0], OFFSET_CMD_READSTATUS);
  ins->chipSelect(false);

  return (res + (int32_t) ins->buffer[1]);
}
