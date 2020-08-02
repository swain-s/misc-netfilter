//
//  packet.h
//  net_mastr
//
//  Created by kun shen on 2019/11/24.
//  Copyright © 2019 kun shen. All rights reserved.
//

#ifndef packet_h
#define packet_h

typedef unsigned char _uint8;
typedef unsigned short int _uint16;
typedef unsigned int _uint32;
typedef unsigned long int _uint64;
//***************** internet layer ***************************
//***************** internet layer ***************************


//***************** transport layer ***********************
typedef struct TCP_HEADER{ //20字节
    _uint16 s_port;
    _uint16 d_port;
    _uint32 sequence_num;
    _uint32 ack_num;
    _uint8 len_tcphdr_a_unused;  //4 bit + 6 bit
    _uint8 flags;               //6 bit
    _uint16 avi_recv_window;
    _uint16 checksum;
    _uint16 urgent_point;
    //optional and fil;
}tcphdr_t;

typedef struct UDP_HEADER{
    _uint16 s_port;    //16位源端口
    _uint16 dport;    //16位目的端口
    
}udpher_t;
//***************** transport layer **************************


//***************** internet layer ***************************
//***************** internet layer ***************************

//***************** internet layer ***************************
//***************** internet layer ***************************
#endif /* packet_h */
