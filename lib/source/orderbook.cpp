#include <cassert>
#include "nse_structs.h"
#include "orderbook.h"
#include <iostream>

void orderbook::add_order( order_message_t &ord ) {
  order_t temp;
  temp.price     = ord.m_price__;
  temp.timestamp = ord.m_time_stamp__;
  temp.order_id  = ord.m_order_id__;
  temp.qty       = ord.m_qty__;

  if ( ord.m_order_type__ == multicast_tbt_order_types_t::Buy ) {
    if ( temp.price >= get_top_ask() ) {
      active_bids.insert( { temp.order_id, temp } );
    } else {
      bid_ob.insert( { { temp.price, temp.timestamp }, temp } );
      order_price[temp.order_id] = temp.price;
    }

  } else if ( ord.m_order_type__ == multicast_tbt_order_types_t::Sell ) {
    if ( temp.price <= get_top_bid() ) {
      active_asks.insert( { temp.order_id, temp } );
    } else {
      ask_ob.insert( { { temp.price, temp.timestamp }, temp } );
      order_price[temp.order_id] = temp.price;
    }
  }
};

void orderbook::cancel_order( order_message_t &ord ) {
  auto it = order_price.find( ord.m_order_id__ );

  if ( it == order_price.end() ) return;

  int32_t price = it->second;

  order_price.erase( it );

  if ( ord.m_order_type__ == multicast_tbt_order_types_t::Buy ) {
    auto price_level = bid_ob.lower_bound( { price, 0LL } );

    while ( price_level != bid_ob.end() ) {
      int32_t  curr_price = ( price_level->first ).first;
      double_t curr_id    = ( price_level->second ).order_id;
      assert( curr_price == price );

      if ( ord.m_order_id__ == curr_id ) {
        bid_ob.erase( price_level );
        break;
      } else {
        price_level++;
      }
    }
  } else if ( ord.m_order_type__ == multicast_tbt_order_types_t::Sell ) {
    auto price_level = ask_ob.lower_bound( { price, 0LL } );

    while ( price_level != ask_ob.end() ) {
      int32_t  curr_price = ( price_level->first ).first;
      double_t curr_id    = ( price_level->second ).order_id;
      assert( curr_price == price );

      if ( ord.m_order_id__ == curr_id ) {
        ask_ob.erase( price_level );
        break;
      } else {
        price_level++;
      }
    }
  }
}

void orderbook::modify_order( order_message_t &ord ) {
  order_t temp;
  temp.price     = ord.m_price__;
  temp.timestamp = ord.m_time_stamp__;
  temp.order_id  = ord.m_order_id__;
  temp.qty       = ord.m_qty__;

  auto it = order_price.find( ord.m_order_id__ );

  if ( it == order_price.end() ) {
    add_order( ord );
    return;
  }

  int32_t price = it->second;

  order_price.erase( it );

  if ( ord.m_order_type__ == multicast_tbt_order_types_t::Buy ) {
    auto price_level = bid_ob.lower_bound( { price, 0LL } );

    while ( price_level != bid_ob.end() ) {
      int32_t  curr_price = ( price_level->first ).first;
      double_t curr_id    = ( price_level->second ).order_id;
      assert( curr_price == price );

      if ( ord.m_order_id__ == curr_id ) {
        bid_ob.erase( price_level );
        break;
      } else {
        price_level++;
      }
    }
    add_order( ord );
    // bid_ob.insert( { { temp.price, temp.timestamp }, temp } );
    // order_price[temp.order_id] = temp.price;

  } else if ( ord.m_order_type__ == multicast_tbt_order_types_t::Sell ) {
    auto price_level = ask_ob.lower_bound( { price, 0LL } );

    while ( price_level != ask_ob.end() ) {
      int32_t  curr_price = ( price_level->first ).first;
      double_t curr_id    = ( price_level->second ).order_id;
      assert( curr_price == price );

      if ( ord.m_order_id__ == curr_id ) {
        ask_ob.erase( price_level );
        break;
      } else {
        price_level++;
      }
    }
    add_order( ord );
    // ask_ob.insert( { { temp.price, temp.timestamp }, temp } );
    // order_price[temp.order_id] = temp.price;
  }
}

void orderbook::process_transaction( trade_message_t &trd ) {
  double_t bid_id = trd.m_buy_order_id__;
  double_t ask_id = trd.m_sell_order_id__;
  // Checking if the bid id is valid
  if ( bid_id != 0 ) {
    auto it             = order_price.find( bid_id );
    auto active_bids_it = active_bids.find( bid_id );
    // checking if the bid_id is present in orderbook
    if ( it != order_price.end() ) {
      int32_t price = it->second;

      auto price_level = bid_ob.lower_bound( { price, 0LL } );

      while ( price_level != bid_ob.end() ) {
        int32_t  curr_price = ( price_level->first ).first;
        double_t curr_id    = ( price_level->second ).order_id;
        assert( curr_price == price );

        if ( bid_id == curr_id ) {
          price_level->second.order_id -= trd.m_trade_qty__;
          assert( price_level->second.order_id >= 0 );
          if ( ( price_level->second ).order_id == 0 ) {
            bid_ob.erase( price_level );
            order_price.erase( it );
          }
          break;
        } else {
          price_level++;
        }
      }
    }  // Checking if the bid id is an active order
    else if ( active_bids_it != active_bids.end() ) {
      auto it = active_bids_it;

      ( it->second ).qty -= trd.m_trade_qty__;
      assert( ( it->second ).qty >= 0 );

      if ( it->second.qty == 0 ) {
        active_bids.erase( it );
      } else if ( ( it->second ).price < get_top_ask() ) {
        order_t ord = it->second;
        bid_ob.insert( { { ord.price, ord.timestamp }, ord } );
        order_price[ord.order_id] = ord.price;
      }
    }
  }
  // checking if the ask id is valid
  if ( ask_id != 0 ) {
    auto it             = order_price.find( ask_id );
    auto active_asks_it = active_asks.find( ask_id );
    // Checking if the ask_id is present in orderbook
    if ( it != order_price.end() ) {
      int32_t price = it->second;

      auto price_level = ask_ob.lower_bound( { price, 0LL } );

      while ( price_level != ask_ob.end() ) {
        int32_t  curr_price = ( price_level->first ).first;
        double_t curr_id    = ( price_level->second ).order_id;
        assert( curr_price == price );

        if ( ask_id == curr_id ) {
          ( price_level->second ).order_id -= trd.m_trade_qty__;
          assert( ( price_level->second ).order_id >= 0 );
          if ( ( price_level->second ).order_id == 0 ) {
            ask_ob.erase( price_level );
            order_price.erase( it );
          }
          break;
        } else {
          price_level++;
        }
      }
    }  // Checking if the ask_id is an active order
    else if ( active_asks_it != active_asks.end() ) {
      auto it = active_asks_it;

      ( it->second ).qty -= trd.m_trade_qty__;
      assert( ( it->second ).qty >= 0 );

      if ( ( it->second ).qty == 0 ) {
        active_asks.erase( it );
      } else if ( ( it->second ).price > get_top_bid() ) {
        order_t ord = ( it->second );
        ask_ob.insert( { { ord.price, ord.timestamp }, ord } );
        order_price[ord.order_id] = ord.price;
      }
    }
  }
}

void orderbook::print_orderbook() {
  int  k   = 8;
  auto it1 = bid_ob.begin();

  std::cout << "bid_order_book:{";
  while ( it1 != bid_ob.end() && k-- ) {
    auto key = it1->first;

    std::cout << key.first << ":" << it1->second.qty;
    if ( k != 0 ) {
      std::cout << ",";
    }
    it1++;
  }

  std::cout << "}\n";

  k        = 8;
  auto it2 = ask_ob.begin();
  std::cout << "ask_order_book:{";
  while ( it2 != ask_ob.end() && k-- ) {
    auto key = it2->first;

    std::cout << key.first << ":" << it1->second.qty;
    if ( k != 0 ) {
      std::cout << ",";
    }
    it2++;
  }

  std::cout << "}\n";
}

int32_t orderbook::get_top_bid() {
  int32_t bid = INT32_MIN;
  auto    it  = bid_ob.begin();

  while ( it != bid_ob.end() ) {
    double_t curr_id = ( it->second ).order_id;

    bid = ( it->first ).first;
    break;

    it++;
  }

  return bid;
}

int32_t orderbook::get_top_ask() {
  int32_t bid = INT32_MAX;
  auto    it  = ask_ob.begin();

  while ( it != ask_ob.end() ) {
    double_t curr_id = ( it->second ).order_id;

    bid = ( it->first ).first;
    break;

    it++;
  }

  return bid;
}

int32_t orderbook::get_total_orders() { return bid_ob.size() + ask_ob.size(); }