
#define EXTUNIX_WANT_SOCKOPT
#include "config.h"

#if defined(EXTUNIX_HAVE_SOCKOPT)

static int tcp_options[] = { 
  TCP_KEEPCNT, TCP_KEEPIDLE, TCP_KEEPINTVL,
};

CAMLprim value caml_extunix_setsockopt_int(value fd, value k, value v)
{
  int optval = Int_val(v);
  socklen_t optlen = sizeof(optval);

  if (Int_val(k) < 0 || (unsigned int)Int_val(k) >= sizeof(tcp_options) / sizeof(tcp_options[0]))
    caml_invalid_argument("setsockopt_int");

  if (0 != setsockopt(Int_val(fd), IPPROTO_TCP, tcp_options[Int_val(k)], &optval, optlen))
  {
    uerror("setsockopt_int", Nothing);
  }

  return Val_unit;
}

CAMLprim value caml_extunix_getsockopt_int(value fd, value k)
{
  int optval;
  socklen_t optlen = sizeof(optval);

  if (Int_val(k) < 0 || (unsigned int)Int_val(k) >= sizeof(tcp_options) / sizeof(tcp_options[0]))
    caml_invalid_argument("getsockopt_int");

  if (0 != getsockopt(Int_val(fd), IPPROTO_TCP, tcp_options[Int_val(k)], &optval, &optlen))
  {
    uerror("getsockopt_int", Nothing);
  }

  return Val_int(optval);
}

#endif
