SUMMARY = "Testing Out-Of-Tree VPU kernel module"
DESCRIPTION = "${SUMMARY}"
LICENSE = "CLOSED"

SRC_URI = "file://user_test.c"
S = "${WORKDIR}"

do_compile() {
    ${CC} ${LDFLAGS} -o user_test user_test.c 
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 user_test ${D}${bindir}
}

RPROVIDES:${PN} += "user_test"
