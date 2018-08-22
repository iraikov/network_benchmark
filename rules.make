
DRIVER_SRC_DIR   	:= src/driver
DRIVER_BUILD_DIR	:= build/driver
DRIVER_SRC      	:= $(foreach sdir,$(DRIVER_SRC_DIR),$(wildcard $(sdir)/*.cc))
DRIVER_OBJ       	:= $(patsubst src/%.cc,build/%.o,$(DRIVER_SRC))
DRIVER_INCLUDES  	:= $(addprefix -I,$(INC_DIR) include $(MPI_INCDIR))

MODULES   	:= data neuron
INC_DIR   	:= $(addprefix include/,$(MODULES))
SRC_DIR   	:= $(addprefix src/,$(MODULES))
BUILD_DIR	:= $(addprefix build/,$(MODULES))

SRC      	:= $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cc))
OBJ       	:= $(patsubst src/%.cc,build/%.o,$(SRC))
INCLUDES  	:= $(addprefix -I,$(INC_DIR) include $(HDF5_INCDIR) $(MPI_INCDIR))

vpath %.cc $(SRC_DIR):$(DRIVER_SRC_DIR)

define make-goal
$1/%.o: %.cc
	$(CC) -std=c++11 -Wall -Wno-unused-but-set-variable $(INCLUDES) -g -c $$< -o $$@
endef

.PHONY: all checkdirs clean

all: checkdirs $(DRIVER_OBJ) build/network_driver

build/network_driver: build/driver/network_driver.o build/libneuron.data.a build/libneuron.neuron.a
	$(LD) -o $@ $^ $(LINK_MPI) $(LINK_STDCPLUS)

build/libneuron.data.a: $(OBJ)
	$(AR) cr $@ $^

build/libneuron.neuron.a: $(OBJ)
	$(AR) cr $@ $^

checkdirs: $(BUILD_DIR) $(DRIVER_BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

$(DRIVER_BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf $(BUILD_DIR) $(DRIVER_BUILD_DIR)

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))
$(foreach bdir,$(DRIVER_BUILD_DIR),$(eval $(call make-goal,$(bdir))))
