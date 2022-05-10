# linux_test
Linux test llc_ui_recvmsg.

add line

#include "af_llc_test.h"

into af_llc.c file.

To run test type "./tools/testing/kunit/kunit.py run --jobs=2 --kunitconfig=.kunitconfig" in the source root.