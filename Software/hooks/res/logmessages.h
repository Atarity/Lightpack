//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//
#define PRISMATIK_LOG_SEVERITY_DEBUG     0x0
#define PRISMATIK_LOG_SEVERITY_INFO      0x1
#define PRISMATIK_LOG_SEVERITY_WARNING   0x2
#define PRISMATIK_LOG_SEVERITY_ERROR     0x3


//
// MessageId: PRISMATIK_LOG_MSG_DEBUG
//
// MessageText:
//
// %1
//
#define PRISMATIK_LOG_MSG_DEBUG          ((DWORD)0x00000100L)

//
// MessageId: PRISMATIK_LOG_MSG_INFO
//
// MessageText:
//
// %1
//
#define PRISMATIK_LOG_MSG_INFO           ((DWORD)0x40000101L)

//
// MessageId: PRISMATIK_LOG_MSG_WARNING
//
// MessageText:
//
// %1
//
#define PRISMATIK_LOG_MSG_WARNING        ((DWORD)0x80000102L)

//
// MessageId: PRISMATIK_LOG_MSG_ERROR
//
// MessageText:
//
// %1
//
#define PRISMATIK_LOG_MSG_ERROR          ((DWORD)0xC0000103L)

