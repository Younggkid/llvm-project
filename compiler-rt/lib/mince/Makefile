# changed by lcy on 10.31
# SGX_PF_DIR ?= ${CURDIR}/../../../../../../
# SGX_SDK_DIR = ${SGX_PF_DIR}/linux-sgx
# COMMON_DIR  = ${SGX_SDK_DIR}/common

CC=gcc
CXX=g++

#ENCLAVE_CFLAGS   = -ffreestanding -nostdinc -fvisibility=hidden -fpie -DSGX
#ENCLAVE_CXXFLAGS = $(ENCLAVE_CFLAGS) -nostdinc++

# CXXFLAGS += $(ENCLAVE_CXXFLAGS) -Werror -fno-exceptions -fno-rtti -O0
CXXFLAGS += -Werror -fno-exceptions -fno-rtti -O0
# CPPFLAGS := -I$(COMMON_DIR)/inc/internal \
            -I$(COMMON_DIR)/inc          \
            -I$(COMMON_DIR)/inc/tlibc

BUILD_DIR = build
SRCS = $(wildcard *.cc)
OBJS = $(patsubst %.cc,$(BUILD_DIR)/%.o,$(SRCS))

TARGET := $(BUILD_DIR)/libsgx_mince.a

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p build

$(OBJS): $(BUILD_DIR)/%.o: %.cc
	@echo $(INCLUDES)
	$(CXX) $(INCLUDES) -c $< $(CXXFLAGS) -o $@

$(TARGET): $(OBJS)
	$(AR) rsD $@ $(OBJS)

.PHONY: clean

clean:
	rm -f $(OBJS) $(TARGET)