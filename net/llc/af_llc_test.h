/*
 * af_llc.c Unit Tests
 * Author: Ametov Imil <ametov.ii@phystech.edu>
 * Note: add line
 * #include "af_llc_test.h"
 * into af_llc.c
 */

#ifdef CONFIG_AF_LLC_KUNIT_TEST

#include <kunit/test.h>

#define TEST_SK_ERR(x) \
  do {struct sock sk ; \
  struct socket sock ; \
  struct proto prto ; \
  struct msghdr msg ; \
  size_t len ; \
  int flags ; \
  int err ; \
  unsigned char origin[128] = "test" ; \
  struct wait_queue_head wqh1 ; \
  struct list_head mylisthead ; \
  mylisthead.next = &mylisthead ; \
  wqh1.head.next = &wqh1.head ; \
  prto.release_cb = &my_release_cb ; \
  sk.sk_type = SOCK_RAW ; \
  sk.sk_state = TCP_LISTEN ; \
  sk.__sk_common.skc_prot=&prto; \
  sk.sk_lock.wq.head.next = &sk.sk_lock.wq.head ;	\
  sk.sk_err = (x) ;					\
  err = - sk.sk_err ;					\
  sock.sk = &sk ;					\
  len = 100 ;						\
  flags = MSG_DONTWAIT ;				\
  msg.msg_name = &origin ;					      \
  KUNIT_EXPECT_EQ(test, llc_ui_recvmsg(&sock, &msg, len, flags), err) ; \
  } while (0)

int wqa (struct wait_queue_head *wq_head)
{
  return (1) ;
}
void my_release_cb (struct sock *sk)
{
  return ;
}

int check_socket (struct socket *sock)
{
  struct sock *sk = sock->sk ;
  return (sk->sk_type == SOCK_STREAM) ;
}

/*
static int test_recvmsg(struct socket *sock, struct msghdr *msg, size_t len,
			int flags)
{
  DECLARE_SOCKADDR(struct sockaddr_llc *, uaddr, msg->msg_name) ;
  const int nonblock = flags & MSG_DONTWAIT;
  struct sk_buff *skb = NULL;

  struct sock *sk = sock->sk;

  // здесь работает приведение к структуре llc_sock
  struct llc_sock *llc = llc_sk(sk);

  size_t copied = 0;
  u32 peek_seq = 0;
  u32 *seq, skb_len;
  unsigned long used;
  int target;	
  long timeo;

  lock_sock(sk);
  copied = -ENOTCONN;
  if (unlikely(sk->sk_type == SOCK_STREAM && sk->sk_state == TCP_LISTEN))
    goto out ;

  //если nonblock является истинным, то возвращает 0,
  //если nonblock является ложным, то возвращает sk->sk_sndtimeo
  //тип sk_sndtimeo есть long, %SO_SNDTIMEO setting
  // надо выяснить когда истинен nonblock
  timeo = sock_rcvtimeo(sk, nonblock);    

  // здесь seq ссылается на значение llc->copied_seq
  seq = &llc->copied_seq;

  if (flags & MSG_PEEK) {
    peek_seq = llc->copied_seq;
    seq = &peek_seq;
  }

  // если flags & MSG_WAITALL истинен, то возвращает len,
  // иначе min_t(int, READ_ONCE(sk->sk_rcvlowat), len)
  // если же v окажется равен нулю, то вернёт 1.
  // для flags = MSG_DONTWAIT target равен 1.
  target = sock_rcvlowat(sk, flags & MSG_WAITALL, len);

  copied = 0 ;
  do {
    u32 offset ;


    // для данного случая signal_pending(current) = 0
    if (signal_pending(current)) {
      if (copied)
	break;
      copied = timeo ? sock_intr_errno(timeo) : -EAGAIN;
      break;
    }

    skb = skb_peek(&sk->sk_receive_queue) ;
    if (skb) {
      offset = *seq;
      copied = 77 ;
      //goto found_ok_skb;
    }

    if (copied >= target && !READ_ONCE(sk->sk_backlog.tail))
      break;

    if (copied) {
      if (sk->sk_err ||
	  sk->sk_state == TCP_CLOSE ||
	  (sk->sk_shutdown & RCV_SHUTDOWN) ||
	  !timeo ||
	  (flags & MSG_PEEK))
	break;
    } else {
      if (sock_flag(sk, SOCK_DONE))
	break;

      if (sk->sk_err) {
	copied = sock_error(sk);
	break;
      }

    }
    
    len -= 1 ;
  } while (len > 0) ;
 out:
  // release_lock begins here
  release_sock(sk);
  // release_lock ends here 
       return (copied) ;
  //return timeo ;
}
*/

static void llc_ui_recvmsg_enotconn_test(struct kunit *test)
{
  struct sock sk ;
  struct socket sock ;
  struct proto prto ;
  struct msghdr msg ;
  size_t len ;
  int flags ;
  unsigned char origin[128] = "test" ;
  struct wait_queue_head wqh1 ;
  struct list_head mylisthead ;
  mylisthead.next = &mylisthead ;
  wqh1.head.next = &wqh1.head ;

  // dumb function
  prto.release_cb = &my_release_cb ;
  
  sk.sk_type = SOCK_STREAM ;
  sk.sk_state = TCP_LISTEN ;
  sk.__sk_common.skc_prot=&prto;
  sk.sk_lock.wq.head.next = &sk.sk_lock.wq.head ;
  
  sock.sk = &sk ;
  len = 100 ;
  flags = 0 ;
  msg.msg_name = &origin ;

  KUNIT_EXPECT_EQ(test, llc_ui_recvmsg(&sock, &msg, len, flags), -ENOTCONN) ;
}

static void llc_ui_recvmsg_sock_done_test(struct kunit *test)
{
  struct sock sk ;
  struct socket sock ;
  struct proto prto ;
  struct msghdr msg ;
  size_t len ;
  int flags ;
  unsigned char origin[128] = "test" ;
  struct wait_queue_head wqh1 ;
  struct list_head mylisthead ;
  mylisthead.next = &mylisthead ;
  wqh1.head.next = &wqh1.head ;

  // dumb function
  prto.release_cb = &my_release_cb ;
  
  sk.sk_type = SOCK_RAW ;
  sk.sk_state = TCP_LISTEN ;
  sk.__sk_common.skc_prot=&prto;
  sk.sk_lock.wq.head.next = &sk.sk_lock.wq.head ;

  sock_set_flag (&sk, SOCK_DONE) ;
  
  sock.sk = &sk ;
  len = 100 ;
  flags = MSG_DONTWAIT ;
  msg.msg_name = &origin ;

  KUNIT_EXPECT_EQ(test, llc_ui_recvmsg(&sock, &msg, len, flags), 0) ;
}

static void llc_ui_recvmsg_sk_err_test(struct kunit *test)
{
  TEST_SK_ERR(1) ;
  TEST_SK_ERR(2) ;
  TEST_SK_ERR(3) ;
  TEST_SK_ERR(4) ;
  TEST_SK_ERR(10) ;
  TEST_SK_ERR(20) ;
}

static struct kunit_case af_llc_test_cases[] = {
  KUNIT_CASE(llc_ui_recvmsg_enotconn_test),
  KUNIT_CASE(llc_ui_recvmsg_sock_done_test),
  KUNIT_CASE(llc_ui_recvmsg_sk_err_test),
  {},
};

static struct kunit_suite af_llc_test_suite = {
  .name = "af_llc.c",
  .test_cases = af_llc_test_cases,
};

kunit_test_suite(af_llc_test_suite);

#endif /* CONFIG_AF_LLC_KUNIT_TEST */
