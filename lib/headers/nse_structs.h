#ifndef P1V2_LIB_HEADERS_NSE_STRUCTS_H_
#define P1V2_LIB_HEADERS_NSE_STRUCTS_H_
#include <stdint.h>
#include <math.h>

const int header_size = 8;

struct header_t {
  int16_t  msg_len;
  int16_t  stream_id;
  uint32_t seq_no;
} __attribute__( ( packed ) );

static_assert( header_size == sizeof( header_t ) );

struct trade_message_t {
  int8_t   m_msg_type__;
  int64_t  m_time_stamp__;
  double_t m_buy_order_id__;
  double_t m_sell_order_id__;
  int32_t  m_token__;
  int32_t  m_trade_price__;
  int32_t  m_trade_qty__;
} __attribute__( ( packed ) );

/**
 * This  struct defines the structure to store the new/modify/cancel orders
 * received from the multicast tbt feed
 */
struct order_message_t {
  int8_t   m_msg_type__;
  int64_t  m_time_stamp__;
  double_t m_order_id__;
  int32_t  m_token__;
  int8_t   m_order_type__;
  int32_t  m_price__;
  int32_t  m_qty__;
} __attribute__( ( packed ) );

struct multicast_tbt_message_types_t {
  static const int8_t NewOrder          = 'N';
  static const int8_t ModificationOrder = 'M';
  static const int8_t CancelOrder       = 'X';
  static const int8_t TradeMessage      = 'T';
};

/*Order types*/

struct multicast_tbt_order_types_t {
  static const int8_t Buy  = 'B';
  static const int8_t Sell = 'S';
};
#endif