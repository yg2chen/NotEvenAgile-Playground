FILESEXTRAPATHS:prepend := "${THISDIR}/files:"
SRC_URI += "file://edu.patch"
ERROR_QA:remove = "patch-fuzz"

