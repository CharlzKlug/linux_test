/*
 * af_llc.c Unit Tests
 * Author: Ametov Imil <ametov.ii@phystech.edu>
 * Note: add line
 * #include "af_llc_test.h"
 * into af_llc.c
 */

#ifdef CONFIG_AF_LLC_KUNIT_TEST

#include <kunit/test.h>

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
  return 0 ;
 out:

  // release_lock begins here 
  spin_lock_bh(&sk->sk_lock.slock) ;
  if (sk->sk_backlog.tail) {__release_sock(sk);}
  if (sk->sk_prot->release_cb) {sk->sk_prot->release_cb(sk);}
  sock_release_ownership(sk);
  if (waitqueue_active(&sk->sk_lock.wq))
    { wake_up(&sk->sk_lock.wq); }
  spin_unlock_bh(&sk->sk_lock.slock);
    // release_lock ends here 
  
  return (copied) ;
}
*/

static void sample_test(struct kunit *test)
{
  //  KUNIT_EXPECT_EQ(test, llc_ui_sap_last_autoport, 192);
  //  KUNIT_EXPECT_TRUE(test, unlikely(1 == 0));
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

  /* dumb function */
  prto.release_cb = &my_release_cb ;
  
  sk.sk_type = SOCK_STREAM ;
  sk.sk_state = TCP_LISTEN ;
  sk.__sk_common.skc_prot=&prto;
  sk.sk_lock.wq.head.next = &sk.sk_lock.wq.head ;
  
  //  struct socket sock ;
  sock.sk = &sk ;
  //KUNIT_EXPECT_TRUE(test, (check_socket(&sock))) ;
  //KUNIT_EXPECT_EQ(test, llc_ui_recvmsg(
  len = 100 ;
  flags = 0 ;
  msg.msg_name = &origin ;
  //llc_ui_recvmsg(&sock, &msg, len, flags) ;
  // KUNIT_EXPECT_EQ(test, test_recvmsg(&sock, &msg, len, flags), -ENOTCONN) ;
  KUNIT_EXPECT_EQ(test, llc_ui_recvmsg(&sock, &msg, len, flags), -ENOTCONN) ;
  //KUNIT_EXPECT_FALSE(test, waitqueue_active(&sk.sk_lock.wq)) ;
  //KUNIT_EXPECT_TRUE(test, list_empty(&mylisthead)) ;
  //KUNIT_EXPECT_FALSE(test, !list_empty(&mylisthead)) ;
}

static struct kunit_case af_llc_test_cases[] = {
  KUNIT_CASE(sample_test),
  {},
};

static struct kunit_suite af_llc_test_suite = {
  .name = "af_llc.c",
  .test_cases = af_llc_test_cases,
};

kunit_test_suite(af_llc_test_suite);

#endif /* CONFIG_AF_LLC_KUNIT_TEST */
