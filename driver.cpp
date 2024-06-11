/* Author: YSS Narasimha Naidu
   Date: June 10th 2024 
   Purpose: generating orderbook from split data
*/
#include <cassert>
#include <fstream>
#include <iostream>

#include "nse_structs.h"
#include "orderbook.h"

int main( int argv, char **argc ) {
  // Usage:
  // ./driver <path_to_split_data_file>
  assert( argv == 2 );
  // char *split_data_file =
  // "/opt/nse_uncompressed_split/fo/20240301/BANKNIFTY24MAYFUT";

  std::fstream fp( argc[1], std::ios::in );

  uint      messages_cnt = 0;
  header_t  readbuff;
  char      databuff[100];
  int64_t   prev_ts = 0;
  orderbook ob;

  while ( !fp.eof() ) {
    fp.read( ( char * )&readbuff, sizeof( readbuff ) );
    if ( fp.gcount() != header_size ) {
      break;
    }
    int payload_len = readbuff.msg_len - header_size;

    // fp.seekg( payload_len, std::ios_base::cur );
    fp.read( databuff, payload_len );
    if ( fp.gcount() != payload_len ) {
      break;
    }
    order_message_t *msg     = reinterpret_cast<order_message_t *>( databuff );
    int64_t          curr_ts = msg->m_time_stamp__;
    if ( prev_ts != 0 ) {
      if ( curr_ts < prev_ts ) {
        std::cout << curr_ts << " " << prev_ts << "\n";
        break;
      }
    }

    messages_cnt += 1;

    if ( msg->m_msg_type__ == multicast_tbt_message_types_t::NewOrder ) {
      // std::cout << "ADD " << msg->order_type << " " << msg->price << "\n";
      ob.add_order( *msg );
    } else if ( msg->m_msg_type__ ==
                multicast_tbt_message_types_t::CancelOrder ) {
      //  std::cout << "CANCEL " << msg->order_type << " " << msg->price <<
      //  "\n";
      ob.cancel_order( *msg );
    } else if ( msg->m_msg_type__ =
                    multicast_tbt_message_types_t::ModificationOrder ) {
      // std::cout << "MODIFY "  << msg->order_type << " " << msg->price <<
      // "\n";
      ob.modify_order( *msg );
    } else if ( msg->m_msg_type__ =
                    multicast_tbt_message_types_t::TradeMessage ) {
      // std::cout << "TRADE " << msg->order_type << " " << msg->price << "\n";
      ob.process_transaction(
          *reinterpret_cast<trade_message_t *>( databuff ) );
    }

    if ( ob.get_top_ask() <= ob.get_top_bid() ) {
      std::cout << "timestamp:" << curr_ts << "\n";
      ob.print_orderbook();
      prev_ts = curr_ts;
      std::cout << "orders:" << ob.get_total_orders() << "\n";
    }

    assert( ob.get_top_ask() > ob.get_top_bid() );

    if ( curr_ts >= prev_ts + 10000000000 ) {
      std::cout << "timestamp:" << curr_ts << "\n";
      ob.print_orderbook();
      prev_ts = curr_ts;
      std::cout << "orders:" << ob.get_total_orders() << "\n";
    }
  }
}