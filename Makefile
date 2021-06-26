FIRMWARE=1.2.1
OPTS=--followSymlinks --target $(FIRMWARE)
FLASH_OPT=--target $(FIRMWARE)
BIN=bin
SOURCE=modules
MODULES=main password morse simon wires

.PHONY: flash
flash: compile
	# particle flash se-4 $(FLASH_OPT) $(BIN)/module-morse.bin
	# particle flash se-6 $(FLASH_OPT) $(BIN)/module-morse.bin

	# particle flash se-0 $(FLASH_OPT) $(BIN)/module-simon.bin
	# particle flash se-b $(FLASH_OPT) $(BIN)/module-simon.bin

	# particle flash se-1 $(FLASH_OPT) $(BIN)/module-main.bin
	# particle flash se-9 $(FLASH_OPT) $(BIN)/module-main.bin

	# particle flash se-7 $(FLASH_OPT) $(BIN)/module-wires.bin
	# particle flash se-8 $(FLASH_OPT) $(BIN)/module-wires.bin

	# particle flash se-2 $(FLASH_OPT) $(BIN)/module-password.bin
	# particle flash se-a $(FLASH_OPT) $(BIN)/module-password.bin

.PHONY: compile
compile: clean bin $(addprefix $(BIN)/module-,$(addsuffix .bin, $(MODULES)))

$(BIN):
	mkdir $(BIN)
$(BIN)/%.bin: $(SOURCE)/%
	particle compile photon $(OPTS) "$<" --saveTo "$@"

.PHONY: clean
clean:
	@rm -vf $(BIN)/*.bin
