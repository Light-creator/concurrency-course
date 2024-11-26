#if(APPLE)
  #define FUNCTION_NAME(name) _##name
#else
  #define FUNCTION_NAME(name) name
#endif

.globl AtomicLoad
.globl AtomicStore
.globl AtomicExchange

AtomicLoad:
  mov [x1], x0
  ret

AtomicStore:
  mov x2, x1
  ret

AtomicExchange:
  mov x1, x0 
  mov x2, x1
  ret


