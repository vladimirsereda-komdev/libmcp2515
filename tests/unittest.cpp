#include "catch/catch.hpp"
#include "../libmcp2515/driver_mcp2515.h"
#include "string.h"

static uint8_t BufferTx[MCP_BUFFER_SIZE];
static uint8_t BufferRx[MCP_BUFFER_SIZE];
static uint8_t BufferNULL[MCP_BUFFER_SIZE] = {0};

static bool     SelectState[4];
static uint32_t SelectPtr;

static int32_t TransactionError;

static void chipSelect(bool select)
{
  SelectState[SelectPtr++] = select;
  SelectPtr &= (sizeof(SelectState) - 1);
}

static int32_t transaction(uint8_t* data, uint8_t len)
{
  memcpy(&BufferTx[0], data, len);
  memcpy(data, &BufferRx[0], len);
  return TransactionError;
}

void resetState()
{
  SelectPtr = 0;
  memset(&SelectState[0], 0, sizeof(SelectState));
  memset(&BufferTx[0], 0, sizeof(BufferTx));
}

TEST_CASE("Read registers")
{
  MCP_Instance ins;
  uint8_t      addr;
  uint8_t      len;
  uint8_t*     data;

  ins.chipSelect  = chipSelect;
  ins.transaction = transaction;

  // просто чтение одного регистра
  addr = 0x00;
  len  = 1;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpRead(&ins, addr, &data, len));
  REQUIRE(&ins.buffer[2] == data);
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x03);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(0 == memcmp(&BufferTx[2], &BufferNULL[0], len));

  // а если длина равна нулю?
  addr = 0x00;
  len  = 0;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpRead(&ins, addr, &data, len));
  REQUIRE(&ins.buffer[2] == data);
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x03);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(0 == memcmp(&BufferTx[2], &BufferNULL[0], len));

  // а если длина впритык размеру буфера?
  addr = 0x00;
  len  = 30;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpRead(&ins, addr, &data, len));
  REQUIRE(&ins.buffer[2] == data);
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x03);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(0 == memcmp(&BufferTx[2], &BufferNULL[0], len));

  // а если длина не влезает в буфер?
  addr = 0x00;
  len  = 31;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_ERROR_BUFFER == mcpRead(&ins, addr, &data, len));
  REQUIRE(&ins.buffer[2] == data);
  REQUIRE(SelectState[0] == false);
  REQUIRE(SelectState[1] == false);
  REQUIRE(0 == memcmp(&BufferTx[0], &BufferNULL[0], sizeof(BufferTx)));

  // а если была ошибка транзакции?
  addr = 0x00;
  len  = 5;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_ERROR;
  REQUIRE(0 > mcpRead(&ins, addr, &data, len));
  REQUIRE(&ins.buffer[2] == data);
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x03);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(0 == memcmp(&BufferTx[2], &data[0], len));
}

TEST_CASE("Read rx buffer")
{
  MCP_Instance        ins;
  MCPReadRxBufferType type;
  uint8_t*            data;
  uint8_t             len;

  ins.chipSelect  = chipSelect;
  ins.transaction = transaction;

  // чтение буфера 0 со всеми заголовками
  type = MCP_READRXBUFFER_RXB0SIDH;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpReadRxBuffer(&ins, type, &data, &len));
  REQUIRE(&ins.buffer[1] == data);
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(len == 13);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], &data[0], len));

  // чтение буфера 0 только данных
  type = MCP_READRXBUFFER_RXB0D0;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpReadRxBuffer(&ins, type, &data, &len));
  REQUIRE(&ins.buffer[1] == data);
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(len == 8);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], &data[0], len));

  // чтение буфера 1 со всеми заголовками
  type = MCP_READRXBUFFER_RXB1SIDH;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpReadRxBuffer(&ins, type, &data, &len));
  REQUIRE(&ins.buffer[1] == data);
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(len == 13);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], &data[0], len));

  // чтение буфера 1 только данных
  type = MCP_READRXBUFFER_RXB1D0;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpReadRxBuffer(&ins, type, &data, &len));
  REQUIRE(&ins.buffer[1] == data);
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(len == 8);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], &data[0], len));

  // а если в транзакции ошибка?
  type = MCP_READRXBUFFER_RXB1D0;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_ERROR;
  REQUIRE(MCP_ERROR == mcpReadRxBuffer(&ins, type, &data, &len));
  REQUIRE(&ins.buffer[1] == data);
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  // REQUIRE (len == 8);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], &data[0], len));
}

TEST_CASE("Write registers")
{
  MCP_Instance ins;
  uint8_t      addr;
  uint8_t      data[40];
  uint8_t      len;

  ins.chipSelect  = chipSelect;
  ins.transaction = transaction;

  for (uint8_t i = 0; i < (uint8_t) sizeof(data); i++)
    data[i] = i;

  // просто запись регистров
  addr = 0x00;
  len  = 5;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpWrite(&ins, addr, data, len));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x02);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(0 == memcmp(&BufferTx[2], data, len));
  REQUIRE(0 == memcmp(&BufferTx[2 + len], &BufferNULL[0], sizeof(BufferTx) - len - 2));

  // а если длина равна нулю?
  addr = 0x00;
  len  = 0;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpWrite(&ins, addr, data, len));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x02);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(0 == memcmp(&BufferTx[2], data, len));
  REQUIRE(0 == memcmp(&BufferTx[2 + len], &BufferNULL[0], sizeof(BufferTx) - len - 2));

  // а если длина впритык размеру буфера?
  addr = 0x00;
  len  = 30;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpWrite(&ins, addr, data, len));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x02);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(0 == memcmp(&BufferTx[2], data, len));
  REQUIRE(0 == memcmp(&BufferTx[2 + len], &BufferNULL[0], sizeof(BufferTx) - len - 2));

  // а если длина не влезает в буфер?
  addr = 0x00;
  len  = 31;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_ERROR_BUFFER == mcpWrite(&ins, addr, data, len));
  REQUIRE(SelectState[0] == false);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x00);
  REQUIRE(BufferTx[1] == 0x00);
  REQUIRE(0 == memcmp(&BufferTx[0], &BufferNULL[0], sizeof(BufferTx)));

  // а если в транзакции была ошибка?
  addr = 0x00;
  len  = 5;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_ERROR;
  REQUIRE(MCP_ERROR == mcpWrite(&ins, addr, data, len));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x02);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(0 == memcmp(&BufferTx[2], data, len));
  REQUIRE(0 == memcmp(&BufferTx[2 + len], &BufferNULL[0], sizeof(BufferTx) - len - 2));
}

TEST_CASE("Load tx buffer")
{
  MCP_Instance        ins;
  MCPLoadTxBufferType type;
  uint8_t             data[40];

  ins.chipSelect  = chipSelect;
  ins.transaction = transaction;

  for (uint8_t i = 0; i < sizeof(data); i++)
    data[i] = i;

  // загрузка буфера 0 с заголовками
  type = MCP_LOADTXBUFFER_TXB0SIDH;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpLoadTxBuffer(&ins, type, data));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], data, 13));
  REQUIRE(0 == memcmp(&BufferTx[1 + 13], &BufferNULL[0], sizeof(BufferTx) - 13 - 1));

  // загрузка буфера 0 только данных
  type = MCP_LOADTXBUFFER_TXB0D0;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpLoadTxBuffer(&ins, type, data));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], data, 8));
  REQUIRE(0 == memcmp(&BufferTx[1 + 8], &BufferNULL[0], sizeof(BufferTx) - 8 - 1));

  // загрузка буфера 1 с заголовками
  type = MCP_LOADTXBUFFER_TXB1SIDH;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpLoadTxBuffer(&ins, type, data));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], data, 13));
  REQUIRE(0 == memcmp(&BufferTx[1 + 13], &BufferNULL[0], sizeof(BufferTx) - 13 - 1));

  // загрузка буфера 1 только данных
  type = MCP_LOADTXBUFFER_TXB1D0;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpLoadTxBuffer(&ins, type, data));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], data, 8));
  REQUIRE(0 == memcmp(&BufferTx[1 + 8], &BufferNULL[0], sizeof(BufferTx) - 8 - 1));

  // а если была ошибка в транзакции?
  type = MCP_LOADTXBUFFER_TXB0D0;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_ERROR;
  REQUIRE(MCP_ERROR == mcpLoadTxBuffer(&ins, type, data));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == type);
  REQUIRE(0 == memcmp(&BufferTx[1], data, 8));
  REQUIRE(0 == memcmp(&BufferTx[1 + 8], &BufferNULL[0], sizeof(BufferTx) - 8 - 1));
}

TEST_CASE("Bit modify")
{
  MCP_Instance ins;
  uint8_t      addr;
  uint8_t      mask;
  uint8_t      data;

  ins.chipSelect  = chipSelect;
  ins.transaction = transaction;

  // просто модификация
  addr = 0;
  mask = 0x55;
  data = 0xFF;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpBitModify(&ins, addr, mask, data));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x05);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(BufferTx[2] == mask);
  REQUIRE(BufferTx[3] == data);
  REQUIRE(0 == memcmp(&BufferTx[4], &BufferNULL[0], sizeof(BufferTx) - 4));

  // а если ошибка в транзакции?
  addr = 0;
  mask = 0x55;
  data = 0xFF;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_ERROR;
  REQUIRE(MCP_ERROR == mcpBitModify(&ins, addr, mask, data));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0x05);
  REQUIRE(BufferTx[1] == addr);
  REQUIRE(BufferTx[2] == mask);
  REQUIRE(BufferTx[3] == data);
  REQUIRE(0 == memcmp(&BufferTx[4], &BufferNULL[0], sizeof(BufferTx) - 4));
}

TEST_CASE("RTS")
{
  MCP_Instance ins;
  uint8_t      cmd;

  ins.chipSelect  = chipSelect;
  ins.transaction = transaction;

  // буфер 0
  cmd = MCP_RTSCMD_BUFFER0;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpRTS(&ins, cmd));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == (cmd | 0x80));
  REQUIRE(0 == memcmp(&BufferTx[1], &BufferNULL[0], sizeof(BufferTx) - 1));

  // буфер 1
  cmd = MCP_RTSCMD_BUFFER1;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpRTS(&ins, cmd));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == cmd);
  REQUIRE(0 == memcmp(&BufferTx[1], &BufferNULL[0], sizeof(BufferTx) - 1));

  // буфер 2
  cmd = MCP_RTSCMD_BUFFER2;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpRTS(&ins, cmd));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == cmd);
  REQUIRE(0 == memcmp(&BufferTx[1], &BufferNULL[0], sizeof(BufferTx) - 1));

  // несколько буферов
  cmd = MCP_RTSCMD_BUFFER0 | MCP_RTSCMD_BUFFER2;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpRTS(&ins, cmd));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == cmd);
  REQUIRE(0 == memcmp(&BufferTx[1], &BufferNULL[0], sizeof(BufferTx) - 1));

  // а если ошибка в транзакции?
  cmd = MCP_RTSCMD_BUFFER2;
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_ERROR;
  REQUIRE(MCP_ERROR == mcpRTS(&ins, cmd));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == cmd);
  REQUIRE(0 == memcmp(&BufferTx[1], &BufferNULL[0], sizeof(BufferTx) - 1));
}

TEST_CASE("Read status")
{
  MCP_Instance ins;

  ins.chipSelect  = chipSelect;
  ins.transaction = transaction;

  // просто чтение
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpReadStatus(&ins));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0xA0);
  REQUIRE(0 == memcmp(&BufferTx[1], &BufferNULL[0], sizeof(BufferTx) - 1));

  // а если ошибка в транзакции?
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_ERROR;
  REQUIRE(MCP_ERROR == mcpReadStatus(&ins));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0xA0);
  REQUIRE(0 == memcmp(&BufferTx[1], &BufferNULL[0], sizeof(BufferTx) - 1));
}

TEST_CASE("Rx status")
{
  MCP_Instance ins;

  ins.chipSelect  = chipSelect;
  ins.transaction = transaction;

  // просто чтение
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_OK;
  REQUIRE(MCP_OK == mcpRxStatus(&ins));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0xB0);
  REQUIRE(0 == memcmp(&BufferTx[1], &BufferNULL[0], sizeof(BufferTx) - 1));

  // а если ошибка в транзакции?
  resetState();
  memset(&ins.buffer[0], 0, MCP_BUFFER_SIZE);
  TransactionError = MCP_ERROR;
  REQUIRE(MCP_ERROR == mcpRxStatus(&ins));
  REQUIRE(SelectState[0] == true);
  REQUIRE(SelectState[1] == false);
  REQUIRE(BufferTx[0] == 0xB0);
  REQUIRE(0 == memcmp(&BufferTx[1], &BufferNULL[0], sizeof(BufferTx) - 1));
}
