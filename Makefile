
run_docker:
	docker run --rm -it -v $(shell pwd):/workdir crops/poky --workdir=/workdir

source_poky:
	cd poky
	source oe-init-build-env my_vpu

# load_layer: source_poky
# 	bitbake-layers add-layer $(shell pwd)/../../meta-vpu
