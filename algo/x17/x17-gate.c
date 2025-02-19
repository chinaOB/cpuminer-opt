#include "x17-gate.h"

bool register_x17_algo( algo_gate_t* gate )
{
#if defined (X17_16X32)
  gate->scanhash  = (void*)&scanhash_x17_16way;
//  gate->hash      = (void*)&x17_16way_hash;
#elif defined (X17_8WAY)
  gate->scanhash  = (void*)&scanhash_x17_8way;
  gate->hash      = (void*)&x17_8way_hash;
#elif defined (X17_4WAY)
  gate->scanhash  = (void*)&scanhash_x17_4way;
//  gate->scanhash  = (void*)&scanhash_4way_64in_32out;
  gate->hash      = (void*)&x17_4way_hash;
#else
  gate->hash      = (void*)&x17_hash;
#endif
  gate->optimizations = SSE2_OPT | AES_OPT | AVX2_OPT | AVX512_OPT | VAES_OPT;
  return true;
};

