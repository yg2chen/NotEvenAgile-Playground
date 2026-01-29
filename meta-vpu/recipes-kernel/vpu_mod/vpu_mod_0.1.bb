SUMMARY = "Out-Of-Tree VPU kernel module"
DESCRIPTION = "${SUMMARY}"

LICENSE = "CLOSED"

inherit module
SRC_URI = "file://Makefile \
           file://vpu_driver.c "

S = "${WORKDIR}"

RPROVIDES:${PN} += "kernel-module-vpu_driver"
