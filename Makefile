run_docker:
	docker run --rm -it -v "$(shell pwd):/workdir" crops/poky --workdir=/workdir

test_local_qemu:


