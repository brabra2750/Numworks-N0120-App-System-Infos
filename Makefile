Q ?= @
CC = arm-none-eabi-gcc
BUILD_DIR = output
NWLINK = npx --yes -- nwlink@latest

define object_for
$(addprefix $(BUILD_DIR)/,$(addsuffix .o,$(basename $(1))))
endef

src = $(addprefix src/,\
  main.c \
)

CFLAGS = -std=c99 -Iinclude
CFLAGS += -mthumb -mcpu=cortex-m7 -mfloat-abi=hard -mfpu=fpv5-sp-d16
CFLAGS += -Os -Wall -ggdb -fdata-sections -ffunction-sections

LDFLAGS = -Wl,--relocatable -nostartfiles --specs=nano.specs
LDFLAGS += -Wl,-e,main -Wl,-u,eadk_app_name -Wl,-u,eadk_api_level -Wl,-u,eadk_app_icon
LDFLAGS += -Wl,--gc-sections

.PHONY: build run clean
build: $(BUILD_DIR)/app.nwa

run: $(BUILD_DIR)/app.nwa
	@echo "INSTALL $<"
	$(Q) $(NWLINK) install-nwa $<

$(BUILD_DIR)/app.nwa: $(call object_for,$(src)) $(BUILD_DIR)/icon.o
	@echo "LD      $@"
	$(Q) $(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(addprefix $(BUILD_DIR)/,%.o): %.c | $(BUILD_DIR)
	@echo "CC      $^"
	$(Q) $(CC) $(CFLAGS) -c $^ -o $@

$(BUILD_DIR)/icon.o: src/icon.png
	@echo "ICON    $<"
	$(Q) $(NWLINK) png-icon-o $< $@

.PRECIOUS: $(BUILD_DIR)
$(BUILD_DIR):
	@mkdir -p $@/src

.PHONY: clean
clean:
	@echo "CLEAN"
	@rm -rf $(BUILD_DIR)