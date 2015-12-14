#ifndef __INCLUDE_FENCING_CONSTANTS_H__
#define __INCLUDE_FENCING_CONSTANTS_H__

#define EPEE_DEPRESS_US 2000 // (2ms - 10ms)
#define EPEE_LOCKOUT_US 45000 // (45ms +/- 5ms)

#define FOIL_DEPRESS_US 14000 // (14ms +/- 1ms)
#define FOIL_LOCKOUT_US 300000 // (300ms +/- 25ms)

#define SABER_DEPRESS_US 100 // (0.1ms - 1ms)
#define SABER_LOCKOUT_US 120000 // (120ms +/- 10ms)

#define WHIPOVER_LOCKOUT_US 4000 // after 4 ms, touches disallowed
#define WHIPOVER_TIMEOUT_US 15000 // after 15 ms, whipover protection ends
#define MAX_WHIPOVER_INTERRUPT_COUNT 10 // after 10 interrupts in blade contact, whipover protection ends

#endif // #ifndef __INCLUDE_FENCING_CONSTANTS_H__



