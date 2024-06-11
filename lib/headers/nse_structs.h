#ifndef P1V2_LIB_HEADERS_NSE_STRUCTS_H_
#define P1V2_LIB_HEADERS_NSE_STRUCTS_H_
#include <bits/stdc++.h>


const int header_size = 8;

#pragma pack( 1 )
struct header_t {
  short    msg_len;
  short    stream_id;
  uint32_t seq_no;
};

static_assert( header_size == sizeof( header_t ) );


struct trade_message {
  int8_t   msg_type;
  int64_t  time_stamp;
  double_t buy_order_id;
  double_t sell_order_id;
  int32_t  token;
  int32_t  trade_price;
  int32_t  trade_qty;
} __attribute__( ( packed ) );

/**
 * This  struct defines the structure to store the new/modify/cancel orders
 * received from the multicast tbt feed
 */
struct order_message {
  int8_t   msg_type;
  int64_t  time_stamp;
  double_t order_id;
  int32_t  token;
  int8_t   order_type;
  int32_t  price;
  int32_t  qty;
} __attribute__( ( packed ) );

struct multicast_tbt_message_types {
  static const int8_t NewOrder          = 'N';
  static const int8_t ModificationOrder = 'M';
  static const int8_t CancelOrder       = 'X';
  static const int8_t TradeMessage      = 'T';
};

/*Order types*/

struct multicast_tbt_order_types {
  static const int8_t Buy  = 'B';
  static const int8_t Sell = 'S';
};
#endif