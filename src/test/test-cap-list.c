/* SPDX-License-Identifier: LGPL-2.1+ */
/***
  This file is part of systemd.

  Copyright 2014 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

#include <sys/prctl.h>

#include "alloc-util.h"
#include "cap-list.h"
#include "capability-util.h"
#include "parse-util.h"
#include "string-util.h"
#include "util.h"

/* verify the capability parser */
static void test_cap_list(void) {
        int i;

        assert_se(!capability_to_name(-1));
        assert_se(!capability_to_name(capability_list_length()));

        for (i = 0; i < capability_list_length(); i++) {
                const char *n;

                assert_se(n = capability_to_name(i));
                assert_se(capability_from_name(n) == i);
                printf("%s = %i\n", n, i);
        }

        assert_se(capability_from_name("asdfbsd") == -EINVAL);
        assert_se(capability_from_name("CAP_AUDIT_READ") == CAP_AUDIT_READ);
        assert_se(capability_from_name("cap_audit_read") == CAP_AUDIT_READ);
        assert_se(capability_from_name("cAp_aUdIt_rEAd") == CAP_AUDIT_READ);
        assert_se(capability_from_name("0") == 0);
        assert_se(capability_from_name("15") == 15);
        assert_se(capability_from_name("-1") == -EINVAL);

        for (i = 0; i < capability_list_length(); i++) {
                _cleanup_cap_free_charp_ char *a = NULL;
                const char *b;
                unsigned u;

                assert_se(a = cap_to_name(i));

                /* quit the loop as soon as libcap starts returning
                 * numeric ids, formatted as strings */
                if (safe_atou(a, &u) >= 0)
                        break;

                assert_se(b = capability_to_name(i));

                printf("%s vs. %s\n", a, b);

                assert_se(strcasecmp(a, b) == 0);
        }
}

static void test_capability_set_to_string_alloc(void) {
        _cleanup_free_ char *t1 = NULL, *t2 = NULL, *t3 = NULL;

        assert_se(capability_set_to_string_alloc(0u, &t1) == 0);
        assert_se(streq(t1, ""));

        assert_se(capability_set_to_string_alloc(1u<<CAP_DAC_OVERRIDE, &t2) == 0);
        assert_se(streq(t2, "cap_dac_override"));

        assert_se(capability_set_to_string_alloc(UINT64_C(1)<<CAP_CHOWN | UINT64_C(1)<<CAP_DAC_OVERRIDE | UINT64_C(1)<<CAP_DAC_READ_SEARCH | UINT64_C(1)<<CAP_FOWNER | UINT64_C(1)<<CAP_SETGID | UINT64_C(1)<<CAP_SETUID | UINT64_C(1)<<CAP_SYS_PTRACE | UINT64_C(1)<<CAP_SYS_ADMIN | UINT64_C(1)<<CAP_AUDIT_CONTROL | UINT64_C(1)<<CAP_MAC_OVERRIDE | UINT64_C(1)<<CAP_SYSLOG, &t3) == 0);
        assert_se(streq(t3, "cap_chown cap_dac_override cap_dac_read_search cap_fowner cap_setgid cap_setuid cap_sys_ptrace cap_sys_admin cap_audit_control cap_mac_override cap_syslog"));
}

int main(int argc, char *argv[]) {
        test_cap_list();
        test_capability_set_to_string_alloc();

        return 0;
}
