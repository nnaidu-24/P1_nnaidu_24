#ifndef P1V2_LIB_HEADERS_ORDERBOOK_H
#define P1V2_LIB_HEADERS_ORDERBOOK_H
#include "nse_structs.h"
#include <map>
#include <unordered_map>

struct order_t {
  int64_t  timestamp;
  double_t order_id;
  int32_t  price;
  int32_t  qty;
};

typedef std::pair<int32_t, int64_t> price_timestamp;
class orderbook {
 public:
  void add_order( order_message_t& ord );
  void cancel_order( order_message_t& ord );
  void modify_order( order_message_t& ord );
  void process_transaction( trade_message_t& ord );

  int32_t get_top_bid();
  int32_t get_top_ask();
  int32_t get_total_orders();

  void print_orderbook();

 private:
  struct bidcomparator {
    bool operator()( const price_timestamp& a,
                     const price_timestamp& b ) const {
      if ( a.first != b.first ) {
        return a.first > b.first;
      }

      return a.second < b.second;
    }
  };

  struct askcomparator {
    bool operator()( const price_timestamp& a,
                     const price_timestamp& b ) const {
      if ( a.first != b.first ) {
        return a.first < b.first;
      }

      return a.second < b.second;
    }
  };
  std::map<price_timestamp, order_t, bidcomparator> bid_ob;
  std::map<price_timestamp, order_t, askcomparator> ask_ob;
  std::map<double_t, int32_t>                       order_price;

  std::unordered_map<double_t, order_t> active_bids;
  std::unordered_map<double_t, order_t> active_asks;
  // std::map<double_t, int32_t> order_price;
};

#endif