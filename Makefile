OBJ_DIR := $(PWD)
KERNELDIR := /lib/modules/${KERNELVER}/build

include $(KERNELDIR)/.config

obj-m += mtgpu.o

mtgpu-objs += src/common/os-interface.o \
	src/common/os-interface-drm.o \
	src/mtgpu/mtgpu_drv.o \
	src/mtgpu/mtgpu_module_param.o \
	src/mtgpu/drm_compatibility.o \
	src/mtgpu/mtgpu_drm_dp.o \
	src/mtgpu/mtgpu_drm_dispc.o \
	src/mtgpu/mtgpu_drm_drv.o \
	src/mtgpu/mtgpu_drm_dummy_connector.o \
	src/mtgpu/mtgpu_drm_dummy_crtc.o \
	src/mtgpu/mtgpu_drm_gem.o \
	src/mtgpu/mtgpu_drm_hdmi.o \
	src/mtgpu/phy-mthreads-dp.o \
	src/mtgpu/p2pdma.o \
	src/mtsnd/eld.o \
	src/mtsnd/mtsnd_codec.o \
	src/mtsnd/mtsnd_drv.o \
	src/mtsnd/mtsnd_pcm.o \
	src/mtvpu/mtvpu_api.o \
	src/mtvpu/mtvpu_drv.o \
	src/mtvpu/mtvpu_gem.o \
	src/pvr/allocmem.o \
	src/pvr/error_mapping.o \
	src/pvr/event.o \
	src/pvr/fwload.o \
	src/pvr/handle_idr.o \
	src/pvr/interrupt_support.o \
	src/pvr/km_apphint.o \
	src/pvr/module_common.o \
	src/pvr/osconnection_server.o \
	src/pvr/osfunc.o \
	src/pvr/osmmap_stub.o \
	src/pvr/pci_support.o \
	src/pvr/physmem_dmabuf.o \
	src/pvr/physmem_osmem_linux.o \
	src/pvr/physmem_test.o \
	src/pvr/pmr_os.o \
	src/pvr/pvr_bridge_k.o \
	src/pvr/pvr_buffer_sync.o \
	src/pvr/pvr_counting_timeline.o \
	src/pvr/pvr_debug.o \
	src/pvr/pvr_debugfs.o \
	src/pvr/pvr_drm.o \
	src/pvr/pvr_fence.o \
	src/pvr/pvr_gputrace.o \
	src/pvr/pvr_platform_drv.o \
	src/pvr/pvr_sw_fence.o \
	src/pvr/pvr_sync_file.o \
	src/pvr/pvr_sync_ioctl_common.o \
	src/pvr/pvr_sync_ioctl_drm.o \
	src/pvr/trace_events.o \
	objs/$(ARCH)/mtgpu_core.o

mtgpu-$(CONFIG_RISCV) += src/pvr/osfunc_riscv.o
mtgpu-$(CONFIG_ARM) += src/pvr/osfunc_arm.o
mtgpu-$(CONFIG_ARM64) += src/pvr/osfunc_arm64.o
mtgpu-$(CONFIG_LOONGARCH) += src/pvr/osfunc_loongarch.o \
	src/mtgpu/dmi_loongarch.o
mtgpu-$(CONFIG_X86) += src/pvr/osfunc_x86.o

ccflags-y := -D__linux__ -include config_kernel.h
ccflags-y += -include linux/version.h
ccflags-$(CONFIG_X86) += -include $(KERNELDIR)/arch/x86/include/asm/cpufeatures.h
ccflags-y += -include conftest.h
ccflags-y += -I$(OBJ_DIR)/inc \
	-I$(OBJ_DIR)/inc/pvr \
	-I$(OBJ_DIR)/inc/pvr/generated \
	-I$(OBJ_DIR)/inc/pvr/hwdefs \
	-I$(OBJ_DIR)/inc/pvr/hwdefs/km \
	-I$(OBJ_DIR)/inc/pvr/include \
	-I$(OBJ_DIR)/inc/pvr/include/powervr \
	-I$(OBJ_DIR)/inc/pvr/services \
	-I$(OBJ_DIR)/inc/common \
	-I$(OBJ_DIR)/inc/mtgpu \
	-I$(OBJ_DIR)/inc/mtgpu/ion \
	-I$(OBJ_DIR)/inc/mtvpu \
	-I$(OBJ_DIR)/inc/mtvpu/linux \
	-I$(OBJ_DIR)/inc/mtsnd \
	-I$(OBJ_DIR)/inc/imgtec

CONFTEST_H := $(OBJ_DIR)/inc/conftest.h
CONFTEST := $(OBJ_DIR)/conftest

MTGPU_BINARY := $(OBJ_DIR)/objs/$(ARCH)/mtgpu_core.o_binary
MTGPU_BINARY_O := $(OBJ_DIR)/objs/$(ARCH)/mtgpu_core.o
quiet_cmd_symlink = SYMLINK $@
cmd_symlink = ln -sf $< $@

all: $(MTGPU_BINARY_O) $(CONFTEST_H)
	make  -C $(KERNELDIR)   M=$(OBJ_DIR) modules
	@echo "make all end"

$(MTGPU_BINARY_O): $(MTGPU_BINARY)
	$(call if_changed,symlink)

$(CONFTEST_H): $(CONFTEST)
	@$< -a $(ARCH) -k $(KERNELVER) -d $(KERNELDIR)
	@ln -sf $(OBJ_DIR)/conftest.h $@

clean:
	make  -C $(KERNELDIR)   M=$(OBJ_DIR) clean
	@echo "make clean end"
