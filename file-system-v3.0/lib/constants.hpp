#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

const int NAME_REQ = 0;
const int LS_REQ = 1; // Client wants to list files
const int GET_REQ = 2; // Client wants to get a file from server
const int SEND_REQ = 3; // Client wants to send a file to server
const int RM_REQ = 4; // Client wants to remove a file from server
const int INIT_SERVER_REQ = 5;
const int UPDATE_SERVER_REQ = 6;
const int CHUNK_SIZE = 524288; // 512 KiB
const int MAX_NUM_SERVERS = 256;

#endif
