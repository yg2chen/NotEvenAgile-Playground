SUMMARY = "Out-Of-Tree VPU kernel module"
DESCRIPTION = "${SUMMARY}"

LICENSE = "CLOSED"

inherit module
SRC_URI = "file://Makefile \
           file://edu_driver.c \
           file://defines.h "

S = "${WORKDIR}"

RPROVIDES:${PN} += "kernel-module-edu_driver"
