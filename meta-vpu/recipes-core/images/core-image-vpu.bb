require recipes-core/images/core-image-minimal.bb

SUMMARY = "My Custom VPU image"

# IMAGE_FEATURES += "package-management"
IMAGE_INSTALL:append = " openssh openssh-sftp-server pciutils"

# kernel test module
IMAGE_INSTALL:append = " user_test"
