# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  IPchan_config = debug

else ifeq ($(config),release)
  IPchan_config = release

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := IPchan

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

IPchan:
ifneq (,$(IPchan_config))
	@echo "==== Building IPchan ($(IPchan_config)) ===="
	@${MAKE} --no-print-directory -C . -f IPchan.make config=$(IPchan_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f IPchan.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   IPchan"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"