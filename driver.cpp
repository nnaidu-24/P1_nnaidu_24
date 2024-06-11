#include <cassert>
#include <fstream>
#include <iostream>

#include "nse_structs.h"
#include "orderbook.h"

int main()
{
  char *rawdata = "/opt/nse_uncompressed_split/fo/20240301/BANKNIFTY24MAYFUT";

  std::fstream fp( rawdata, std::ios::in );


  uint      messages_cnt = 0;
  header_t    readbuff;
  char      databuff[100];
  int64_t prev_ts=0;
  orderbook ob;


    while ( !fp.eof() ) {
    fp.read( ( char * )&readbuff, sizeof( readbuff ) );
    if ( fp.gcount() != header_size ) {
      break;
    }
    int payload_len = readbuff.msg_len - header_size;

    // fp.seekg( payload_len, std::ios_base::cur );
    fp.read( databuff, payload_len );
    order_message *msg = reinterpret_cast<order_message *> (databuff);
    int64_t curr_ts = msg->time_stamp;
    if (prev_ts != 0) {
        if (curr_ts < prev_ts)
        {
            std::cout << curr_ts << " " << prev_ts << "\n";
            break;
        }
        
    }

    messages_cnt += 1;

 

    if (msg->msg_type == multicast_tbt_message_types::NewOrder)
    {
     // std::cout << "ADD " << msg->order_type << " " << msg->price << "\n";
          ob.add_order(*msg);
    }else if (msg->msg_type == multicast_tbt_message_types::CancelOrder)
    {
    //  std::cout << "CANCEL " << msg->order_type << " " << msg->price << "\n";
      ob.cancel_order(*msg);
    }else if (msg->msg_type = multicast_tbt_message_types::ModificationOrder)
    {
       //std::cout << "MODIFY "  << msg->order_type << " " << msg->price << "\n";
      ob.modify_order(*msg);
    }else if (msg->msg_type = multicast_tbt_message_types::TradeMessage)
    {
      // std::cout << "TRADE " << msg->order_type << " " << msg->price << "\n";
      ob.process_transaction(*reinterpret_cast<trade_message*>(databuff));
    }
    
    if (ob.get_top_ask() <= ob.get_top_bid())
    {
            std::cout<< "timestamp:"<<curr_ts<<"\n";
     ob.print_orderbook();   
      prev_ts = curr_ts;
      std::cout<<"orders:"<<ob.get_total_orders()<<"\n";
    }

    assert(ob.get_top_ask() > ob.get_top_bid());
   

    
    if (curr_ts >= prev_ts + 10000000000 )
    {
      std::cout<< "timestamp:"<<curr_ts<<"\n";
     ob.print_orderbook();   
      prev_ts = curr_ts;
      std::cout<<"orders:"<<ob.get_total_orders()<<"\n";
    }
     
   

    }
}