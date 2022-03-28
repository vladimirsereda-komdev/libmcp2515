#ifndef DRIVER_MCP2515_H
#define DRIVER_MCP2515_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MCP_Instance MCP_Instance;

#define MCP_BUFFER_SIZE (uint8_t) 32U

#define MCP_OK           (int32_t) 0   ///< Операция выполнена успешно
#define MCP_ERROR        (int32_t)(-1) ///< Возникли неизвестные ошибки
#define MCP_ERROR_BUFFER (int32_t)(-2) ///< Возникли ошибки, связанные с переполнением буфера

/// @brief Структура для описания конкретного экземпляра драйвера
struct MCP_Instance
{
  /// @brief Вызывается для установки сигнала CS интерфейса SPI
  /// @param [in] select уровень сигнала CS
  ///        true  устанавливает низкий логический уровень;
  ///        false устанавливает высокий логический уровень
  /// @details Пользователь библиотеки должен сам реализовать данную функцию
  void (*chipSelect)(bool select);

  /// @brief Вызывается, когда необходимо передать данные по интерфейсу SPI
  /// @param [in] data указатель на данные, которые необходимо передать
  /// @param [in] len количество данных (байт), которое необходимо передать
  /// @return MCP_OK, если транзакция данных завершена успешно;
  ///         иначе возвращает код ошибки
  /// @details Пользователь библиотеки должен сам реализовать данную функцию.
  /// Принимаемые по SPI данные необходимо помещать по адресу data
  int32_t (*transaction)(uint8_t* data, uint8_t len);

  /// @brief Буферный массив для формирования и приема данных SPI протокола
  /// @details Пользователь не должен напрямую обращаться к данному полю
  uint8_t buffer[MCP_BUFFER_SIZE];
};

/// @brief Читает данные из регистров MCP2515
/// @param [in] ins указатель на экземпляр драйвера
/// @param [in] addr адрес, начиная с которого необходимо читать данные из MCP2515
/// @param [out] data сюда запишется адрес считанных данных по окончанию операции
/// @param [in] len количество данных (байт), которое необходимо прочитать
/// @return MCP_OK, если транзакция данных завершена успешно;
///         иначе возвращает код ошибки
/// @details Обратите внимание, что data представляет собой указатель на
/// указатель
int32_t mcpRead(MCP_Instance* ins, uint8_t addr, uint8_t** data, uint8_t len);

/// @brief Возможные варианты чтения приемного буфера
typedef enum
{
  MCP_READRXBUFFER_RXB0SIDH = 0x90U, ///< Читать буфер 0, начиная с SIDH
  MCP_READRXBUFFER_RXB0D0   = 0x92U, ///< Читать буфер 0, начиная с D0 
  MCP_READRXBUFFER_RXB1SIDH = 0x94U, ///< Читать буфер 1, начиная с SIDH
  MCP_READRXBUFFER_RXB1D0   = 0x96U  ///< Читать буфер 1, начиная с D0 
} MCPReadRxBufferType;

/// @brief Читает данные из приемного буфера MCP2515
/// @param [in] ins указатель на экземпляр драйвера
/// @param [in] type тип операции (см. MCPReadRxBufferType)
/// @param [out] data сюда запишется адрес считанных данных по окончанию операции
/// @param [out] len количество данных (байт), которое было прочитано
/// @return MCP_OK, если транзакция данных завершена успешно;
///         иначе возвращает код ошибки
/// @details Обратите внимание, что data представляет собой указатель на
/// указатель. @b
/// Количество данных len указывает не на размер полезной нагрузки фрейма (DLC),
/// а на то, сколько байт было прочитано за транзакцию.
int32_t mcpReadRxBuffer(MCP_Instance* ins, MCPReadRxBufferType type, uint8_t** data, uint8_t* len);

/// @brief Записывает данные в регистры MCP2515
/// @param [in] ins указатель на экземпляр драйвера
/// @param [in] addr адрес, начиная с которого необходимо записывать данные в MCP2515
/// @param [in] data указатель на данные, которые необходимо записать
/// @param [in] len количество данных (байт), которое необходимо записать
/// @return MCP_OK, если транзакция данных завершена успешно;
///         иначе возвращает код ошибки
int32_t mcpWrite(MCP_Instance* ins, uint8_t addr, uint8_t* data, uint8_t len);

/// @brief Возможные варианты записи передающего буфера
typedef enum
{
  MCP_LOADTXBUFFER_TXB0SIDH = 0x40U, ///< Записать в буфер 0, начиная с SIDH
  MCP_LOADTXBUFFER_TXB0D0   = 0x41U, ///< Записать в буфер 0, начиная с D0
  MCP_LOADTXBUFFER_TXB1SIDH = 0x42U, ///< Записать в буфер 1, начиная с SIDH
  MCP_LOADTXBUFFER_TXB1D0   = 0x43U, ///< Записать в буфер 1, начиная с D0
  MCP_LOADTXBUFFER_TXB2SIDH = 0x44U, ///< Записать в буфер 2, начиная с SIDH
  MCP_LOADTXBUFFER_TXB2D0   = 0x45U  ///< Записать в буфер 2, начиная с D0
} MCPLoadTxBufferType;

/// @brief Записывает данные в передающий буфер MCP2515
/// @param [in] ins указатель на экземпляр драйвера
/// @param [in] type тип операции (см. MCPLoadTxBufferType)
/// @param [in] data указатель на дынне, которые необходимо записать
/// @return MCP_OK, если транзакция данных завершена успешно;
///         иначе возвращает код ошибки
int32_t mcpLoadTxBuffer(MCP_Instance* ins, MCPLoadTxBufferType type, uint8_t* data);

/// @brief Побитово модифицирует значение регистра MCP2515
/// @param [in] ins указатель на экземпляр драйвера
/// @param [in] addr адрес, содержимое которого необходимо модифицировать
/// @param [in] mask маска, применяемая к содержимому адреса
/// @param [in] data записываемое значение
/// @return MCP_OK, если транзакция данных завершена успешно;
///         иначе возвращает код ошибки
int32_t mcpBitModify(MCP_Instance* ins, uint8_t addr, uint8_t mask, uint8_t data);

#define MCP_RTSCMD_BUFFER0 0x80U ///< Отправить данные из передающего буфера 0
#define MCP_RTSCMD_BUFFER1 0x82U ///< Отправить данные из передающего буфера 1
#define MCP_RTSCMD_BUFFER2 0x84U ///< Отправить данные из передающего буфера 2

/// @brief Команда отправки данных из передающего буфера MCP2515
/// @param [in] ins указатель на экземпляр драйвера
/// @param [in] cmd тип операции (см. MCP_RTSCMD_*)
/// @return MCP_OK, если транзакция данных завершена успешно;
///         иначе возвращает код ошибки
int32_t mcpRTS(MCP_Instance* ins, uint8_t cmd);

/// @brief Команда чтения статуса MCP2515
/// @param [in] ins указатель на экземпляр драйвера
/// @return MCP_OK, если транзакция данных завершена успешно;
///         иначе возвращает код ошибки
int32_t mcpReadStatus(MCP_Instance* ins);

/// @brief Команда чтения статуса приема MCP2515
/// @param [in] ins указатель на экземпляр драйвера
/// @return MCP_OK, если транзакция данных завершена успешно;
///         иначе возвращает код ошибки
int32_t mcpRxStatus(MCP_Instance* ins);

#ifdef __cplusplus
}
#endif  // __cplusplus
#endif  // DRIVER_MCP2515_H
