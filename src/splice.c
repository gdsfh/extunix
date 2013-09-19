
#define EXTUNIX_WANT_VMSPLICE
#define EXTUNIX_WANT_SPLICE
#define EXTUNIX_WANT_TEE
#include "config.h"

#if defined(EXTUNIX_HAVE_SPLICE) | defined(EXTUNIX_HAVE_TEE) | defined(EXTUNIX_HAVE_VMSPLICE)
static int splice_flags[] =
  {
    SPLICE_F_MOVE,
    SPLICE_F_NONBLOCK,
    SPLICE_F_MORE,
    SPLICE_F_GIFT
  };
#endif

#if defined(EXTUNIX_HAVE_SPLICE)
static loff_t* get_offset(value v_off)
{
  return (Is_long(v_off) ? NULL : &(Field(v_off, 0)));
}

CAMLprim value caml_extunix_splice(value v_fd_in, value v_off_in, value v_fd_out, value v_off_out, value v_len, value v_flags)
{
  CAMLparam5(v_fd_in, v_off_in, v_fd_out, v_off_out, v_len);
  CAMLxparam1(v_flags);

  unsigned int flags = caml_convert_flag_list(v_flags, splice_flags);
  int fd_in = Int_val(v_fd_in);
  int fd_out = Int_val(v_fd_out);
  loff_t* off_in = get_offset(v_off_in);
  loff_t* off_out = get_offset(v_off_out);
  size_t len = Int_val(v_len);
  ssize_t ret;

  caml_enter_blocking_section();
  ret = splice(fd_in, off_in, fd_out, off_out, len, flags);
  caml_leave_blocking_section();

  if (ret == -1)
    uerror("splice", Nothing);

  CAMLreturn(Val_long(ret));
}

CAMLprim value caml_extunix_splice_bytecode(value * argv, int argn)
{
  (void)argn;
  return caml_extunix_splice(argv[0], argv[1], argv[2],
                             argv[3], argv[4], argv[5]);
}

#endif

#if defined(EXTUNIX_HAVE_TEE)

CAMLprim value caml_extunix_tee(value v_fd_in, value v_fd_out, value v_len, value v_flags)
{
  CAMLparam4(v_fd_in, v_fd_out, v_len, v_flags);

  unsigned int flags = caml_convert_flag_list(v_flags, splice_flags);
  int fd_in = Int_val(v_fd_in);
  int fd_out = Int_val(v_fd_out);
  size_t len = Int_val(v_len);
  ssize_t ret;

  caml_enter_blocking_section();
  ret = tee(fd_in, fd_out, len, flags);
  caml_leave_blocking_section();

  if (ret == -1)
    uerror("tee", Nothing);

  CAMLreturn(Val_long(ret));
}

#endif


#if defined(EXTUNIX_HAVE_VMSPLICE)

CAMLprim value caml_extunixba_vmsplice(value v_fd_out, value v_iov, value v_flags)
{
  CAMLparam3(v_fd_out, v_iov, v_flags);

  unsigned int flags = caml_convert_flag_list(v_flags, splice_flags);
  int fd_out = Int_val(v_fd_out);
  int size = Wosize_val(v_iov);
  struct iovec* iov = alloca(sizeof(struct iovec) * size);
  ssize_t ret;
  int i;
  value tmp;
  struct caml_ba_array* ba;
  int offset, length;

  for (i = 0; i < size; i++)
  {
    tmp = Field(v_iov,i);
    /* field 0 is a 'a carray8 (bigarray of 8-bit elements)
       field 1 is the offset in the bigarray
       field 2 is the length */
    ba = Caml_ba_array_val(Field(tmp,0));
    offset = Int_val(Field(tmp,1));
    length = Int_val(Field(tmp,2));
    if(offset + length > ba->dim[0])
      caml_invalid_argument("vmsplice");
    iov[i].iov_base = (char *) ba->data + offset;
    iov[i].iov_len = length;
  }

  caml_enter_blocking_section();
  ret = vmsplice(fd_out, iov, size, flags);
  caml_leave_blocking_section();

  if (ret == -1)
    uerror("vmsplice", Nothing);

  CAMLreturn(Val_long(ret));
}

#endif
